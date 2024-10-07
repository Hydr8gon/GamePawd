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
    uint8_t eeprom[0x800];
    uint8_t *flashData;
    uint32_t flashAddr;
    uint32_t flashStart;
    uint32_t flashSize;

    uint32_t writeCount;
    uint32_t address;
    uint8_t flashStatus;
    uint8_t command;
    uint8_t uicFwStatus;

    uint32_t control;
    uint32_t irqFlags;
    uint32_t irqEnable;
    uint32_t readCount;
    uint32_t devSelect;

    void calcCrc16(uint8_t *data, uint32_t size);
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
    uicFwStatus = 0x3F;

    // Reset the I/O registers
    control = 0;
    irqFlags = 0;
    irqEnable = 0;
    readCount = 0;
    devSelect = 0;

    // Build a barebones UIC EEPROM with essential data
    eeprom[0x100] = 0x00; // Board version
    calcCrc16(&eeprom[0x100], 0x1);
    eeprom[0x103] = 0x01; // Region
    calcCrc16(&eeprom[0x103], 0x1);
    eeprom[0x256] = 0x01; // Language bank
    calcCrc16(&eeprom[0x256], 0x4);

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
        uint32_t start = -1, end = 0;
        for (uint32_t i = 8; i < flashSize; i += 4) {
            // Find the start of the partition table
            uint8_t *data = &flashData[i];
            if (data[0] == 'I' && data[1] == 'N' && data[2] == 'D' && data[3] == 'X') {
                start = flashStart = i - 8;
                flashAddr = 0x100000;
                continue;
            }

            // Find the ARM9 code entry and parse offset and length
            if (start != -1 && data[0] == 'L' && data[1] == 'V' && data[2] == 'C' && data[3] == '_') {
                start += (data[-8] | (data[-7] << 8) | (data[-6] << 16) | (data[-5] << 24));
                end = start + (data[-4] | (data[-3] << 8) | (data[-2] << 16) | (data[-1] << 24));
                printf("Found firmware code at 0x%X with size 0x%X\n", start, end - start);
                break;
            }
        }

        // Copy the firmware code into memory, skipping the bootloader
        for (uint32_t i = start; i < end; i++)
            Memory::write<uint8_t>(i - start, flashData[i]);

        // Initialize values set by the bootloader
        Memory::write<uint8_t>(0x3FFFFC, 0x3F);
    }
}

void Spi::calcCrc16(uint8_t *data, uint32_t size) {
    // Calculate a CRC16 for the given data
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < size; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
            crc = (crc & 0x1) ? ((crc >> 1) ^ 0x8408) : (crc >> 1);
    }

    // Append the checksum to the end of the data
    data[size + 0] = crc >> 0;
    data[size + 1] = crc >> 8;
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
    // Report words in the read FIFO during transfers, and an empty write FIFO always
    return (std::min(readCount, 0x10U) << 8) | 0x10;
}

uint32_t Spi::readData() {
    // Ensure there's data to read and the direction is correct
    if (!readCount || (~control & 0x2)) return 0;
    readCount--;

    // Trigger a read interrupt if all remaining data fits in the FIFO
    if ((irqEnable & 0x40) && readCount <= 0x10) {
        irqFlags |= 0x40;
        Interrupts::requestIrq(6);
    }

    // Handle the current command for the selected device
    switch (devSelect) {
    case 0x1: // FLASH
        switch (command) {
        case 0x03: // Read
            // Return a byte from FLASH and increment the address
            if (address++ >= flashAddr && address <= flashAddr + flashSize - flashStart)
                return flashData[flashStart + address - flashAddr - 1];
            return 0x00;

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
            return 0x00;
        }

    case 0x2: // UIC
        switch (command) {
        case 0x03: // Read EEPROM
            // Return a byte from EEPROM and increment the address
            return eeprom[(((address += 0x10000) >> 16) - 0x1101) & 0x7FF];

        case 0x05: // Check expansion
            // Report no expansion device
            return 0x00;

        case 0x07: // Scan input
            // Stub to avoid getting stuck
            return (address++ == 0x7F) ? 0xFF : 0x00;

        case 0x0B: // Firmware version
            // Return the version byte for the current address
            switch (address++) {
                case 0: return 0x28;
                case 3: return 0x58;
                default: return 0x00;
            }

        case 0x13: // Unknown
            // Stub to avoid getting stuck
            return 0x01;

        case 0x7F: // Firmware status
            // Return the current firmware status
            return uicFwStatus;

        default:
            // Handle unknown commands by doing nothing
            printf("Unimplemented UIC read with command 0x%X\n", command);
            return 0x79;
        }

    default:
        // Handle invalid devices by doing nothing
        //printf("SPI read with invalid device selection: 0x%X\n", devSelect);
        return 0x00;
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

    // Handle commands with special behavior
    switch (devSelect) {
    case 0x1: // FLASH
        switch (command) {
        case 0x04: // Write disable
            // Clear the write enable bit
            flashStatus &= ~0x2;
            return;

        case 0x06: // Write enable
            // Set the write enable bit
            flashStatus |= 0x2;
            return;
        }

    case 0x2: // UIC
        switch (command) {
        case 0x09: // Begin update
            // Change the firmware status to be ready for update
            uicFwStatus = 0x79;
            return;
        }
    }
}

void Spi::writeIrqEnable(uint32_t mask, uint32_t value) {
    // Write to the SPI interrupt enable register
    irqEnable = (irqEnable & ~mask) | (value & mask);
}

void Spi::writeReadCount(uint32_t mask, uint32_t value) {
    // Write to the SPI read count register
    readCount = (readCount & ~mask) | (value & mask);

    // Trigger a read interrupt instantly if all data fits in the FIFO
    if ((irqEnable & 0x40) && readCount <= 0x10) {
        irqFlags |= 0x40;
        Interrupts::requestIrq(6);
    }
}

void Spi::writeDevSelect(uint32_t mask, uint32_t value) {
    // Write to the SPI device select register
    devSelect = (devSelect & ~mask) | (value & mask);
}

void Spi::writeGpioFlash(uint32_t mask, uint32_t value) {
    // Select or deselect the FLASH for SPI transfers
    if (value & mask & 0x200)
        devSelect = (devSelect & ~0x1) | ((~value >> 8) & 0x1);
}

void Spi::writeGpioUic(uint32_t mask, uint32_t value) {
    // Select or deselect the UIC for SPI transfers
    if (value & mask & 0x200)
        devSelect = (devSelect & ~0x2) | ((~value >> 7) & 0x2);
}
