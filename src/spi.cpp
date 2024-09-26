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

#include <algorithm>
#include <cstdio>

#include "spi.h"
#include "interrupts.h"
#include "memory.h"

namespace Spi {
    uint8_t *flashData;
    uint32_t flashAddr;
    uint32_t flashStart;
    uint32_t flashSize;

    uint32_t writeCount;
    uint32_t address;
    uint8_t flashStatus;
    uint8_t command;
    bool uicMode;

    uint32_t control;
    uint32_t irqFlags;
    uint32_t irqEnable;
    uint32_t readCount;
}

void Spi::reset() {
    // Reset the FLASH mapping
    delete[] flashData;
    flashAddr = 0;
    flashStart = 0;
    flashSize = 0;

    // Reset the internal registers
    writeCount = 0;
    address = 0;
    flashStatus = 0;
    command = 0;
    uicMode = false;

    // Reset the I/O registers
    control = 0;
    irqFlags = 0;
    irqEnable = 0;
    readCount = 0;

    // Boot from a FLASH dump or a firmware file mapped to FLASH
    if (FILE *file = fopen("flash.bin", "rb")) {
        // Load the FLASH dump into memory
        fseek(file, 0, SEEK_END);
        flashSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        flashData = new uint8_t[flashSize];
        fread(flashData, sizeof(uint8_t), flashSize, file);
        fclose(file);

        // Map the whole file directly to FLASH
        flashAddr = 0;
        flashStart = 0;

        // Get the size of the bootloader code
        uint32_t size = (flashData[0] | (flashData[1] << 8) | (flashData[2] << 16) | (flashData[3] << 24));
        if (!size) size = std::min(flashSize - 68, 0x10000U);
        printf("Found bootloader code with size 0x%X\n", size);

        // Copy the bootloader code into memory
        for (uint32_t i = 0; i < 64; i++)
            Memory::write<uint8_t>(i, flashData[i + 4]);
        for (uint32_t i = 0; i < size; i++)
            Memory::write<uint8_t>(0x3F0000 + i, flashData[i + 68]);
    }
    else if ((file = fopen("drc_fw.bin", "rb"))) {
        // Load the firmware file into memory
        fseek(file, 0, SEEK_END);
        flashSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        flashData = new uint8_t[flashSize];
        fread(flashData, sizeof(uint8_t), flashSize, file);
        fclose(file);

        // Determine start and end offsets of the ARM9 code
        uint32_t start = 0, end = 0;
        for (uint32_t i = 8; i < flashSize; i += 4) {
            // Find the start of the partition table
            uint8_t *data = &flashData[i];
            if (data[0] == 'I' && data[1] == 'N' && data[2] == 'D' && data[3] == 'X') {
                start = flashStart = i - 8;
                flashAddr = 0x100000;
                continue;
            }

            // Find the ARM9 code entry and parse offset and length
            if (start && data[0] == 'L' && data[1] == 'V' && data[2] == 'C' && data[3] == '_') {
                start += (data[-8] | (data[-7] << 8) | (data[-6] << 16) | (data[-5] << 24));
                end = start + (data[-4] | (data[-3] << 8) | (data[-2] << 16) | (data[-1] << 24));
                printf("Found firmware code at 0x%X with size 0x%X\n", start, end - start);
                break;
            }
        }

        // Copy the firmware code into memory, skipping the bootloader
        for (uint32_t i = start; i < end; i++)
            Memory::write<uint8_t>(i - start, flashData[i]);

        // Initialize values presumably set by the bootloader
        Memory::write<uint8_t>(0x3FFFFC, 0x79);
    }
}

uint32_t Spi::readControl() {
    // Read from the SPI control register
    return control;
}

uint32_t Spi::readIrqFlags() {
    // Read from the SPI interrupt flag register
    return irqFlags;
}

uint32_t Spi::readFifoStat() {
    // Report a word in the read FIFO during transfers, and an empty write FIFO always
    return ((readCount > 0) << 8) | 0x10;
}

uint32_t Spi::readData() {
    // Ensure there's data to read and the direction is correct
    if (!readCount || (~control & 0x2)) return 0;
    readCount--;

    // Trigger a read interrupt instantly if enabled
    if (irqEnable & 0x40) {
        irqFlags |= 0x40;
        Interrupts::requestIrq(6);
    }

    // Ignore reads from the UIC for now
    if (uicMode) {
        //printf("Unimplemented UIC read with command 0x%X\n", command);
        return 0x79;
    }

    // Handle the current FLASH command
    switch (command) {
    case 0x03: // Read
        // Return a byte from FLASH and increment the address
        if (address++ >= flashAddr && address <= flashAddr + flashSize - flashStart)
            return flashData[flashStart + address - flashAddr - 1];
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
        printf("Unimplemented FLASH read with command 0x%X\n", command);
        return 0;
    }
}

uint32_t Spi::readIrqEnable() {
    // Read from the SPI interrupt enable register
    return irqEnable;
}

void Spi::writeControl(uint32_t mask, uint32_t value) {
    // Write to the SPI control register
    control = (control & ~mask) | (value & mask);

    // Reset the write count if the chip is deselected
    if (control & 0x200)
        writeCount = 0;
}

void Spi::writeIrqFlags(uint32_t mask, uint32_t value) {
    // Acknowledge SPI interrupt flags by clearing them
    irqFlags &= ~(value & mask);
}

void Spi::writeData(uint32_t mask, uint32_t value) {
    // Ensure the transfer direction is correct
    if (control & 0x2) return;

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

    // Trigger a write interrupt instantly if enabled
    if (irqEnable & 0x80) {
        irqFlags |= 0x80;
        Interrupts::requestIrq(6);
    }

    // Handle FLASH commands with special behavior
    if (uicMode) return;
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

void Spi::writeIrqEnable(uint32_t mask, uint32_t value) {
    // Write to the SPI interrupt enable register
    irqEnable = (irqEnable & ~mask) | (value & mask);
}

void Spi::writeReadCount(uint32_t mask, uint32_t value) {
    // Write to the SPI read count register
    readCount = (readCount & ~mask) | (value & mask);
}

void Spi::writeUicGpio(uint32_t mask, uint32_t value) {
    // Select or deselect the UIC for SPI transfers
    if (value & mask & 0x200)
        uicMode = (~value & 0x100);
}
