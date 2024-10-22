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

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "wifi.h"

namespace Wifi {
    uint32_t response[4];
    uint32_t args;
    uint32_t irqFlags;
    uint32_t irqEnable;
    uint16_t clockControl;

    uint32_t f1Address;
    uint8_t clockCsr;

    uint32_t bufferAddr;
    uint16_t bufferSize;
    uint8_t bufferFunc;

    extern const uint8_t erom[0x100];

    void requestIrq(int i);
    uint8_t readByte(uint8_t func, uint32_t address, bool first = true);
    void writeByte(uint8_t func, uint32_t address, uint8_t value, bool first = true);
}

// Enumeration ROM that defines WiFi cores
const uint8_t Wifi::erom[] = {
    0x01, 0x00, 0xF8, 0x4B, 0x11, 0x42, 0x00, 0x23, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x18, // 0x00-0x0F
    0xC5, 0x00, 0x10, 0x18, 0x01, 0x12, 0xF8, 0x4B, 0x11, 0x42, 0x00, 0x16, 0x03, 0x01, 0x00, 0x00, // 0x10-0x1F
    0x05, 0x10, 0x00, 0x18, 0xC5, 0x10, 0x10, 0x18, 0x01, 0x29, 0xF8, 0x4B, 0x11, 0x42, 0x00, 0x03, // 0x20-0x2F
    0x03, 0x02, 0x00, 0x00, 0x05, 0x20, 0x00, 0x18, 0xC5, 0x20, 0x10, 0x18, 0x01, 0x2A, 0xF8, 0x4B, // 0x30-0x3F
    0x11, 0x42, 0x00, 0x02, 0x03, 0x03, 0x00, 0x00, 0x05, 0x30, 0x00, 0x18, 0xC5, 0x30, 0x10, 0x18, // 0x40-0x4F
    0x01, 0x0E, 0xF8, 0x4B, 0x01, 0x04, 0x08, 0x06, 0x05, 0x40, 0x00, 0x18, 0x35, 0x01, 0x00, 0x00, // 0x50-0x5F
    0x00, 0x80, 0x04, 0x00, 0x35, 0x01, 0x00, 0x10, 0x00, 0x80, 0x04, 0x00, 0x35, 0x01, 0x00, 0x1E, // 0x60-0x6F
    0x00, 0x00, 0x02, 0x00, 0x85, 0x41, 0x10, 0x18, 0x01, 0x1A, 0xF8, 0x4B, 0x11, 0x42, 0x00, 0x08, // 0x70-0x7F
    0x03, 0x04, 0x00, 0x00, 0x05, 0x50, 0x00, 0x18, 0xC5, 0x50, 0x10, 0x18, 0x01, 0x35, 0xB1, 0x43, // 0x80-0x8F
    0x01, 0x02, 0x08, 0x00, 0x75, 0x00, 0x00, 0x18, 0x00, 0x00, 0x01, 0x00, 0x85, 0x60, 0x10, 0x18, // 0x90-0x9F
    0x01, 0x35, 0xB1, 0x43, 0x01, 0x02, 0x08, 0x00, 0x75, 0x00, 0x10, 0x18, 0x00, 0x00, 0x01, 0x00, // 0xA0-0xAF
    0x85, 0x70, 0x10, 0x18, 0x01, 0x67, 0xB3, 0x43, 0x01, 0x02, 0x00, 0x00, 0x05, 0x80, 0x10, 0x18, // 0xB0-0xBF
    0x01, 0x66, 0xB3, 0x43, 0x01, 0x02, 0x00, 0x00, 0x05, 0x90, 0x10, 0x18, 0x01, 0x01, 0xB3, 0x43, // 0xC0-0xCF
    0x01, 0x02, 0x00, 0x11, 0x05, 0x60, 0x00, 0x18, 0x01, 0xFF, 0xBF, 0x43, 0x01, 0x02, 0x08, 0x00, // 0xD0-0xDF
    0x35, 0x70, 0x00, 0x18, 0x00, 0x90, 0x0F, 0x00, 0x35, 0xB0, 0x10, 0x18, 0x00, 0x50, 0xEF, 0x05, // 0xE0-0xEF
    0x35, 0x40, 0x01, 0x1E, 0x00, 0xC0, 0xFE, 0x01, 0x85, 0xA0, 0x10, 0x18, 0x0F, 0x00, 0x00, 0x00, // 0xF0-0xFF
};

void Wifi::reset() {
    // Reset the I/O registers
    memset(response, 0, sizeof(response));
    args = 0;
    irqFlags = 0;
    irqEnable = 0;
    clockControl = 0;

    // Reset the function 1 registers
    f1Address = 0;
    clockCsr = 0;

    // Reset the device state
    bufferAddr = 0;
    bufferSize = 0;
    bufferFunc = 0;
}

void Wifi::requestIrq(int i) {
    // Set an interrupt flag if it's enabled
    if (irqEnable & (1 << i))
        irqFlags |= (1 << i);
}

