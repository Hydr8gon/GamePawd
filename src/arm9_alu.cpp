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

#include "arm9.h"

namespace Arm9 {
    int32_t clampQ(int64_t value);

    uint32_t lli(uint32_t opcode);
    uint32_t llr(uint32_t opcode);
    uint32_t lri(uint32_t opcode);
    uint32_t lrr(uint32_t opcode);
    uint32_t ari(uint32_t opcode);
    uint32_t arr(uint32_t opcode);
    uint32_t rri(uint32_t opcode);
    uint32_t rrr(uint32_t opcode);
    uint32_t imm(uint32_t opcode);

    uint32_t lliS(uint32_t opcode);
    uint32_t llrS(uint32_t opcode);
    uint32_t lriS(uint32_t opcode);
    uint32_t lrrS(uint32_t opcode);
    uint32_t ariS(uint32_t opcode);
    uint32_t arrS(uint32_t opcode);
    uint32_t rriS(uint32_t opcode);
    uint32_t rrrS(uint32_t opcode);
    uint32_t immS(uint32_t opcode);

    int _and(uint32_t opcode, uint32_t op2);
    int eor(uint32_t opcode, uint32_t op2);
    int sub(uint32_t opcode, uint32_t op2);
    int rsb(uint32_t opcode, uint32_t op2);
    int add(uint32_t opcode, uint32_t op2);
    int adc(uint32_t opcode, uint32_t op2);
    int sbc(uint32_t opcode, uint32_t op2);
    int rsc(uint32_t opcode, uint32_t op2);
    int tst(uint32_t opcode, uint32_t op2);
    int teq(uint32_t opcode, uint32_t op2);
    int cmp(uint32_t opcode, uint32_t op2);
    int cmn(uint32_t opcode, uint32_t op2);
    int orr(uint32_t opcode, uint32_t op2);
    int mov(uint32_t opcode, uint32_t op2);
    int bic(uint32_t opcode, uint32_t op2);
    int mvn(uint32_t opcode, uint32_t op2);

    int ands(uint32_t opcode, uint32_t op2);
    int eors(uint32_t opcode, uint32_t op2);
    int subs(uint32_t opcode, uint32_t op2);
    int rsbs(uint32_t opcode, uint32_t op2);
    int adds(uint32_t opcode, uint32_t op2);
    int adcs(uint32_t opcode, uint32_t op2);
    int sbcs(uint32_t opcode, uint32_t op2);
    int rscs(uint32_t opcode, uint32_t op2);
    int orrs(uint32_t opcode, uint32_t op2);
    int movs(uint32_t opcode, uint32_t op2);
    int bics(uint32_t opcode, uint32_t op2);
    int mvns(uint32_t opcode, uint32_t op2);
}

// Define functions for each ARM shift variation
#define ALU_FUNCS(func, S) \
    int Arm9::func##Lli(uint32_t opcode) { return func(opcode, lli##S(opcode)); } \
    int Arm9::func##Llr(uint32_t opcode) { return func(opcode, llr##S(opcode)) + 1; } \
    int Arm9::func##Lri(uint32_t opcode) { return func(opcode, lri##S(opcode)); } \
    int Arm9::func##Lrr(uint32_t opcode) { return func(opcode, lrr##S(opcode)) + 1; } \
    int Arm9::func##Ari(uint32_t opcode) { return func(opcode, ari##S(opcode)); } \
    int Arm9::func##Arr(uint32_t opcode) { return func(opcode, arr##S(opcode)) + 1; } \
    int Arm9::func##Rri(uint32_t opcode) { return func(opcode, rri##S(opcode)); } \
    int Arm9::func##Rrr(uint32_t opcode) { return func(opcode, rrr##S(opcode)) + 1; } \
    int Arm9::func##Imm(uint32_t opcode) { return func(opcode, imm##S(opcode)); }

