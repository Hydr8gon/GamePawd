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

#include "wifi.h"

namespace Wifi {
    uint32_t response[4];
    uint32_t args;
    uint32_t irqFlags;
    uint32_t irqEnable;
    uint16_t clockControl;
    uint8_t clockCsr;
}

void Wifi::reset() {
    // Reset the registers
    memset(response, 0, sizeof(response));
    args = 0;
    irqFlags = 0;
    irqEnable = 0;
    clockControl = 0;
    clockCsr = 0;
}

uint32_t Wifi::readResponse(int i) {
    // Read from one of the SDIO response registers
    return response[i];
}

uint16_t Wifi::readClockCtrl() {
    // Read from the SDIO clock control register
    return clockControl;
}

uint32_t Wifi::readIrqFlags() {
    // Read from the SDIO interrupt flags register
    return irqFlags;
}

uint32_t Wifi::readIrqEnable() {
    // Read from the SDIO interrupt enable register
    return irqEnable;
}

void Wifi::writeArgs(uint32_t mask, uint32_t value) {
    // Write to the SDIO argument registers
    args = (args & ~mask) | (value & mask);
}

void Wifi::writeCommand(uint16_t mask, uint16_t value) {
    // Trigger a command complete interrupt instantly if enabled
    if (irqEnable & 0x1)
        irqFlags |= 0x1;

    switch (uint8_t cmd = ((value & mask) >> 8) & 0x3F) {
    case 5: // Operation condition
        // Report one I/O function and a ready card
        response[0] = 0x90000000;
        return;

    case 7: // Select card
        // Return the expected response
        response[0] = 0x1E00;
        return;

    case 52: // Single-byte transfer
        // Write or read a single byte to WiFi registers
        if (args & 0x80000000) { // Write
            switch (uint32_t addr = (args >> 9) & 0x1FFFF) {
            case 0x1000E: // Clock CSR
                // Write to the clock CSR register and set ready bits instantly
                clockCsr = (clockCsr & ~0x3F) | (args & 0x3F);
                clockCsr |= (clockCsr << 3) & 0xC0;
                return;

            default:
                // Handle unknown writes by doing nothing
                printf("Unknown WiFi register write: 0x%X @ 0x%X\n", args & 0xFF, addr);
                response[0] = 0;
                return;
            }
        }
        else { // Read
            switch (uint32_t addr = (args >> 9) & 0x1FFFF) {
            case 0x1000E: // Clock CSR
                // Read from the clock CSR register
                response[0] = clockCsr;
                return;

            default:
                // Handle unknown reads by doing nothing
                printf("Unknown WiFi register read: 0x%X\n", addr);
                response[0] = 0;
                return;
            }
        }

    default:
        // Handle unknown commands by doing nothing
        printf("Unknown WiFi command: %d\n", cmd);
        return;
    }
}

void Wifi::writeClockCtrl(uint16_t mask, uint16_t value) {
    // Write to the SDIO clock control register
    clockControl = (clockControl & ~mask) | (value & mask);

    // Report a stable clock instantly if enabled
    clockControl |= (clockControl << 1) & 0x2;
}

void Wifi::writeIrqFlags(uint16_t mask, uint16_t value) {
    // Acknowledge SDIO interrupt flags by clearing them
    irqFlags &= ~(value & mask);
}

void Wifi::writeIrqEnable(uint16_t mask, uint16_t value) {
    // Write to the SDIO interrupt enable register
    irqEnable = (irqEnable & ~mask) | (value & mask);
}
