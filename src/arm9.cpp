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

#include "arm9.h"
#include "memory.h"

namespace Arm9 {
    uint32_t *registers[32];
    uint32_t registersUsr[16];
    uint32_t registersFiq[7];
    uint32_t registersSvc[2];
    uint32_t registersAbt[2];
    uint32_t registersIrq[2];
    uint32_t registersUnd[2];

    uint32_t pipeline[2];
    uint32_t cpsr, *spsr;
    uint32_t spsrFiq;
    uint32_t spsrSvc;
    uint32_t spsrAbt;
    uint32_t spsrIrq;
    uint32_t spsrUnd;
}

void Arm9::reset() {
    // Reset the register arrays
    memset(registersUsr, 0, sizeof(registersUsr));
    memset(registersFiq, 0, sizeof(registersFiq));
    memset(registersSvc, 0, sizeof(registersSvc));
    memset(registersAbt, 0, sizeof(registersAbt));
    memset(registersIrq, 0, sizeof(registersIrq));
    memset(registersUnd, 0, sizeof(registersUnd));

    // Reset the flag registers
    cpsr = 0;
    spsr = nullptr;
    spsrFiq = 0;
    spsrSvc = 0;
    spsrAbt = 0;
    spsrIrq = 0;
    spsrUnd = 0;

    // Prepare for code execution
    for (int i = 0; i < 32; i++)
        registers[i] = &registersUsr[i & 0xF];
    setCpsr(0xD3); // Supervisor, interrupts off
    flushPipeline();
}

int Arm9::runOpcode() {
    // Push the next opcode through the pipeline
    uint32_t opcode = pipeline[0];
    pipeline[0] = pipeline[1];

    // Execute an instruction
    if (cpsr & 0x20) { // THUMB mode
        // Fill the pipeline, incrementing the program counter
        pipeline[1] = Memory::read<uint16_t>(*registers[15] += 2);

        // Execute a THUMB instruction
        return (*thumbInstrs[(opcode >> 6) & 0x3FF])(opcode);
    }
    else { // ARM mode
        // Fill the pipeline, incrementing the program counter
        pipeline[1] = Memory::read<uint32_t>(*registers[15] += 4);

        // Execute an ARM instruction based on its condition
        switch (condition[((opcode >> 24) & 0xF0) | (cpsr >> 28)]) {
            case 0: return 1; // False
            case 2: return ((opcode & 0xE000000) == 0xA000000) ? blx(opcode) : 1; // BLX
            default: return (*armInstrs[((opcode >> 16) & 0xFF0) | ((opcode >> 4) & 0xF)])(opcode);
        }
    }
}

int Arm9::exception(uint8_t vector) {
    // Switch the CPU mode, save the return address, and jump to the exception vector
    static const uint8_t modes[] = { 0x13, 0x1B, 0x13, 0x17, 0x17, 0x13, 0x12, 0x11 };
    setCpsr((cpsr & ~0x3F) | 0x80 | modes[vector >> 2], true); // ARM, interrupts off, new mode
    *registers[14] = *registers[15] + ((*spsr & 0x20) >> 4);
    *registers[15] = /*Cp15::exceptionAddr + */vector;
    flushPipeline();
    return 3;
}

void Arm9::flushPipeline() {
    // Adjust the program counter and refill the pipeline after a jump
    if (cpsr & 0x20) { // THUMB mode
        *registers[15] = (*registers[15] & ~0x1) + 2;
        pipeline[0] = Memory::read<uint16_t>(*registers[15] - 2);
        pipeline[1] = Memory::read<uint16_t>(*registers[15]);
    }
    else { // ARM mode
        *registers[15] = (*registers[15] & ~0x3) + 4;
        pipeline[0] = Memory::read<uint32_t>(*registers[15] - 4);
        pipeline[1] = Memory::read<uint32_t>(*registers[15]);
    }
}

void Arm9::swapRegisters(uint32_t value) {
    // Swap banked registers based on a CPU mode value
    switch (uint8_t mode = value & 0x1F) {
    case 0x10: // User
    case 0x1F: // System
        registers[8] = &registersUsr[8];
        registers[9] = &registersUsr[9];
        registers[10] = &registersUsr[10];
        registers[11] = &registersUsr[11];
        registers[12] = &registersUsr[12];
        registers[13] = &registersUsr[13];
        registers[14] = &registersUsr[14];
        spsr = nullptr;
        break;

    case 0x11: // FIQ
        registers[8] = &registersFiq[0];
        registers[9] = &registersFiq[1];
        registers[10] = &registersFiq[2];
        registers[11] = &registersFiq[3];
        registers[12] = &registersFiq[4];
        registers[13] = &registersFiq[5];
        registers[14] = &registersFiq[6];
        spsr = &spsrFiq;
        break;

    case 0x12: // IRQ
        registers[8] = &registersUsr[8];
        registers[9] = &registersUsr[9];
        registers[10] = &registersUsr[10];
        registers[11] = &registersUsr[11];
        registers[12] = &registersUsr[12];
        registers[13] = &registersIrq[0];
        registers[14] = &registersIrq[1];
        spsr = &spsrIrq;
        break;

    case 0x13: // Supervisor
        registers[8] = &registersUsr[8];
        registers[9] = &registersUsr[9];
        registers[10] = &registersUsr[10];
        registers[11] = &registersUsr[11];
        registers[12] = &registersUsr[12];
        registers[13] = &registersSvc[0];
        registers[14] = &registersSvc[1];
        spsr = &spsrSvc;
        break;

    case 0x17: // Abort
        registers[8] = &registersUsr[8];
        registers[9] = &registersUsr[9];
        registers[10] = &registersUsr[10];
        registers[11] = &registersUsr[11];
        registers[12] = &registersUsr[12];
        registers[13] = &registersAbt[0];
        registers[14] = &registersAbt[1];
        spsr = &spsrAbt;
        break;

    case 0x1B: // Undefined
        registers[8] = &registersUsr[8];
        registers[9] = &registersUsr[9];
        registers[10] = &registersUsr[10];
        registers[11] = &registersUsr[11];
        registers[12] = &registersUsr[12];
        registers[13] = &registersUnd[0];
        registers[14] = &registersUnd[1];
        spsr = &spsrUnd;
        break;

    default:
        printf("Unknown ARM9 mode: 0x%X\n", mode);
        break;
    }
}

void Arm9::setCpsr(uint32_t value, bool save) {
    // Update registers if the CPU mode changed
    if ((value & 0x1F) != (cpsr & 0x1F))
        swapRegisters(value);

    // Set the CPSR, saving the old value if requested
    // TODO: check interrupts
    if (save && spsr) *spsr = cpsr;
    cpsr = value;
}

int Arm9::unkArm(uint32_t opcode) {
    // Handle an unknown ARM opcode
    printf("Unknown ARM opcode: 0x%X\n", opcode);
    return 1;
}

int Arm9::unkThumb(uint16_t opcode) {
    // Handle an unknown THUMB opcode
    printf("Unknown THUMB opcode: 0x%X\n", opcode);
    return 1;
}
