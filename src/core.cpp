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
#include <thread>
#include <vector>

#include "core.h"
#include "arm9.h"
#include "display.h"
#include "dma.h"
#include "i2c.h"
#include "interrupts.h"
#include "memory.h"
#include "spi.h"
#include "timers.h"
#include "wifi.h"

struct SchedEvent {
    void (*task)();
    uint32_t cycles;

    SchedEvent(void (*task)(), uint32_t cycles): task(task), cycles(cycles) {}
    bool operator<(const SchedEvent &event) const { return cycles < event.cycles; }
};

namespace Core {
    std::thread *thread;
    bool running;

    std::vector<SchedEvent> events;
    uint32_t globalCycles;
    uint32_t arm9Cycles;

    void runLoop();
    void resetCycles();
}

void Core::reset() {
    // Reset the scheduler
    events.clear();
    globalCycles = 0;
    arm9Cycles = 0;
    schedule(resetCycles, 0x7FFFFFFF);

    // Reset the rest of the emulator
    Display::reset();
    Dma::reset();
    I2c::reset();
    Interrupts::reset();
    Memory::reset();
    Spi::reset();
    Timers::reset();
    Wifi::reset();
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
        // Run the ARM9 until the next scheduled task
        globalCycles = arm9Cycles;
        while (events[0].cycles > globalCycles)
            globalCycles = (arm9Cycles += Arm9::runOpcode());

        // Run all tasks that are scheduled now
        globalCycles = events[0].cycles;
        while (events[0].cycles == globalCycles) {
            events[0].task();
            events.erase(events.begin());
        }
    }
}

void Core::resetCycles() {
    // Reset cycle counts periodically to prevent overflow
    for (uint32_t i = 0; i < events.size(); i++)
        events[i].cycles -= globalCycles;
    Timers::timerCycles -= globalCycles;
    Timers::countCycles -= globalCycles;
    arm9Cycles -= globalCycles;
    globalCycles -= globalCycles;
    schedule(resetCycles, 0x7FFFFFFF);
}

uint32_t Core::schedule(void (*task)(), uint32_t cycles) {
    // Add a task to the scheduler, sorted by least to most cycles until execution
    SchedEvent event(task, cycles += globalCycles);
    auto it = std::upper_bound(events.cbegin(), events.cend(), event);
    events.insert(it, event);
    return cycles;
}
