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

namespace Spi {
    void reset();
    void pressKey(int key);
    void releaseKey(int key);

    uint32_t readControl();
    uint32_t readIrqFlags();
    uint32_t readFifoStat();
    uint32_t readData();
    uint32_t readIrqEnable();

    void writeControl(uint32_t mask, uint32_t value);
    void writeIrqFlags(uint32_t mask, uint32_t value);
    void writeData(uint32_t mask, uint32_t value);
    void writeIrqEnable(uint32_t mask, uint32_t value);
    void writeReadCount(uint32_t mask, uint32_t value);
    void writeDevSelect(uint32_t mask, uint32_t value);
    void writeGpioFlash(uint32_t mask, uint32_t value);
    void writeGpioUic(uint32_t mask, uint32_t value);
}
