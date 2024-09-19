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
#include <cstring>

#include "memory.h"
#include "display.h"
#include "spi.h"

// Defines a 32-bit register in an I/O switch statement
#define DEF_IO32(addr, func) \
case addr + 0: case addr + 1: \
case addr + 2: case addr + 3: \
    base -= addr; \
    size = 4; \
    func; \
    break;

// Defines shared parameters for I/O register writes
#define IOWR_PARAMS mask << (base * 8), data << (base * 8)

namespace Memory {
    uint8_t ram[0x400000]; // 4MB RAM
    uint32_t counter;

    template <typename T> T ioRead(uint32_t address);
    template <typename T> void ioWrite(uint32_t address, T value);
}

void Memory::reset()
{
    // Reset the memory array
    memset(ram, 0, sizeof(ram));
    counter = 0;
}

template uint8_t Memory::read(uint32_t address);
template uint16_t Memory::read(uint32_t address);
template uint32_t Memory::read(uint32_t address);
template <typename T> T Memory::read(uint32_t address) {
    // Read an LSB-first value from an aligned RAM address or I/O register
    if ((address &= ~(sizeof(T) - 1)) < 0x400000) {
        T value = 0;
        uint8_t *data = &ram[address];
        for (uint32_t i = 0; i < sizeof(T); i++)
            value |= data[i] << (i * 8);
        return value;
    }
    else if ((address >> 28) == 0xF) {
        return ioRead<T>(address);
    }

    // Handle unknown reads by returning nothing
    printf("Unmapped memory read: 0x%X\n", address);
    return 0;
}

template void Memory::write(uint32_t address, uint8_t value);
template void Memory::write(uint32_t address, uint16_t value);
template void Memory::write(uint32_t address, uint32_t value);
template <typename T> void Memory::write(uint32_t address, T value) {
    // Write an LSB-first value to an aligned RAM address or I/O register
    if ((address &= ~(sizeof(T) - 1)) < 0x400000) {
        uint8_t *data = &ram[address];
        for (uint32_t i = 0; i < sizeof(T); i++)
            data[i] = value >> (i * 8);
        return;
    }
    else if ((address >> 28) == 0xF) {
        return ioWrite<T>(address, value);
    }

    // Handle unknown writes by doing nothing
    printf("Unmapped memory write: 0x%X\n", address);
}

template <typename T> T Memory::ioRead(uint32_t address) {
    // Read a value from one or more I/O registers
    T value = 0;
    for (uint32_t i = 0; i < sizeof(T);) {
        // Load data from a register
        uint32_t base, size, data;
        switch (base = address + i) {
            DEF_IO32(0xF0004404, data = Spi::readControl())
            DEF_IO32(0xF000440C, data = Spi::readFifoStat())
            DEF_IO32(0xF0004410, data = Spi::readData())
            DEF_IO32(0xF0000408, data = ++counter) // TODO: unstub

        default:
            // Handle unknown reads by returning nothing
            if (i == 0) {
                printf("Unknown I/O register read: 0x%X\n", address);
                return 0;
            }

            // Ignore unknown reads after the first byte
            i++;
            continue;
        }

        // Add data to the return value and adjust byte offset
        value |= (data >> (base * 8)) << (i * 8);
        i += size - base;
    }
    return value;
}

template <typename T> void Memory::ioWrite(uint32_t address, T value) {
    // Write a value to one or more I/O registers
    for (uint32_t i = 0; i < sizeof(T);) {
        // Store data to a register
        uint32_t base, size, data = value >> (i * 8);
        uint32_t mask = (1ULL << ((sizeof(T) - i) * 8)) - 1;
        switch (base = address + i) {
            DEF_IO32(0xF0004404, Spi::writeControl(IOWR_PARAMS))
            DEF_IO32(0xF0004410, Spi::writeData(IOWR_PARAMS))
            DEF_IO32(0xF0004420, Spi::writeReadCount(IOWR_PARAMS))
            DEF_IO32(0xF0009474, Display::writeFbAddr(IOWR_PARAMS))
            DEF_IO32(0xF0009500, Display::writePalAddr(IOWR_PARAMS))
            DEF_IO32(0xF0009504, Display::writePalData(IOWR_PARAMS))

        default:
            // Handle unknown writes by doing nothing
            if (i == 0) {
                printf("Unknown I/O register write: 0x%X\n", address);
                return;
            }

            // Ignore unknown writes after the first byte
            i++;
            continue;
        }

        // Adjust the byte offset
        i += size - base;
    }
}