// Create functions for instructions that have shift variations
ALU_FUNCS(_and,)
ALU_FUNCS(ands, S)
ALU_FUNCS(eor,)
ALU_FUNCS(eors, S)
ALU_FUNCS(sub,)
ALU_FUNCS(subs,)
ALU_FUNCS(rsb,)
ALU_FUNCS(rsbs,)
ALU_FUNCS(add,)
ALU_FUNCS(adds,)
ALU_FUNCS(adc,)
ALU_FUNCS(adcs,)
ALU_FUNCS(sbc,)
ALU_FUNCS(sbcs,)
ALU_FUNCS(rsc,)
ALU_FUNCS(rscs,)
ALU_FUNCS(tst, S)
ALU_FUNCS(teq, S)
ALU_FUNCS(cmp, S)
ALU_FUNCS(cmn, S)
ALU_FUNCS(orr,)
ALU_FUNCS(orrs, S)
ALU_FUNCS(mov,)
ALU_FUNCS(movs, S)
ALU_FUNCS(bic,)
ALU_FUNCS(bics, S)
ALU_FUNCS(mvn,)
ALU_FUNCS(mvns, S)

inline int32_t Arm9::clampQ(int64_t value) {
    // Clamp value and set Q flag for saturated operations
    if (value > 0x7FFFFFFF) {
        cpsr |= (1 << 27);
        return 0x7FFFFFFF;
    }
    else if (value < -0x80000000) {
        cpsr |= (1 << 27);
        return -0x80000000;
    }
    return (int32_t)value;
}

inline uint32_t Arm9::lli(uint32_t opcode) { // Rm,LSL #i
    // Logical shift left by immediate
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return value << shift;
}

inline uint32_t Arm9::llr(uint32_t opcode) { // Rm,LSL Rs
    // Logical shift left by register
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    return (shift < 32) ? (value << shift) : 0;
}

inline uint32_t Arm9::lri(uint32_t opcode) { // Rm,LSR #i
    // Logical shift right by immediate
    // A shift of 0 translates to a shift of 32
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return shift ? (value >> shift) : 0;
}

inline uint32_t Arm9::lrr(uint32_t opcode) { // Rm,LSR Rs
    // Logical shift right by register
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    return (shift < 32) ? (value >> shift) : 0;
}

inline uint32_t Arm9::ari(uint32_t opcode) { // Rm,ASR #i
    // Arithmetic shift right by immediate
    // A shift of 0 translates to a shift of 32
    int32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return value >> (shift ? shift : 31);
}

inline uint32_t Arm9::arr(uint32_t opcode) { // Rm,ASR Rs
    // Arithmetic shift right by register
    // When used as Rm, the program counter is read with +4
    int32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    return value >> ((shift < 32) ? shift : 31);
}

inline uint32_t Arm9::rri(uint32_t opcode) { // Rm,ROR #i
    // Rotate right by immediate
    // A shift of 0 translates to a rotate with carry of 1
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return shift ? ((value << (32 - shift)) | (value >> shift)) : (((cpsr & (1 << 29)) << 2) | (value >> 1));
}

inline uint32_t Arm9::rrr(uint32_t opcode) { // Rm,ROR Rs
    // Rotate right by register
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    return (value << (32 - (shift & 0x1F))) | (value >> ((shift & 0x1F)));
}

inline uint32_t Arm9::imm(uint32_t opcode) { // #i
    // Rotate 8-bit immediate right by a multiple of 2
    uint32_t value = opcode & 0xFF;
    uint8_t shift = (opcode >> 7) & 0x1E;
    return (value << (32 - shift)) | (value >> shift);
}

inline uint32_t Arm9::lliS(uint32_t opcode) { // Rm,LSL #i (S)
    // Logical shift left by immediate and set carry flag
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << (32 - shift))) << 29);
    return value << shift;
}

inline uint32_t Arm9::llrS(uint32_t opcode) { // Rm,LSL Rs (S)
    // Logical shift left by register and set carry flag
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((shift <= 32 && (value & (1 << (32 - shift)))) << 29);
    return (shift < 32) ? (value << shift) : 0;
}

inline uint32_t Arm9::lriS(uint32_t opcode) { // Rm,LSR #i (S)
    // Logical shift right by immediate and set carry flag
    // A shift of 0 translates to a shift of 32
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << (shift ? (shift - 1) : 31))) << 29);
    return shift ? (value >> shift) : 0;
}

inline uint32_t Arm9::lrrS(uint32_t opcode) { // Rm,LSR Rs (S)
    // Logical shift right by register and set carry flag
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((shift <= 32 && (value & (1 << (shift - 1)))) << 29);
    return (shift < 32) ? (value >> shift) : 0;
}

