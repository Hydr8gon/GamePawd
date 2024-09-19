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
#include <thread>

#include "core.h"
#include "arm9.h"
#include "display.h"
#include "memory.h"
#include "spi.h"

namespace Core {
    bool running;
    std::thread *thread;

    void runLoop();
}

void Core::loadFirm() {
    // Reset memory and load the firmware
    Memory::reset();
    if (FILE *file = fopen("drc_fw.bin", "rb")) {
        // Load the firmware file into memory
        fseek(file, 0, SEEK_END);
        uint32_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        uint8_t *firm = new uint8_t[size];
        fread(firm, sizeof(uint8_t), size, file);
        fclose(file);

        // Determine start and end offsets of the ARM9 code
        uint32_t start = 0, end = 0;
        for (uint32_t i = 8; i < size; i += 4) {
            // Find the start of the partition table
            if (firm[i] == 'I' && firm[i + 1] == 'N' && firm[i + 2] == 'D' && firm[i + 3] == 'X') {
                start = i - 8;
                continue;
            }

            // Find the ARM9 code entry and parse offset and length
            if (start && firm[i] == 'L' && firm[i + 1] == 'V' && firm[i + 2] == 'C' && firm[i + 3] == '_') {
                start += (firm[i - 8] | (firm[i - 7] << 8) | (firm[i - 6] << 16) | (firm[i - 5] << 24));
                end = start + (firm[i - 4] | (firm[i - 3] << 8) | (firm[i - 2] << 16) | (firm[i - 1] << 24));
                printf("Found ARM9 code at 0x%X with size 0x%X\n", start, end - start);
                break;
            }
        }

        // Copy the ARM9 code into RAM and free the firmware file
        for (uint32_t i = start; i < end; i++)
            Memory::write(i - start, firm[i]);
        delete[] firm;
    }

    // Reset the rest of the emulator
    Arm9::reset();
    Display::reset();
    Spi::reset();
}

void Core::start() {
    // Start the emulation thread if it wasn't running
    if (running) return;
    running = true;
    thread = new std::thread(runLoop);
}

void Core::stop() {
    // Stop the emulation thread if it was running
    if (!running) return;
    running = false;
    thread->join();
    delete thread;
}

void Core::runLoop() {
    // Run the ARM9
    while (running)
        Arm9::runOpcode();
}
