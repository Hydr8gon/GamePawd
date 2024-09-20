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

#include <cstring>

#include "display.h"
#include "memory.h"

namespace Display {
    uint32_t palette[0x100];
    uint32_t fbAddress;
    uint8_t palAddress;
}

void Display::reset() {
    // Reset the palette and registers
    memset(palette, 0, sizeof(palette));
    fbAddress = 0;
    palAddress = 0;
}

uint32_t *Display::getBuffer() {
    // Build a basic framebuffer with a lot of assumptions
    uint32_t *buffer = new uint32_t[854 * 480];
    for (int y = 0; y < 480; y++)
        for (int x = 0; x < 854; x++)
            buffer[y * 854 + x] = palette[Memory::read<uint8_t>(fbAddress + y * 854 + x)];
    return buffer;
}

void Display::writeFbAddr(uint32_t mask, uint32_t value) {
    // Write to the framebuffer address register
    fbAddress = (fbAddress & ~mask) | (value & mask);
}

void Display::writePalAddr(uint32_t mask, uint32_t value) {
    // Write to the palette address register
    palAddress = (palAddress & ~mask) | (value & mask);
}

void Display::writePalData(uint32_t mask, uint32_t value) {
    // Write to a palette entry and move to the next one
    uint8_t r = (value & mask) >> 16;
    uint8_t g = (value & mask) >> 8;
    uint8_t b = (value & mask) >> 0;
    palette[palAddress++] = 0xFF000000 | (b << 16) | (g << 8) | r;
}
