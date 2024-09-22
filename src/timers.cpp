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

#include "timers.h"
#include "interrupts.h"

namespace Timers {
    uint64_t timers[2];
    uint32_t controls[2];
    uint32_t reloads[2];
    uint32_t counter;
}

void Timers::reset() {
    // Reset the registers
    memset(timers, 0, sizeof(timers));
    memset(controls, 0, sizeof(controls));
    memset(reloads, 0, sizeof(reloads));
    counter = 0;
}

void Timers::tick() {
    // Increment the counter
    counter++;

    // Decrement enabled timers and trigger an interrupt on reload
    for (int i = 0; i < 2; i++) {
        if ((~controls[i] & 0x2) || timers[i]-- != 0) continue;
        timers[i] = reloads[i] << ((controls[i] >> 4) & 0x7);
        Interrupts::requestIrq(i);
    }
}

uint32_t Timers::readCounter() {
    // Read the current counter value
    return counter;
}

uint32_t Timers::readTimer(int i) {
    // Read one of the current timer values, adjusted for prescaling
    return timers[i] >> ((controls[i] >> 4) & 0x7);
}

void Timers::writeControl(int i, uint32_t mask, uint32_t value) {
    // Reload the timer, adjusted for prescaling, if it becomes enabled
    if ((~controls[i] & 0x2) && (value & mask & 0x2))
        timers[i] = reloads[i] << ((value >> 4) & 0x7);

    // Write to one of the timer control values
    controls[i] = (controls[i] & ~mask) | (value & mask);
}

void Timers::writeReload(int i, uint32_t mask, uint32_t value) {
    // Write to one of the timer reload values
    reloads[i] = (reloads[i] & ~mask) | (value & mask);
}
