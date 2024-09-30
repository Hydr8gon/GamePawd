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

namespace I2c {
    void reset();

    uint32_t readIrqFlags();
    uint32_t readIrqEnable();
    uint32_t readData(int i);
    uint32_t readControl(int i);
    uint32_t readStatus(int i);

    void writeIrqEnable(uint32_t mask, uint32_t value);
    void writeIrqAck(uint32_t mask, uint32_t value);
    void writeData(int i, uint32_t mask, uint32_t value);
    void writeControl(int i, uint32_t mask, uint32_t value);
}
