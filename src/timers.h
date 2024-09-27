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

namespace Timers {
    extern uint32_t timerCycles;
    extern uint32_t countCycles;

    void reset();

    uint32_t readCounter();
    uint32_t readControl(int i);
    uint32_t readTimer(int i);

    void writeTimerScale(uint32_t mask, uint32_t value);
    void writeCountScale(uint32_t mask, uint32_t value);
    void writeCounter(uint32_t mask, uint32_t value);
    void writeControl(int i, uint32_t mask, uint32_t value);
    void writeTimer(int i, uint32_t mask, uint32_t value);
    void writeTarget(int i, uint32_t mask, uint32_t value);
}
