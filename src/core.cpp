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

#include <thread>

#include "core.h"
#include "arm9.h"
#include "display.h"
#include "dma.h"
#include "interrupts.h"
#include "memory.h"
#include "spi.h"
#include "timers.h"

namespace Core {
    bool running;
    std::thread *thread;

    void runLoop();
}

void Core::reset() {
    // Reset the emulator
    Display::reset();
    Dma::reset();
    Interrupts::reset();
    Memory::reset();
    Spi::reset();
    Timers::reset();
    Arm9::reset();
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
    // Run the emulator
    while (running) {
        Arm9::runOpcode();
        Timers::tick();
    }
}
