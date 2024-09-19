/*
    Copyright 2024 Hydr8gon

    This file is part of GamePawd.

    GamePawd is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GamePawd is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GamePawd. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdio>

#include "spi.h"
#include "memory.h"

namespace Spi {
    uint8_t *firmware;
    uint32_t firmSize;
    uint32_t partStart;

    uint32_t writeCount;
    uint32_t address;
    uint8_t flashStatus;
    uint8_t command;

    uint32_t control;
    uint32_t readCount;
}

void Spi::reset() {
    // Reset the internal registers
    writeCount = 0;
    address = 0;
    flashStatus = 0;
    command = 0;

    // Reset the I/O registers
    control = 0;
    readCount = 0;

    // Parse the firmware so it can be mapped to FLASH
    if (FILE *file = fopen("drc_fw.bin", "rb")) {
        // Load the firmware file into memory
        fseek(file, 0, SEEK_END);
        firmSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        delete[] firmware;
        firmware = new uint8_t[firmSize];
        fread(firmware, sizeof(uint8_t), firmSize, file);
        fclose(file);

        // Determine start and end offsets of the ARM9 code
        uint32_t start = 0, end = 0;
        for (uint32_t i = 8; i < firmSize; i += 4) {
            // Find the start of the partition table
            uint8_t *data = &firmware[i];
            if (data[0] == 'I' && data[1] == 'N' && data[2] == 'D' && data[3] == 'X') {
                start = partStart = i - 8;
                continue;
            }

            // Find the ARM9 code entry and parse offset and length
            if (start && data[0] == 'L' && data[1] == 'V' && data[2] == 'C' && data[3] == '_') {
                start += (data[-8] | (data[-7] << 8) | (data[-6] << 16) | (data[-5] << 24));
                end = start + (data[-4] | (data[-3] << 8) | (data[-2] << 16) | (data[-1] << 24));
                printf("Found ARM9 code at 0x%X with size 0x%X\n", start, end - start);
                break;
            }
        }

        // Copy the ARM9 code into memory
        for (uint32_t i = start; i < end; i++)
            Memory::write(i - start, firmware[i]);
    }
}

uint32_t Spi::readControl() {
    // Read from the SPI control register
    return control;
}

uint32_t Spi::readFifoStat() {
    // Report a word in the read FIFO during transfers, and an empty write FIFO always
    return ((readCount > 0) << 8) | 0x10;
}

uint32_t Spi::readData() {
    // Ensure there's data to read and the direction is correct
    if (!readCount || (~control & 0x2)) return 0;
    readCount--;

    // Ignore reads from the UIC for now
    if (~control & 0x100) {
        printf("Unimplemented SPI read from UIC\n");
        return 0;
    }

    // Handle the current FLASH command
    switch (command) {
    case 0x03: // Read
        // Return a byte from FLASH and increment the address
        if (address++ >= 0x100000 && address <= 0x100000 + firmSize - partStart)
            return firmware[partStart + address - 0x100001];
        return 0;

    case 0x05: // Read status register
        // Return the current status value
        return flashStatus;

    case 0x9F: // Read ID
        // Return the ID byte for the current address
        switch (address++) {
            case 0: return 0x20;
            case 1: return 0xBA;
            case 2: return 0x19;
            default: return 0x00;
        }

    default:
        // Handle unknown commands by doing nothing
        printf("Unimplemented SPI read with command 0x%X\n", command);
        return 0;
    }
}

void Spi::writeControl(uint32_t mask, uint32_t value) {
    // Write to the SPI control register
    control = (control & ~mask) | (value & mask);

    // Reset the write count if the chip is deselected
    if (control & 0x200)
        writeCount = 0;
}

void Spi::writeData(uint32_t mask, uint32_t value) {
    // Ensure the transfer direction is correct
    if (control & 0x2) return;

    // Ignore writes to the UIC for now
    if (~control & 0x100) {
        printf("Unimplemented SPI write to UIC\n");
        return;
    }

    // Process the incoming data
    if (++writeCount == 1) {
        // Set the command byte on first write
        command = (value & mask);
        address = 0;
    }
    else if (writeCount < 6) {
        // Set an address byte on writes 2 to 5
        address |= (value & mask) << ((5 - writeCount) * 8);
    }

    // Handle FLASH commands with special behavior
    switch (command) {
    case 0x04: // Write disable
        // Clear the write enable bit
        flashStatus &= ~0x2;
        break;

    case 0x06: // Write enable
        // Set the write enable bit
        flashStatus |= 0x2;
        break;
    }
}

void Spi::writeReadCount(uint32_t mask, uint32_t value) {
    // Write to the SPI read count register
    readCount = (readCount & ~mask) | (value & mask);
}