inline uint32_t Arm9::ariS(uint32_t opcode) { // Rm,ASR #i (S)
    // Arithmetic shift right by immediate and set carry flag
    // A shift of 0 translates to a shift of 32
    int32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << (shift ? (shift - 1) : 31))) << 29);
    return value >> (shift ? shift : 31);
}

inline uint32_t Arm9::arrS(uint32_t opcode) { // Rm,ASR Rs (S)
    // Arithmetic shift right by register and set carry flag
    // When used as Rm, the program counter is read with +4
    int32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << ((shift <= 32) ? (shift - 1) : 31))) << 29);
    return value >> ((shift < 32) ? shift : 31);
}

inline uint32_t Arm9::rriS(uint32_t opcode) { // Rm,ROR #i (S)
    // Rotate right by immediate and set carry flag
    // A shift of 0 translates to a rotate with carry of 1
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    uint32_t res = shift ? ((value << (32 - shift)) | (value >> shift)) : (((cpsr & (1 << 29)) << 2) | (value >> 1));
    cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << (shift ? (shift - 1) : 0))) << 29);
    return res;
}

inline uint32_t Arm9::rrrS(uint32_t opcode) { // Rm,ROR Rs (S)
    // Rotate right by register and set carry flag
    // When used as Rm, the program counter is read with +4
    uint32_t value = *registers[opcode & 0xF] + (((opcode & 0xF) == 0xF) << 2);
    uint8_t shift = *registers[(opcode >> 8) & 0xF];
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << ((shift - 1) & 0x1F))) << 29);
    return (value << (32 - (shift & 0x1F))) | (value >> ((shift & 0x1F)));
}

inline uint32_t Arm9::immS(uint32_t opcode) { // #i (S)
    // Rotate 8-bit immediate right by a multiple of 2 and set carry flag
    uint32_t value = opcode & 0xFF;
    uint8_t shift = (opcode >> 7) & 0x1E;
    if (shift) cpsr = (cpsr & ~(1 << 29)) | ((bool)(value & (1 << (shift - 1))) << 29);
    return (value << (32 - shift)) | (value >> shift);
}

