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
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "display.h"
#include "core.h"
#include "interrupts.h"
#include "memory.h"

namespace Display {
    std::queue<uint32_t*> buffers;
    std::mutex mutex;

    uint32_t palette[0x100];
    uint32_t fbXOffset;
    uint32_t fbWidth;
    uint32_t fbYOffset;
    uint32_t fbHeight;
    uint32_t fbStride;
    uint32_t fbAddress;
    uint32_t pixelFormat;
    uint8_t palAddress;

    void drawFrame();
}

void Display::reset() {
    // Reset the palette and registers
    memset(palette, 0, sizeof(palette));
    fbXOffset = 0;
    fbWidth = 0;
    fbYOffset = 0;
    fbHeight = 0;
    fbStride = 0;
    fbAddress = 0;
    pixelFormat = 0;
    palAddress = 0;

    // Schedule initial tasks
    Core::schedule(drawFrame, 108000000 / 60);
}

uint32_t *Display::getBuffer() {
    // Get the next framebuffer for display if one is queued
    uint32_t *buffer = nullptr;
    mutex.lock();
    if (!buffers.empty()) {
        buffer = buffers.front();
        buffers.pop();
    }
    mutex.unlock();
    return buffer;
}

void Display::drawFrame() {
    // Create a new framebuffer and clear it
    uint32_t *buffer = new uint32_t[854 * 480];
    memset(buffer, 0, 854 * 480 * 4);

    // Render a buffer from memory with the current parameters
    uint32_t w = std::min(fbWidth, 854U), h = std::min(fbHeight, 480U);
    switch (uint8_t fmt = pixelFormat & 0x3) {
    case 0: // 8-bit palette
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                // Check bounds for the current pixel
                uint32_t fbY, fbX;
                if ((fbY = y + fbYOffset - 8) >= 480) break;
                if ((fbX = x + fbXOffset - 96) >= 854) continue;

                // Look up a color with an 8-bit palette index
                buffer[fbY * 854 + fbX] = palette[Memory::read<uint8_t>(fbAddress + y * fbStride + x)];
            }
        }
        break;

    case 2: // 16-bit ARGB
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                // Check bounds for the current pixel
                uint32_t fbY, fbX;
                if ((fbY = y + fbYOffset - 8) >= 480) break;
                if ((fbX = x + fbXOffset - 96) >= 854) continue;

                // Convert a 16-bit ARGB color to 32-bit ABGR
                uint16_t color = Memory::read<uint16_t>(fbAddress + (y * fbStride + x) * 2);
                uint8_t r = ((color >> 10) & 0x1F) * 0xFF / 0x1F;
                uint8_t g = ((color >> 5) & 0x1F) * 0xFF / 0x1F;
                uint8_t b = ((color >> 0) & 0x1F) * 0xFF / 0x1F;
                buffer[fbY * 854 + fbX] = 0xFF000000 | (b << 16) | (g << 8) | r;
            }
        }
        break;

    default:
        // Handle unimplemented formats by drawing nothing
        printf("Unimplemented framebuffer format: %d\n", fmt);
        break;
    }

    // Queue the buffer to be displayed once there's room
    mutex.lock();
    while (buffers.size() > 2) {
        mutex.unlock();
        std::this_thread::yield();
        mutex.lock();
    }
    buffers.push(buffer);
    mutex.unlock();

    // Trigger a V-blank interrupt and schedule the next one
    Interrupts::requestIrq(22);
    Core::schedule(drawFrame, 108000000 / 60);
}

uint32_t Display::readFbXOfs() {
    // Read from the framebuffer X-offset register
    return fbXOffset;
}

uint32_t Display::readFbWidth() {
    // Read from the framebuffer width register
    return fbWidth;
}

uint32_t Display::readFbYOfs() {
    // Read from the framebuffer Y-offset register
    return fbYOffset;
}

uint32_t Display::readFbHeight() {
    // Read from the framebuffer height register
    return fbHeight;
}

uint32_t Display::readFbAddr() {
    // Read from the framebuffer address register
    return fbAddress;
}

void Display::writeFbXOfs(uint32_t mask, uint32_t value) {
    // Write to the framebuffer X-offset register
    fbXOffset = (fbXOffset & ~mask) | (value & mask);
}

void Display::writeFbWidth(uint32_t mask, uint32_t value) {
    // Write to the framebuffer width register
    fbWidth = (fbWidth & ~mask) | (value & mask);
}

void Display::writeFbYOfs(uint32_t mask, uint32_t value) {
    // Write to the framebuffer Y-offset register
    fbYOffset = (fbYOffset & ~mask) | (value & mask);
}

void Display::writeFbHeight(uint32_t mask, uint32_t value) {
    // Write to the framebuffer height register
    fbHeight = (fbHeight & ~mask) | (value & mask);
}

void Display::writeFbStride(uint32_t mask, uint32_t value) {
    // Write to the framebuffer stride register
    fbStride = (fbStride & ~mask) | (value & mask);
}

void Display::writeFbAddr(uint32_t mask, uint32_t value) {
    // Write to the framebuffer address register
    fbAddress = (fbAddress & ~mask) | (value & mask);
}

void Display::writePixelFmt(uint32_t mask, uint32_t value) {
    // Write to the pixel format register
    pixelFormat = (pixelFormat & ~mask) | (value & mask);
}

void Display::writePalAddr(uint32_t mask, uint32_t value) {
    // Write to the palette address register
    palAddress = (palAddress & ~mask) | (value & mask);
}

void Display::writePalData(uint32_t mask, uint32_t value) {
    // Write a palette entry as 32-bit ABGR and move to the next one
    uint8_t r = (value & mask) >> 16;
    uint8_t g = (value & mask) >> 8;
    uint8_t b = (value & mask) >> 0;
    palette[palAddress++] = 0xFF000000 | (b << 16) | (g << 8) | r;
}
