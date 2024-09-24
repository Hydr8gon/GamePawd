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

#include "i2c.h"
#include "interrupts.h"

namespace I2c {
    uint32_t statuses[4];
    uint32_t irqEnable;
    uint32_t irqFlags;

    void requestIrq(int i);
}

void I2c::reset() {
    // Reset the registers
    memset(statuses, 0, sizeof(statuses));
    irqFlags = 0;
    irqEnable = 0;
}

void I2c::requestIrq(int i) {
    // Request an I2C interrupt if enabled
    if (~irqEnable & (2 << i)) return;
    irqFlags |= (2 << i);
    Interrupts::requestIrq(15);
}

uint32_t I2c::readIrqFlags() {
    // Read from the I2C interrupt flag register
    return irqFlags;
};

uint32_t I2c::readIrqEnable() {
    // Read from the I2C interrupt enable register
    return irqEnable;
};

uint32_t I2c::readData(int i) {
    // Stub I2C data reads by simply triggering an interrupt
    requestIrq(i);
    return 0;
};

uint32_t I2c::readStatus(int i) {
    // Read from one of the I2C status registers
    return statuses[i];
};

void I2c::writeIrqEnable(uint32_t mask, uint32_t value) {
    // Write to the I2C interrupt enable register
    irqEnable = (irqEnable & ~mask) | (value & mask);
}

void I2c::writeIrqAck(uint32_t mask, uint32_t value) {
    // Acknowledge I2C interrupt flags by clearing them
    irqFlags &= ~(value & mask);
}

void I2c::writeData(int i, uint32_t mask, uint32_t value) {
    // Stub I2C data writes by simply triggering an interrupt
    requestIrq(i);
}

void I2c::writeControl(int i, uint32_t mask, uint32_t value) {
    // Stub I2C control writes by setting some bits and triggering an interrupt
    statuses[i] = (mask & value & 0x1) ? 0x1 : ((mask & value & 0x2) ? 0xC : 0x4);
    requestIrq(i);
}
