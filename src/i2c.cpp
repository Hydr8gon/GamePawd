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

#include "i2c.h"
#include "interrupts.h"

namespace I2c {
    uint32_t controls[4];
    uint32_t statuses[4];
    uint32_t irqEnable;
    uint32_t irqFlags;

    uint32_t dataCount;
    uint8_t deviceId;
    uint8_t command;

    void updateTransfer(int i);
}

void I2c::reset() {
    // Reset the registers
    memset(controls, 0, sizeof(controls));
    memset(statuses, 0, sizeof(statuses));
    irqFlags = 0;
    irqEnable = 0;

    // Reset the device state
    dataCount = 0;
    deviceId = 0;
    command = 0;
}

void I2c::updateTransfer(int i) {
    // Indicate that a transfer has completed if started
    if (~statuses[i] & 0x2) return;
    statuses[i] |= 0x4; // Acknowledge
    controls[i] &= ~0x20; // Data ready

    // Request an I2C interrupt if enabled
    if (~irqEnable & (2 << i)) return;
    irqFlags |= (2 << i);
    Interrupts::requestIrq(15);
}

uint32_t I2c::readIrqFlags() {
    // Read from the I2C interrupt flag register
    return irqFlags;
}

uint32_t I2c::readIrqEnable() {
    // Read from the I2C interrupt enable register
    return irqEnable;
}

uint32_t I2c::readData(int i) {
    // Update transfer state for the next byte
    updateTransfer(i);
    dataCount++;

    // Handle the current command for the selected device
    switch (uint8_t id = deviceId >> 1) {
    case 0x39: // LCD
        switch (command) {
        case 0xBF: // Read ID
            // Return the ID of a Panasonic LCD
            return (dataCount == 4) ? 0x02 : 0x00;

        default:
            // Handle unknown commands by doing nothing
            printf("Unimplemented LCD read with command 0x%X\n", command);
            return 0x00;
        }

    case 0x21: // Camera
        switch (command) {
            case 0x0A: return 0x77; // Product ID A
            case 0x0B: return 0x42; // Product ID B
            case 0x11: return 0x01; // Clock divider
            case 0x9C: return 0x1D; // Gamma Y-coord A
            case 0x9D: return 0x2E; // Gamma Y-coord B

        default:
            // Handle unknown commands by doing nothing
            printf("Unimplemented camera read with command 0x%X\n", command);
            return 0x00;
        }

    default:
        // Handle unknown devices by doing nothing
        printf("Unimplemented I2C read with device ID 0x%X\n", id);
        return 0x00;
    }
}

uint32_t I2c::readControl(int i) {
    // Read from one of the I2C control registers
    return controls[i];
}

uint32_t I2c::readStatus(int i) {
    // Read from one of the I2C status registers
    return statuses[i];
}

void I2c::writeIrqEnable(uint32_t mask, uint32_t value) {
    // Write to the I2C interrupt enable register
    irqEnable = (irqEnable & ~mask) | (value & mask);
}

void I2c::writeIrqAck(uint32_t mask, uint32_t value) {
    // Acknowledge I2C interrupt flags by clearing them
    irqFlags &= ~(value & mask);
}

void I2c::writeData(int i, uint32_t mask, uint32_t value) {
    // Update transfer state for the next byte
    updateTransfer(i);
    dataCount++;

    // Set the device ID or command depending on write count
    if (dataCount == 1)
        deviceId = value & mask;
    else if (dataCount == 2)
        command = value & mask;
}

void I2c::writeControl(int i, uint32_t mask, uint32_t value) {
    // Write to one of the I2C control registers, with stop masked out
    controls[i] = (controls[i] & ~mask) | (value & ~0x1 & mask);

    // Forward some bits to the unit's status register
    mask &= 0xF;
    statuses[i] = (statuses[i] & ~mask) | (value & mask);

    // Reset or initiate a transfer depending on state
    if (statuses[i] & 0x1) dataCount = 0;
    updateTransfer(i);
}
