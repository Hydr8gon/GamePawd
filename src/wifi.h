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

namespace Wifi {
    void reset();

    uint32_t readResponse(int i);
    uint16_t readClockCtrl();
    uint32_t readIrqFlags();
    uint32_t readIrqEnable();

    void writeArgs(uint32_t mask, uint32_t value);
    void writeCommand(uint16_t mask, uint16_t value);
    void writeClockCtrl(uint16_t mask, uint16_t value);
    void writeIrqFlags(uint16_t mask, uint16_t value);
    void writeIrqEnable(uint16_t mask, uint16_t value);
}
