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

#include "interrupts.h"
#include "arm9.h"

namespace Interrupts {
    uint32_t requestFlags;
    uint32_t enableMask;
    uint32_t irqIndex;
}

void Interrupts::reset() {
    // Reset the registers
    requestFlags = 0;
    enableMask = 0;
    irqIndex = 0;
}

void Interrupts::checkIrqs() {
    // Check interrupt conditions and trigger the first requested and enabled one
    if ((requestFlags & enableMask) && !(Arm9::cpsr & 0x80)) {
        for (irqIndex = 0; !(enableMask & requestFlags & (1 << irqIndex)); irqIndex++);
        Arm9::exception(0x18);
        printf("Interrupt %d triggered\n", irqIndex);
    }
}

void Interrupts::requestIrq(int i) {
    // Request an interrupt and check if one should trigger
    requestFlags |= (1 << i);
    checkIrqs();
}

uint32_t Interrupts::readIrqIndex() {
    // Read from the interrupt index register
    return irqIndex;
}

void Interrupts::writeIrqEnable(int i, uint32_t mask, uint32_t value) {
    // Enable or disable an interrupt and check if one should trigger
    enableMask = (enableMask & ~(1 << i)) | (!(value & mask & 0x40) << i);
    checkIrqs();
}

void Interrupts::writeIrqAck(uint32_t mask, uint32_t value) {
    // Stop requesting the most recently triggered interrupt
    requestFlags &= ~(1 << irqIndex);
}
