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

namespace Memory {
    uint8_t ram[0x400000]; // 4MB RAM
}

void Memory::reset()
{
    // Reset the memory array
    memset(ram, 0, sizeof(ram));
}

template uint8_t Memory::read(uint32_t address);
template uint16_t Memory::read(uint32_t address);
template uint32_t Memory::read(uint32_t address);
template <typename T> T Memory::read(uint32_t address) {
    // Return an LSB-first value from data at an aligned RAM address
    if ((address &= ~(sizeof(T) - 1)) < 0x400000) {
        T value = 0;
        uint8_t *data = &ram[address];
        for (uint32_t i = 0; i < sizeof(T); i++)
            value |= data[i] << (i * 8);
        return value;
    }

    // Handle unknown reads by returning nothing
    printf("Unmapped memory read: 0x%X\n", address);
    return 0;
}

template void Memory::write(uint32_t address, uint8_t value);
template void Memory::write(uint32_t address, uint16_t value);
template void Memory::write(uint32_t address, uint32_t value);
template <typename T> void Memory::write(uint32_t address, T value) {
    // Write an LSB-first value to data at an aligned RAM address
    if ((address &= ~(sizeof(T) - 1)) < 0x400000) {
        uint8_t *data = &ram[address];
        for (uint32_t i = 0; i < sizeof(T); i++)
            data[i] = value >> (i * 8);
        return;
    }

    // Handle unknown writes by doing nothing
    printf("Unmapped memory write: 0x%X\n", address);
}
