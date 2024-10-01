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

namespace Dma {
    void reset();

    uint32_t readSpiCount();
    uint32_t readCount(int i);

    void writeSpiEnable(uint32_t mask, uint32_t value);
    void writeSpiControl(uint32_t mask, uint32_t value);
    void writeSpiCount(uint32_t mask, uint32_t value);
    void writeSpiAddress(uint32_t mask, uint32_t value);
    void writeEnable(int i, uint32_t mask, uint32_t value);
    void writeControl(int i, uint32_t mask, uint32_t value);
    void writeChunkSize(int i, uint32_t mask, uint32_t value);
    void writeSrcStride(int i, uint32_t mask, uint32_t value);
    void writeDstStride(int i, uint32_t mask, uint32_t value);
    void writeCount(int i, uint32_t mask, uint32_t value);
    void writeSrcAddr(int i, uint32_t mask, uint32_t value);
    void writeDstAddr(int i, uint32_t mask, uint32_t value);
    void writeSimpFill(int i, uint32_t mask, uint32_t value);
}