inline int Arm9::_and(uint32_t opcode, uint32_t op2) { // AND Rd,Rn,op2
    // Bitwise and
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 & op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::eor(uint32_t opcode, uint32_t op2) { // EOR Rd,Rn,op2
    // Bitwise exclusive or
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 ^ op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::sub(uint32_t opcode, uint32_t op2) { // SUB Rd,Rn,op2
    // Subtraction
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 - op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::rsb(uint32_t opcode, uint32_t op2) { // RSB Rd,Rn,op2
    // Reverse subtraction
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op2 - op1;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::add(uint32_t opcode, uint32_t op2) { // ADD Rd,Rn,op2
    // Addition
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 + op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::adc(uint32_t opcode, uint32_t op2) { // ADC Rd,Rn,op2
    // Addition with carry
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 + op2 + ((cpsr & (1 << 29)) >> 29);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::sbc(uint32_t opcode, uint32_t op2) { // SBC Rd,Rn,op2
    // Subtraction with carry
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 - op2 - 1 + ((cpsr & (1 << 29)) >> 29);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::rsc(uint32_t opcode, uint32_t op2) { // RSC Rd,Rn,op2
    // Reverse subtraction with carry
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op2 - op1 - 1 + ((cpsr & (1 << 29)) >> 29);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::tst(uint32_t opcode, uint32_t op2) { // TST Rn,op2
    // Test bits and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    uint32_t res = op1 & op2;
    cpsr = (cpsr & ~0xC0000000) | (res & (1 << 31)) | ((res == 0) << 30);
    return 1;
}

inline int Arm9::teq(uint32_t opcode, uint32_t op2) { // TEQ Rn,op2
    // Test bits and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    uint32_t res = op1 ^ op2;
    cpsr = (cpsr & ~0xC0000000) | (res & (1 << 31)) | ((res == 0) << 30);
    return 1;
}

inline int Arm9::cmp(uint32_t opcode, uint32_t op2) { // CMP Rn,op2
    // Compare and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    uint32_t res = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 >= res) << 29) | (((op2 ^ op1) & ~(res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

inline int Arm9::cmn(uint32_t opcode, uint32_t op2) { // CMN Rn,op2
    // Compare negative and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    uint32_t res = op1 + op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 > res) << 29) | ((~(op2 ^ op1) & (res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

inline int Arm9::orr(uint32_t opcode, uint32_t op2) { // ORR Rd,Rn,op2
    // Bitwise or
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 | op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::mov(uint32_t opcode, uint32_t op2) { // MOV Rd,op2
    // Move
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    *op0 = op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::bic(uint32_t opcode, uint32_t op2) { // BIC Rd,Rn,op2
    // Bit clear
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 & ~op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::mvn(uint32_t opcode, uint32_t op2) { // MVN Rd,op2
    // Move negative
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    *op0 = ~op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

inline int Arm9::ands(uint32_t opcode, uint32_t op2) { // ANDS Rd,Rn,op2
    // Bitwise and and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 & op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::eors(uint32_t opcode, uint32_t op2) { // EORS Rd,Rn,op2
    // Bitwise exclusive or and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 ^ op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::subs(uint32_t opcode, uint32_t op2) { // SUBS Rd,Rn,op2
    // Subtraction and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 >= *op0) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::rsbs(uint32_t opcode, uint32_t op2) { // RSBS Rd,Rn,op2
    // Reverse subtraction and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op2 - op1;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op2 >= *op0) << 29) | (((op1 ^ op2) & ~(*op0 ^ op1) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::adds(uint32_t opcode, uint32_t op2) { // ADDS Rd,Rn,op2
    // Addition and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 + op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 > *op0) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::adcs(uint32_t opcode, uint32_t op2) { // ADCS Rd,Rn,op2
    // Addition with carry and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 + op2 + ((cpsr & (1 << 29)) >> 29);
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((op1 > *op0 ||
        (op2 == -1 && (cpsr & (1 << 29)))) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::sbcs(uint32_t opcode, uint32_t op2) { // SBCS Rd,Rn,op2
    // Subtraction with carry and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 - op2 - 1 + ((cpsr & (1 << 29)) >> 29);
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((op1 >= *op0 &&
        (op2 != -1 || (cpsr & (1 << 29)))) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::rscs(uint32_t opcode, uint32_t op2) { // RSCS Rd,Rn,op2
    // Reverse subtraction with carry and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op2 - op1 - 1 + ((cpsr & (1 << 29)) >> 29);
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((op2 >= *op0 &&
        (op1 != -1 || (cpsr & (1 << 29)))) << 29) | (((op1 ^ op2) & ~(*op0 ^ op1) & (1 << 31)) >> 3);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::orrs(uint32_t opcode, uint32_t op2) { // ORRS Rd,Rn,op2
    // Bitwise or and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 | op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::movs(uint32_t opcode, uint32_t op2) { // MOVS Rd,op2
    // Move and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    *op0 = op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::bics(uint32_t opcode, uint32_t op2) { // BICS Rd,Rn,op2
    // Bit clear and set flags
    // When used as Rn when shifting by register, the program counter is read with +4
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF] + (((opcode & 0x20F0010) == 0x00F0010) << 2);
    *op0 = op1 & ~op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

inline int Arm9::mvns(uint32_t opcode, uint32_t op2) { // MVNS Rd,op2
    // Move negative and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    *op0 = ~op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);

    // Handle pipelining and mode switching
    if (op0 != registers[15]) return 1;
    if (spsr) setCpsr(*spsr);
    flushPipeline();
    return 3;
}

int Arm9::mul(uint32_t opcode) { // MUL Rd,Rm,Rs
    // Multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 8) & 0xF];
    *op0 = op1 * op2;
    return 2;
}

int Arm9::mla(uint32_t opcode) { // MLA Rd,Rm,Rs,Rn
    // Multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 8) & 0xF];
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    *op0 = op1 * op2 + op3;
    return 2;
}

int Arm9::umull(uint32_t opcode) { // UMULL RdLo,RdHi,Rm,Rs
    // Unsigned long multiplication
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    uint64_t res = (uint64_t)op2 * (uint32_t)op3;
    *op1 = res >> 32;
    *op0 = res;
    return 3;
}

int Arm9::umlal(uint32_t opcode) { // UMLAL RdLo,RdHi,Rm,Rs
    // Unsigned long multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    uint64_t res = (uint64_t)op2 * (uint32_t)op3;
    res += ((uint64_t)*op1 << 32) | *op0;
    *op1 = res >> 32;
    *op0 = res;
    return 3;
}

int Arm9::smull(uint32_t opcode) { // SMULL RdLo,RdHi,Rm,Rs
    // Signed long multiplication
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = (int64_t)op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    return 3;
}

int Arm9::smlal(uint32_t opcode) { // SMLAL RdLo,RdHi,Rm,Rs
    // Signed long multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = (int64_t)op2 * op3;
    res += ((int64_t)*op1 << 32) | *op0;
    *op1 = res >> 32;
    *op0 = res;
    return 3;
}

int Arm9::muls(uint32_t opcode) { // MULS Rd,Rm,Rs
    // Multiplication and set flags
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 8) & 0xF];
    *op0 = op1 * op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 4;
}

int Arm9::mlas(uint32_t opcode) { // MLAS Rd,Rm,Rs,Rn
    // Multiplication with accumulate and set flags
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 8) & 0xF];
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    *op0 = op1 * op2 + op3;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 4;
}

int Arm9::umulls(uint32_t opcode) { // UMULLS RdLo,RdHi,Rm,Rs
    // Unsigned long multiplication and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    uint64_t res = (uint64_t)op2 * (uint32_t)op3;
    *op1 = res >> 32;
    *op0 = res;
    cpsr = (cpsr & ~0xC0000000) | (*op1 & (1 << 31)) | ((*op1 == 0) << 30);
    return 5;
}

int Arm9::umlals(uint32_t opcode) { // UMLALS RdLo,RdHi,Rm,Rs
    // Unsigned long multiplication with accumulate and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    uint64_t res = (uint64_t)op2 * (uint32_t)op3;
    res += ((uint64_t)*op1 << 32) | *op0;
    *op1 = res >> 32;
    *op0 = res;
    cpsr = (cpsr & ~0xC0000000) | (*op1 & (1 << 31)) | ((*op1 == 0) << 30);
    return 5;
}

int Arm9::smulls(uint32_t opcode) { // SMULLS RdLo,RdHi,Rm,Rs
    // Signed long multiplication and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = (int64_t)op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    cpsr = (cpsr & ~0xC0000000) | (*op1 & (1 << 31)) | ((*op1 == 0) << 30);
    return 5;
}

int Arm9::smlals(uint32_t opcode) { // SMLALS RdLo,RdHi,Rm,Rs
    // Signed long multiplication with accumulate and set flags
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int32_t op2 = *registers[opcode & 0xF];
    int32_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = (int64_t)op2 * op3;
    res += ((int64_t)*op1 << 32) | *op0;
    *op1 = res >> 32;
    *op0 = res;
    cpsr = (cpsr & ~0xC0000000) | (*op1 & (1 << 31)) | ((*op1 == 0) << 30);
    return 5;
}

int Arm9::smulbb(uint32_t opcode) { // SMULBB Rd,Rm,Rs
    // Signed half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    *op0 = op1 * op2;
    return 1;
}

int Arm9::smulbt(uint32_t opcode) { // SMULBT Rd,Rm,Rs
    // Signed half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    *op0 = op1 * op2;
    return 1;
}

int Arm9::smultb(uint32_t opcode) { // SMULTB Rd,Rm,Rs
    // Signed half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF] >> 16;
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    *op0 = op1 * op2;
    return 1;
}

int Arm9::smultt(uint32_t opcode) { // SMULTT Rd,Rm,Rs
    // Signed half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF] >> 16;
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    *op0 = op1 * op2;
    return 1;
}

int Arm9::smulwb(uint32_t opcode) { // SMULWB Rd,Rm,Rs
    // Signed word by half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    *op0 = ((int64_t)op1 * op2) >> 16;
    return 1;
}

int Arm9::smulwt(uint32_t opcode) { // SMULWT Rd,Rm,Rs
    // Signed word by half-word multiplication
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    *op0 = ((int64_t)op1 * op2) >> 16;
    return 1;
}

int Arm9::smlabb(uint32_t opcode) { // SMLABB Rd,Rm,Rs,Rn
    // Signed half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = op1 * op2;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlabt(uint32_t opcode) { // SMLABT Rd,Rm,Rs,Rn
    // Signed half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = op1 * op2;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlatb(uint32_t opcode) { // SMLATB Rd,Rm,Rs,Rn
    // Signed half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF] >> 16;
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = op1 * op2;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlatt(uint32_t opcode) { // SMLATT Rd,Rm,Rs,Rn
    // Signed half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int16_t op1 = *registers[opcode & 0xF] >> 16;
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = op1 * op2;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlawb(uint32_t opcode) { // SMLAWB Rd,Rm,Rs,Rn
    // Signed word by half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF];
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = ((int64_t)op1 * op2) >> 16;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlawt(uint32_t opcode) { // SMLAWT Rd,Rm,Rs,Rn
    // Signed word by half-word multiplication with accumulate and set Q flag
    uint32_t *op0 = registers[(opcode >> 16) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int16_t op2 = *registers[(opcode >> 8) & 0xF] >> 16;
    uint32_t op3 = *registers[(opcode >> 12) & 0xF];
    uint32_t res = ((int64_t)op1 * op2) >> 16;
    *op0 = res + op3;
    cpsr |= ((*op0 ^ res) & (1 << 31)) >> 4;
    return 1;
}

int Arm9::smlalbb(uint32_t opcode) { // SMLALBB RdLo,RdHi,Rm,Rs
    // Signed long half-word multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int16_t op2 = *registers[opcode & 0xF];
    int16_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = ((int64_t)*op1 << 32) | *op0;
    res += op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    return 2;
}

int Arm9::smlalbt(uint32_t opcode) { // SMLALBT RdLo,RdHi,Rm,Rs
    // Signed long half-word multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int16_t op2 = *registers[opcode & 0xF];
    int16_t op3 = *registers[(opcode >> 8) & 0xF] >> 16;
    int64_t res = ((int64_t)*op1 << 32) | *op0;
    res += op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    return 2;
}

int Arm9::smlaltb(uint32_t opcode) { // SMLALTB RdLo,RdHi,Rm,Rs
    // Signed long half-word multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int16_t op2 = *registers[opcode & 0xF] >> 16;
    int16_t op3 = *registers[(opcode >> 8) & 0xF];
    int64_t res = ((int64_t)*op1 << 32) | *op0;
    res += op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    return 2;
}

int Arm9::smlaltt(uint32_t opcode) { // SMLALTT RdLo,RdHi,Rm,Rs
    // Signed long half-word multiplication with accumulate
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    int16_t op2 = *registers[opcode & 0xF] >> 16;
    int16_t op3 = *registers[(opcode >> 8) & 0xF] >> 16;
    int64_t res = ((int64_t)*op1 << 32) | *op0;
    res += op2 * op3;
    *op1 = res >> 32;
    *op0 = res;
    return 2;
}

int Arm9::qadd(uint32_t opcode) { // QADD Rd,Rm,Rn
    // Signed saturated addition
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = clampQ((int64_t)op1 + op2);
    return 1;
}

int Arm9::qsub(uint32_t opcode) { // QSUB Rd,Rm,Rn
    // Signed saturated subtraction
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = clampQ((int64_t)op1 - op2);
    return 1;
}

int Arm9::qdadd(uint32_t opcode) { // QDADD Rd,Rm,Rn
    // Signed saturated double and addition
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = clampQ((int64_t)clampQ((int64_t)op2 * 2) + op1);
    return 1;
}

int Arm9::qdsub(uint32_t opcode) { // QDSUB Rd,Rm,Rn
    // Signed saturated double and subtraction
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    int32_t op1 = *registers[opcode & 0xF];
    int32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = clampQ((int64_t)clampQ((int64_t)op2 * 2) - op1);
    return 1;
}

int Arm9::clz(uint32_t opcode) { // CLZ Rd,Rm
    // Count leading zeros
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    for (*op0 = 32; op1 != 0; op1 >>= 1, (*op0)--);
    return 1;
}

int Arm9::addRegT(uint16_t opcode) { // ADD Rd,Rs,Rn
    // Addition and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = op1 + op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 > *op0) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::subRegT(uint16_t opcode) { // SUB Rd,Rs,Rn
    // Subtraction and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 >= *op0) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::addHT(uint16_t opcode) { // ADD Rd,Rs
    // Addition (THUMB)
    uint32_t *op0 = registers[((opcode >> 4) & 0x8) | (opcode & 0x7)];
    uint32_t op2 = *registers[(opcode >> 3) & 0xF];
    *op0 += op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

int Arm9::cmpHT(uint16_t opcode) { // CMP Rd,Rs
    // Compare and set flags (THUMB)
    uint32_t op1 = *registers[((opcode >> 4) & 0x8) | (opcode & 0x7)];
    uint32_t op2 = *registers[(opcode >> 3) & 0xF];
    uint32_t res = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 >= res) << 29) | (((op2 ^ op1) & ~(res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::movHT(uint16_t opcode) { // MOV Rd,Rs
    // Move (THUMB)
    uint32_t *op0 = registers[((opcode >> 4) & 0x8) | (opcode & 0x7)];
    uint32_t op2 = *registers[(opcode >> 3) & 0xF];
    *op0 = op2;

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 3;
}

int Arm9::addPcT(uint16_t opcode) { // ADD Rd,PC,#i
    // Program counter addition (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[15] & ~3;
    uint32_t op2 = (opcode & 0xFF) << 2;
    *op0 = op1 + op2;
    return 1;
}

int Arm9::addSpT(uint16_t opcode) { // ADD Rd,SP,#i
    // Stack pointer addition (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[13];
    uint32_t op2 = (opcode & 0xFF) << 2;
    *op0 = op1 + op2;
    return 1;
}

int Arm9::addSpImmT(uint16_t opcode) { // ADD SP,#i
    // Stack pointer addition (THUMB)
    uint32_t *op0 = registers[13];
    uint32_t op2 = ((opcode & 0x80) ? (0 - (opcode & 0x7F)) : (opcode & 0x7F)) << 2;
    *op0 += op2;
    return 1;
}

int Arm9::lslImmT(uint16_t opcode) { // LSL Rd,Rs,#i
    // Logical shift left and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint8_t op2 = (opcode >> 6) & 0x1F;
    *op0 = op1 << op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    if (op2) cpsr = (cpsr & ~(1 << 29)) | ((bool)(op1 & (1 << (32 - op2))) << 29);
    return 1;
}

int Arm9::lsrImmT(uint16_t opcode) { // LSR Rd,Rs,#i
    // Logical shift right and set flags (THUMB)
    // A shift of 0 translates to a shift of 32
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint8_t op2 = (opcode >> 6) & 0x1F;
    *op0 = op2 ? (op1 >> op2) : 0;
    cpsr = (cpsr & ~0xE0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((bool)(op1 & (1 << (op2 ? (op2 - 1) : 31))) << 29);
    return 1;
}

int Arm9::asrImmT(uint16_t opcode) { // ASR Rd,Rs,#i
    // Arithmetic shift right and set flags (THUMB)
    // A shift of 0 translates to a shift of 32
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint8_t op2 = (opcode >> 6) & 0x1F;
    *op0 = op2 ? ((int32_t)op1 >> op2) : ((op1 & (1 << 31)) ? -1 : 0);
    cpsr = (cpsr & ~0xE0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((bool)(op1 & (1 << (op2 ? (op2 - 1) : 31))) << 29);
    return 1;
}

int Arm9::addImm3T(uint16_t opcode) { // ADD Rd,Rs,#i
    // Addition and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 6) & 0x7;
    *op0 = op1 + op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 > *op0) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::subImm3T(uint16_t opcode) { // SUB Rd,Rs,#i
    // Subtraction and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 6) & 0x7;
    *op0 = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 >= *op0) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::addImm8T(uint16_t opcode) { // ADD Rd,#i
    // Addition and set flags (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[(opcode >> 8) & 0x7];
    uint32_t op2 = opcode & 0xFF;
    *op0 += op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 > *op0) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::subImm8T(uint16_t opcode) { // SUB Rd,#i
    // Subtraction and set flags (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[(opcode >> 8) & 0x7];
    uint32_t op2 = opcode & 0xFF;
    *op0 -= op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) |
        ((op1 >= *op0) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::cmpImm8T(uint16_t opcode) { // CMP Rd,#i
    // Compare and set flags (THUMB)
    uint32_t op1 = *registers[(opcode >> 8) & 0x7];
    uint32_t op2 = opcode & 0xFF;
    uint32_t res = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 >= res) << 29) | (((op2 ^ op1) & ~(res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::movImm8T(uint16_t opcode) { // MOV Rd,#i
    // Move and set flags (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op2 = opcode & 0xFF;
    *op0 = op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::lslDpT(uint16_t opcode) { // LSL Rd,Rs
    // Logical shift left and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint8_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = (op2 < 32) ? (*op0 << op2) : 0;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    if (op2) cpsr = (cpsr & ~(1 << 29)) | ((op2 <= 32 && (op1 & (1 << (32 - op2)))) << 29);
    return 1;
}

int Arm9::lsrDpT(uint16_t opcode) { // LSR Rd,Rs
    // Logical shift right and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint8_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = (op2 < 32) ? (*op0 >> op2) : 0;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    if (op2) cpsr = (cpsr & ~(1 << 29)) | ((op2 <= 32 && (op1 & (1 << (op2 - 1)))) << 29);
    return 1;
}

int Arm9::asrDpT(uint16_t opcode) { // ASR Rd,Rs
    // Arithmetic shift right and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint8_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = (op2 < 32) ? ((int32_t)(*op0) >> op2) : ((*op0 & (1 << 31)) ? -1 : 0);
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    if (op2) cpsr = (cpsr & ~(1 << 29)) | ((bool)(op1 & (1 << ((op2 <= 32) ? (op2 - 1) : 31))) << 29);
    return 1;
}

int Arm9::rorDpT(uint16_t opcode) { // ROR Rd,Rs
    // Rotate right and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint8_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = (*op0 << (32 - (op2 & 0x1F))) | (*op0 >> (op2 & 0x1F));
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    if (op2) cpsr = (cpsr & ~(1 << 29)) | ((bool)(op1 & (1 << ((op2 - 1) & 0x1F))) << 29);
    return 1;
}

int Arm9::andDpT(uint16_t opcode) { // AND Rd,Rs
    // Bitwise and and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 &= op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::eorDpT(uint16_t opcode) { // EOR Rd,Rs
    // Bitwise exclusive or and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 ^= op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::adcDpT(uint16_t opcode) { // ADC Rd,Rs
    // Addition with carry and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 += op2 + ((cpsr & (1 << 29)) >> 29);
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((op1 > *op0 ||
        (op2 == -1 && (cpsr & (1 << 29)))) << 29) | ((~(op2 ^ op1) & (*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::sbcDpT(uint16_t opcode) { // SBC Rd,Rs
    // Subtraction with carry and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = op1 - op2 - 1 + ((cpsr & (1 << 29)) >> 29);
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((op1 >= *op0 &&
        (op2 != -1 || (cpsr & (1 << 29)))) << 29) | (((op2 ^ op1) & ~(*op0 ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::tstDpT(uint16_t opcode) { // TST Rd,Rs
    // Test bits and set flags (THUMB)
    uint32_t op1 = *registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    uint32_t res = op1 & op2;
    cpsr = (cpsr & ~0xC0000000) | (res & (1 << 31)) | ((res == 0) << 30);
    return 1;
}

int Arm9::cmpDpT(uint16_t opcode) { // CMP Rd,Rs
    // Compare and set flags (THUMB)
    uint32_t op1 = *registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    uint32_t res = op1 - op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 >= res) << 29) | (((op2 ^ op1) & ~(res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::cmnDpT(uint16_t opcode) { // CMN Rd,Rs
    // Compare negative and set flags (THUMB)
    uint32_t op1 = *registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    uint32_t res = op1 + op2;
    cpsr = (cpsr & ~0xF0000000) | (res & (1 << 31)) | ((res == 0) << 30) |
        ((op1 > res) << 29) | ((~(op2 ^ op1) & (res ^ op2) & (1 << 31)) >> 3);
    return 1;
}

int Arm9::orrDpT(uint16_t opcode) { // ORR Rd,Rs
    // Bitwise or and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 |= op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::bicDpT(uint16_t opcode) { // BIC Rd,Rs
    // Bit clear and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 &= ~op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::mvnDpT(uint16_t opcode) { // MVN Rd,Rs
    // Move negative and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = ~op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 1;
}

int Arm9::negDpT(uint16_t opcode) { // NEG Rd,Rs
    // Negation and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op2 = *registers[(opcode >> 3) & 0x7];
    *op0 = -op2;
    cpsr = (cpsr & ~0xF0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30) | ((*op0 <= 0) << 29);
    return 1;
}

int Arm9::mulDpT(uint16_t opcode) { // MUL Rd,Rs
    // Multiplication and set flags (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    int32_t op2 = *registers[opcode & 0x7];
    *op0 = op1 * op2;
    cpsr = (cpsr & ~0xC0000000) | (*op0 & (1 << 31)) | ((*op0 == 0) << 30);
    return 4;
}
