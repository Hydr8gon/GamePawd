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

#pragma once

#include <cstdint>

namespace Display {
    void reset();
    uint32_t *getBuffer();

    uint32_t readFbXOfs();
    uint32_t readFbWidth();
    uint32_t readFbYOfs();
    uint32_t readFbHeight();
    uint32_t readFbAddr();

    void writeFbXOfs(uint32_t mask, uint32_t value);
    void writeFbWidth(uint32_t mask, uint32_t value);
    void writeFbYOfs(uint32_t mask, uint32_t value);
    void writeFbHeight(uint32_t mask, uint32_t value);
    void writeFbStride(uint32_t mask, uint32_t value);
    void writeFbAddr(uint32_t mask, uint32_t value);
    void writePixelFmt(uint32_t mask, uint32_t value);
    void writePalAddr(uint32_t mask, uint32_t value);
    void writePalData(uint32_t mask, uint32_t value);
}
