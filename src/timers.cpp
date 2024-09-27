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
#include "core.h"
#include "interrupts.h"

namespace Timers {
    uint8_t shifts[2];
    uint32_t timerCycles;
    uint32_t countCycles;

    uint64_t timers[2];
    uint32_t controls[2];
    uint32_t targets[2];
    uint32_t timerScale;
    uint32_t countScale;
    uint32_t counter;

    void tickTimers();
    void tickCounter();
}

void Timers::reset() {
    // Reset the prescale values
    memset(shifts, 0, sizeof(shifts));
    countCycles = 0;
    timerCycles = 0;

    // Reset the I/O registers
    memset(timers, 0, sizeof(timers));
    memset(controls, 0, sizeof(controls));
    memset(targets, 0, sizeof(targets));
    timerScale = 0;
    countScale = 0;
    counter = 0;

    // Schedule initial tasks
    timerCycles = Core::schedule(tickTimers, timerScale + 1);
    countCycles = Core::schedule(tickCounter, countScale + 1);
}

void Timers::tickTimers() {
    // Verify timestamp and schedule the next tick
    if (timerCycles != Core::globalCycles) return;
    timerCycles = Core::schedule(tickTimers, timerScale + 1);

    // Increment enabled timers and trigger an interrupt on reload
    for (int i = 0; i < 2; i++) {
        if ((controls[i] & 0x2) && (timers[i]++ >> shifts[i]) == targets[i]) {
            Interrupts::requestIrq(i);
            timers[i] = 0;
        }
    }
}

void Timers::tickCounter() {
    // Verify timestamp and schedule the next tick
    if (countCycles != Core::globalCycles) return;
    countCycles = Core::schedule(tickCounter, countScale + 1);

    // Increment the counter
    counter++;
}

uint32_t Timers::readCounter() {
    // Read the current counter value
    return counter;
}

uint32_t Timers::readControl(int i) {
    // Read from one of the timer control registers
    return controls[i];
}

uint32_t Timers::readTimer(int i) {
    // Read one of the current timer values, adjusted for prescaling
    return timers[i] >> shifts[i];
}

void Timers::writeTimerScale(uint32_t mask, uint32_t value) {
    // Write to the timer prescale register and reschedule its next tick
    timerScale = (timerScale & ~mask) | (value & mask);
    timerCycles = Core::schedule(tickTimers, timerScale + 1);
}

void Timers::writeCountScale(uint32_t mask, uint32_t value) {
    // Write to the counter prescale register and reschedule its next tick
    countScale = (countScale & ~mask) | (value & mask);
    countCycles = Core::schedule(tickCounter, countScale + 1);
}

void Timers::writeCounter(uint32_t mask, uint32_t value) {
    // Write a new value to the counter
    counter = (counter & ~mask) | (value & mask);
}

void Timers::writeControl(int i, uint32_t mask, uint32_t value) {
    // Write to one of the timer control registers and reset the timer if disabled
    controls[i] = (controls[i] & ~mask) | (value & mask);
    if (~controls[i] & 0x2) timers[i] = 0;

    // Set the prescale shift and adjust the timer if it changed
    uint8_t shift = ((controls[i] >> 4) & 0x7) + 1;
    if (shifts[i] == shift) return;
    timers[i] = (timers[i] >> shifts[i]) << shift;
    shifts[i] = shift;
}

void Timers::writeTimer(int i, uint32_t mask, uint32_t value) {
    // Write one of the current timer values, adjusted for prescaling
    timers[i] = (((timers[i] >> shifts[i]) & ~mask) | (value & mask)) << shifts[i];
}

void Timers::writeTarget(int i, uint32_t mask, uint32_t value) {
    // Write to one of the timer target registers
    targets[i] = (targets[i] & ~mask) | (value & mask);
}
