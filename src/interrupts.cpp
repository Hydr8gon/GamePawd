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

#include "interrupts.h"
#include "arm9.h"
#include "core.h"

namespace Interrupts {
    uint32_t irqEnables[32];
    uint32_t requestFlags;
    uint32_t enableMask;
    uint32_t priorityMask;
    uint32_t irqIndex;
}

void Interrupts::reset() {
    // Reset the registers
    memset(irqEnables, 0, sizeof(irqEnables));
    requestFlags = 0;
    enableMask = 0;
    priorityMask = 0;
    irqIndex = 0;
}

void Interrupts::checkIrqs() {
    // Ensure interrupts are enabled and one is actually requested
    if ((Arm9::cpsr & 0x80) || !(enableMask & requestFlags) || !priorityMask)
        return;

    // Trigger an exception on the first enabled interrupt with high enough priority, if any
    for (irqIndex = 0; irqIndex < 31; irqIndex++) {
        if (!(enableMask & requestFlags & (1 << irqIndex))) continue;
        if ((irqEnables[irqIndex] & 0xF) >= priorityMask) continue;
        Arm9::exception(0x18);
        //printf("Interrupt %d triggered\n", irqIndex);
        return;
    }
}

void Interrupts::requestIrq(int i) {
    // Request an interrupt and check if one should trigger
    requestFlags |= (1 << i);
    Core::schedule(checkIrqs, 1);
}

uint32_t Interrupts::readIrqEnable(int i) {
    // Read from one of the interrupt enable registers
    return irqEnables[i];
}

uint32_t Interrupts::readIrqIndex() {
    // Read from the interrupt index register
    return irqIndex;
}

uint32_t Interrupts::readPrioMask() {
    // Read from the interrupt priority mask register
    return priorityMask;
}

uint32_t Interrupts::readPrioClear() {
    // Read the interrupt priority mask and clear it
    uint32_t value = priorityMask;
    priorityMask = 0;
    return value;
}

void Interrupts::writeIrqEnable(int i, uint32_t mask, uint32_t value) {
    // Write to one of the interrupt enable registers
    irqEnables[i] = (irqEnables[i] & ~mask) | (value & mask);

    // Track enabled interrupts and check if one should trigger
    enableMask = (enableMask & ~(1 << i)) | (!(irqEnables[i] & 0x40) << i);
    Core::schedule(checkIrqs, 1);
}

void Interrupts::writePrioMask(uint32_t mask, uint32_t value) {
    // Write to the interrupt priority mask register
    mask &= 0xF;
    priorityMask = (priorityMask & ~mask) | (value & mask);

    // Acknowledge the most recent interrupt and check if one should trigger
    requestFlags &= ~(1 << irqIndex);
    Core::schedule(checkIrqs, 1);
}