uint8_t Wifi::readByte(uint8_t func, uint32_t address, bool first) {
    // Ensure the read is from an implemented function
    if (func != 1) {
        if (first)
            printf("Read from unimplemented WiFi function: %d\n", func);
        return 0x00;
    }

    // Read a value from function 1's 32KB window into 32-bit address space
    if (address < 0x10000) {
        // Read a byte from the EROM
        address = f1Address + (address & 0x7FFF);
        if (address - 0x18109000 < 0x100) {
            return erom[address - 0x18109000];
        }

        // Read a byte from a register
        switch (address) {
        case 0x18000000: case 0x18000001: case 0x18000002: case 0x18000003: // Chip ID
            // Return a byte of the 32-bit chip ID register
            return 0x16914319 >> ((address & 0x3) * 8);

        case 0x18000004: case 0x18000005: case 0x18000006: case 0x18000007: // Chip capabilities
            // Return a byte of the 32-bit chip capabilities register
            return 0x10480009 >> ((address & 0x3) * 8);

        case 0x180000FC: case 0x180000FD: case 0x180000FE: case 0x180000FF: // EROM address
            // Return a byte of the 32-bit EROM address register
            return 0x18109000 >> ((address & 0x3) * 8);

        case 0x18000604: case 0x18000605: case 0x18000606: case 0x18000607: // PMU capabilities
            // Return a byte of the 32-bit PMU capabilities register
            return 0x19CC3607 >> ((address & 0x3) * 8);

        case 0x18004000: case 0x18004001: case 0x18004002: case 0x18004003: // SOCRAM info
            // Return a byte of the 32-bit SOCRAM info register
            return 0x00258033 >> ((address & 0x3) * 8);

        default:
            // Handle unknown reads by returning nothing
            if (first)
                printf("Unmapped WiFi memory read: 0x%X\n", address);
            return 0x00;
        }
    }

    // Read a value from one of function 1's registers
    switch (address) {
        case 0x1000A: return f1Address >> 8; // Window address A
        case 0x1000B: return f1Address >> 16; // Window address B
        case 0x1000C: return f1Address >> 24; // Window address C
        case 0x1000E: return clockCsr; // Clock CSR

    default:
        // Handle unknown reads by returning nothing
        if (first)
            printf("Unknown WiFi function 1 read: 0x%X\n", address);
        return 0x00;
    }
}

void Wifi::writeByte(uint8_t func, uint32_t address, uint8_t value, bool first) {
    // Ensure the write is to an implemented function
    if (func != 1) {
        if (first)
            printf("Write to unimplemented WiFi function: %d\n", func);
        return;
    }

    // Write a value to function 1's 32KB window into 32-bit address space
    if (address < 0x10000) {
        if (first)
            printf("Unmapped WiFi memory write: 0x%X\n", f1Address + (address & 0x7FFF));
        return;
    }

    // Write a value to one of function 1's registers
    switch (address) {
    case 0x1000A: // Window address A
        // Write to bit 15 of the window address
        f1Address = (f1Address & ~0x8000) | ((value << 8) & 0x8000);
        return;

    case 0x1000B: // Window address B
        // Write to bits 16-23 of the window address
        f1Address = (f1Address & ~0xFF0000) | ((value << 16) & 0xFF0000);
        return;

    case 0x1000C: // Window address C
        // Write to bits 24-31 of the window address
        f1Address = (f1Address & ~0xFF000000) | ((value << 24) & 0xFF000000);
        return;

    case 0x1000E: // Clock CSR
        // Write to the clock CSR register and set ready bits instantly
        clockCsr = (clockCsr & ~0x3F) | (args & 0x3F);
        clockCsr |= (clockCsr << 3) & 0xC0;
        return;

    default:
        // Handle unknown writes by doing nothing
        if (first)
            printf("Unknown WiFi function 1 write: 0x%X @ 0x%X\n", value, address);
        return;
    }
}

uint32_t Wifi::readResponse(int i) {
    // Read from one of the SDIO response registers
    return response[i];
}

uint32_t Wifi::readBufferData() {
    // Trigger a transfer complete interrupt when the last data is read
    uint32_t size = std::min<uint16_t>(bufferSize, 4);
    if (!(bufferSize -= size))
        requestIrq(1);

    // Read an LSB-first value from a WiFi function
    uint32_t value = 0;
    for (int i = 0; i < size; i++)
        value |= readByte(bufferFunc, bufferAddr++, i == 0) << (i * 8);
    return value;
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
    requestIrq(0);

    switch (uint8_t cmd = ((value & mask) >> 8) & 0x3F) {
    case 5: // Operation condition
        // Report a ready card and two functions
        response[0] = 0xA0FE0000;
        return;

    case 7: // Select card
        // Return the expected response
        response[0] = 0x1E00;
        return;

    case 52: // Single-byte transfer
        // Write/read a single byte to/from a WiFi function
        if (args & 0x80000000) { // Write
            writeByte((args >> 28) & 0x7, (args >> 9) & 0x1FFFF, args & 0xFF);
            response[0] = 0;
        }
        else { // Read
            response[0] = readByte((args >> 28) & 0x7, (args >> 9) & 0x1FFFF);
        }
        return;

    case 53: // Multi-byte transfer
        // Set parameters for a multi-byte transfer
        bufferAddr = (args >> 9) & 0x1FFFF;
        bufferSize = (args >> 0) & 0x1FF;
        bufferFunc = (args >> 28) & 0x7;

        // Trigger a read/write ready interrupt instantly
        requestIrq(5 - (args >> 31));
        response[0] = 0;
        return;

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
