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
#include "memory.h"

namespace Arm9 {
    uint32_t ip(uint32_t opcode);
    uint32_t ipH(uint32_t opcode);
    uint32_t rp(uint32_t opcode);
    uint32_t rpll(uint32_t opcode);
    uint32_t rplr(uint32_t opcode);
    uint32_t rpar(uint32_t opcode);
    uint32_t rprr(uint32_t opcode);

    int ldrsbOf(uint32_t opcode, uint32_t op2);
    int ldrshOf(uint32_t opcode, uint32_t op2);
    int ldrbOf(uint32_t opcode, uint32_t op2);
    int strbOf(uint32_t opcode, uint32_t op2);
    int ldrhOf(uint32_t opcode, uint32_t op2);
    int strhOf(uint32_t opcode, uint32_t op2);
    int ldrOf(uint32_t opcode, uint32_t op2);
    int strOf(uint32_t opcode, uint32_t op2);
    int ldrdOf(uint32_t opcode, uint32_t op2);
    int strdOf(uint32_t opcode, uint32_t op2);

    int ldrsbPr(uint32_t opcode, uint32_t op2);
    int ldrshPr(uint32_t opcode, uint32_t op2);
    int ldrbPr(uint32_t opcode, uint32_t op2);
    int strbPr(uint32_t opcode, uint32_t op2);
    int ldrhPr(uint32_t opcode, uint32_t op2);
    int strhPr(uint32_t opcode, uint32_t op2);
    int ldrPr(uint32_t opcode, uint32_t op2);
    int strPr(uint32_t opcode, uint32_t op2);
    int ldrdPr(uint32_t opcode, uint32_t op2);
    int strdPr(uint32_t opcode, uint32_t op2);

    int ldrsbPt(uint32_t opcode, uint32_t op2);
    int ldrshPt(uint32_t opcode, uint32_t op2);
    int ldrbPt(uint32_t opcode, uint32_t op2);
    int strbPt(uint32_t opcode, uint32_t op2);
    int ldrhPt(uint32_t opcode, uint32_t op2);
    int strhPt(uint32_t opcode, uint32_t op2);
    int ldrPt(uint32_t opcode, uint32_t op2);
    int strPt(uint32_t opcode, uint32_t op2);
    int ldrdPt(uint32_t opcode, uint32_t op2);
    int strdPt(uint32_t opcode, uint32_t op2);
}

// Define functions for each ARM offset variation (half type)
#define HALF_FUNCS(func) \
    int Arm9::func##Ofrm(uint32_t opcode) { return func##Of(opcode, -rp(opcode)); } \
    int Arm9::func##Ofim(uint32_t opcode) { return func##Of(opcode, -ipH(opcode)); } \
    int Arm9::func##Ofrp(uint32_t opcode) { return func##Of(opcode, rp(opcode)); } \
    int Arm9::func##Ofip(uint32_t opcode) { return func##Of(opcode, ipH(opcode)); } \
    int Arm9::func##Prrm(uint32_t opcode) { return func##Pr(opcode, -rp(opcode)); } \
    int Arm9::func##Prim(uint32_t opcode) { return func##Pr(opcode, -ipH(opcode)); } \
    int Arm9::func##Prrp(uint32_t opcode) { return func##Pr(opcode, rp(opcode)); } \
    int Arm9::func##Prip(uint32_t opcode) { return func##Pr(opcode, ipH(opcode)); } \
    int Arm9::func##Ptrm(uint32_t opcode) { return func##Pt(opcode, -rp(opcode)); } \
    int Arm9::func##Ptim(uint32_t opcode) { return func##Pt(opcode, -ipH(opcode)); } \
    int Arm9::func##Ptrp(uint32_t opcode) { return func##Pt(opcode, rp(opcode)); } \
    int Arm9::func##Ptip(uint32_t opcode) { return func##Pt(opcode, ipH(opcode)); }

// Define functions for each ARM offset variation (full type)
#define FULL_FUNCS(func) \
    int Arm9::func##Ofim(uint32_t opcode) { return func##Of(opcode, -ip(opcode)); } \
    int Arm9::func##Ofip(uint32_t opcode) { return func##Of(opcode, ip(opcode)); } \
    int Arm9::func##Ofrmll(uint32_t opcode) { return func##Of(opcode, -rpll(opcode)); } \
    int Arm9::func##Ofrmlr(uint32_t opcode) { return func##Of(opcode, -rplr(opcode)); } \
    int Arm9::func##Ofrmar(uint32_t opcode) { return func##Of(opcode, -rpar(opcode)); } \
    int Arm9::func##Ofrmrr(uint32_t opcode) { return func##Of(opcode, -rprr(opcode)); } \
    int Arm9::func##Ofrpll(uint32_t opcode) { return func##Of(opcode, rpll(opcode)); } \
    int Arm9::func##Ofrplr(uint32_t opcode) { return func##Of(opcode, rplr(opcode)); } \
    int Arm9::func##Ofrpar(uint32_t opcode) { return func##Of(opcode, rpar(opcode)); } \
    int Arm9::func##Ofrprr(uint32_t opcode) { return func##Of(opcode, rprr(opcode)); } \
    int Arm9::func##Prim(uint32_t opcode) { return func##Pr(opcode, -ip(opcode)); } \
    int Arm9::func##Prip(uint32_t opcode) { return func##Pr(opcode, ip(opcode)); } \
    int Arm9::func##Prrmll(uint32_t opcode) { return func##Pr(opcode, -rpll(opcode)); } \
    int Arm9::func##Prrmlr(uint32_t opcode) { return func##Pr(opcode, -rplr(opcode)); } \
    int Arm9::func##Prrmar(uint32_t opcode) { return func##Pr(opcode, -rpar(opcode)); } \
    int Arm9::func##Prrmrr(uint32_t opcode) { return func##Pr(opcode, -rprr(opcode)); } \
    int Arm9::func##Prrpll(uint32_t opcode) { return func##Pr(opcode, rpll(opcode)); } \
    int Arm9::func##Prrplr(uint32_t opcode) { return func##Pr(opcode, rplr(opcode)); } \
    int Arm9::func##Prrpar(uint32_t opcode) { return func##Pr(opcode, rpar(opcode)); } \
    int Arm9::func##Prrprr(uint32_t opcode) { return func##Pr(opcode, rprr(opcode)); } \
    int Arm9::func##Ptim(uint32_t opcode) { return func##Pt(opcode, -ip(opcode)); } \
    int Arm9::func##Ptip(uint32_t opcode) { return func##Pt(opcode, ip(opcode)); } \
    int Arm9::func##Ptrmll(uint32_t opcode) { return func##Pt(opcode, -rpll(opcode)); } \
    int Arm9::func##Ptrmlr(uint32_t opcode) { return func##Pt(opcode, -rplr(opcode)); } \
    int Arm9::func##Ptrmar(uint32_t opcode) { return func##Pt(opcode, -rpar(opcode)); } \
    int Arm9::func##Ptrmrr(uint32_t opcode) { return func##Pt(opcode, -rprr(opcode)); } \
    int Arm9::func##Ptrpll(uint32_t opcode) { return func##Pt(opcode, rpll(opcode)); } \
    int Arm9::func##Ptrplr(uint32_t opcode) { return func##Pt(opcode, rplr(opcode)); } \
    int Arm9::func##Ptrpar(uint32_t opcode) { return func##Pt(opcode, rpar(opcode)); } \
    int Arm9::func##Ptrprr(uint32_t opcode) { return func##Pt(opcode, rprr(opcode)); }

// Create functions for instructions that have offset variations (half type)
HALF_FUNCS(ldrsb)
HALF_FUNCS(ldrsh)
HALF_FUNCS(ldrh)
HALF_FUNCS(strh)
HALF_FUNCS(ldrd)
HALF_FUNCS(strd)

// Create functions for instructions that have offset variations (full type)
FULL_FUNCS(ldrb)
FULL_FUNCS(strb)
FULL_FUNCS(ldr)
FULL_FUNCS(str)

inline uint32_t Arm9::ip(uint32_t opcode) { // #i (B/_)
    // Immediate offset for byte and word transfers
    return opcode & 0xFFF;
}

inline uint32_t Arm9::ipH(uint32_t opcode) { // #i (SB/SH/H/D)
    // Immediate offset for signed, half-word, and double word transfers
    return ((opcode >> 4) & 0xF0) | (opcode & 0xF);
}

inline uint32_t Arm9::rp(uint32_t opcode) { // Rm
    // Register offset for signed and half-word transfers
    return *registers[opcode & 0xF];
}

inline uint32_t Arm9::rpll(uint32_t opcode) { // Rm,LSL #i
    // Logical shift left by immediate
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return value << shift;
}

inline uint32_t Arm9::rplr(uint32_t opcode) { // Rm,LSR #i
    // Logical shift right by immediate
    // A shift of 0 translates to a shift of 32
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return shift ? (value >> shift) : 0;
}

inline uint32_t Arm9::rpar(uint32_t opcode) { // Rm,ASR #i
    // Arithmetic shift right by immediate
    // A shift of 0 translates to a shift of 32
    int32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return value >> (shift ? shift : 31);
}

inline uint32_t Arm9::rprr(uint32_t opcode) { // Rm,ROR #i
    // Rotate right by immediate
    // A shift of 0 translates to a1 rotate with carry of 1
    uint32_t value = *registers[opcode & 0xF];
    uint8_t shift = (opcode >> 7) & 0x1F;
    return shift ? ((value << (32 - shift)) | (value >> shift)) : (((cpsr & (1 << 29)) << 2) | (value >> 1));
}

inline int Arm9::ldrsbOf(uint32_t opcode, uint32_t op2) { // LDRSB Rd,[Rn,op2]
    // Signed byte load, pre-adjust without writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *op0 = (int8_t)Memory::read<uint8_t>(op1 + op2);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrshOf(uint32_t opcode, uint32_t op2) { // LDRSH Rd,[Rn,op2]
    // Signed half-word load, pre-adjust without writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *op0 = (int16_t)Memory::read<uint16_t>(op1 += op2);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrbOf(uint32_t opcode, uint32_t op2) { // LDRB Rd,[Rn,op2]
    // Byte load, pre-adjust without writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint8_t>(op1 + op2);

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strbOf(uint32_t opcode, uint32_t op2) { // STRB Rd,[Rn,op2]
    // Byte store, pre-adjust without writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    Memory::write<uint8_t>(op1 + op2, op0);
    return 1;
}

inline int Arm9::ldrhOf(uint32_t opcode, uint32_t op2) { // LDRH Rd,[Rn,op2]
    // Half-word load, pre-adjust without writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint16_t>(op1 += op2);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::strhOf(uint32_t opcode, uint32_t op2) { // STRH Rd,[Rn,op2]
    // Half-word store, pre-adjust without writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    Memory::write<uint16_t>(op1 + op2, op0);
    return 1;
}

inline int Arm9::ldrOf(uint32_t opcode, uint32_t op2) { // LDR Rd,[Rn,op2]
    // Word load, pre-adjust without writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint32_t>(op1 += op2);

    // Rotate misaligned reads
    if (op1 & 0x3) {
        uint8_t shift = (op1 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strOf(uint32_t opcode, uint32_t op2) { // STR Rd,[Rn,op2]
    // Word store, pre-adjust without writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(op1 + op2, op0);
    return 1;
}

inline int Arm9::ldrdOf(uint32_t opcode, uint32_t op2) { // LDRD Rd,[Rn,op2]
    // Double word load, pre-adjust without writeback
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    *registers[op0] = Memory::read<uint32_t>(op1 += op2);
    *registers[op0 + 1] = Memory::read<uint32_t>(op1 + 4);
    return 2;
}

inline int Arm9::strdOf(uint32_t opcode, uint32_t op2) { // STRD Rd,[Rn,op2]
    // Double word store, pre-adjust without writeback
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t op1 = *registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(op1 += op2, *registers[op0]);
    Memory::write<uint32_t>(op1 + 4, *registers[op0 + 1]);
    return 2;
}

inline int Arm9::ldrsbPr(uint32_t opcode, uint32_t op2) { // LDRSB Rd,[Rn,op2]!
    // Signed byte load, pre-adjust with writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    *op0 = (int8_t)Memory::read<uint8_t>(*op1 += op2);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrshPr(uint32_t opcode, uint32_t op2) { // LDRSH Rd,[Rn,op2]!
    // Signed half-word load, pre-adjust with writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = *op1 += op2;
    *op0 = (int16_t)Memory::read<uint16_t>(address);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrbPr(uint32_t opcode, uint32_t op2) { // LDRB Rd,[Rn,op2]!
    // Byte load, pre-adjust with writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint8_t>(*op1 += op2);

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strbPr(uint32_t opcode, uint32_t op2) { // STRB Rd,[Rn,op2]!
    // Byte store, pre-adjust with writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint8_t>(*op1 += op2, op0);
    return 1;
}

inline int Arm9::ldrhPr(uint32_t opcode, uint32_t op2) { // LDRH Rd,[Rn,op2]!
    // Half-word load, pre-adjust with writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = *op1 += op2;
    *op0 = Memory::read<uint16_t>(address);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::strhPr(uint32_t opcode, uint32_t op2) { // STRH Rd,[Rn,op2]!
    // Half-word store, pre-adjust with writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint16_t>(*op1 += op2, op0);
    return 1;
}

inline int Arm9::ldrPr(uint32_t opcode, uint32_t op2) { // LDR Rd,[Rn,op2]!
    // Word load, pre-adjust with writeback
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = *op1 += op2;
    *op0 = Memory::read<uint32_t>(address);

    // Rotate misaligned reads
    if (address & 0x3) {
        uint8_t shift = (address & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strPr(uint32_t opcode, uint32_t op2) { // STR Rd,[Rn,op2]!
    // Word store, pre-adjust with writeback
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(*op1 += op2, op0);
    return 1;
}

inline int Arm9::ldrdPr(uint32_t opcode, uint32_t op2) { // LDRD Rd,[Rn,op2]!
    // Double word load, pre-adjust with writeback
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    *registers[op0] = Memory::read<uint32_t>(*op1 += op2);
    *registers[op0 + 1] = Memory::read<uint32_t>(*op1 + 4);
    return 2;
}

inline int Arm9::strdPr(uint32_t opcode, uint32_t op2) { // STRD Rd,[Rn,op2]!
    // Double word store, pre-adjust with writeback
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(*op1 += op2, *registers[op0]);
    Memory::write<uint32_t>(*op1 + 4, *registers[op0 + 1]);
    return 2;
}

inline int Arm9::ldrsbPt(uint32_t opcode, uint32_t op2) { // LDRSB Rd,[Rn],op2
    // Signed byte load, post-adjust
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *op0 = (int8_t)Memory::read<uint8_t>(address);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrshPt(uint32_t opcode, uint32_t op2) { // LDRSH Rd,[Rn],op2
    // Signed half-word load, post-adjust
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *op0 = (int16_t)Memory::read<uint16_t>(address);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::ldrbPt(uint32_t opcode, uint32_t op2) { // LDRB Rd,[Rn],op2
    // Byte load, post-adjust
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *op0 = Memory::read<uint8_t>(address);

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strbPt(uint32_t opcode, uint32_t op2) { // STRB Rd,[Rn],op2
    // Byte store, post-adjust
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint8_t>(*op1, op0);
    *op1 += op2;
    return 1;
}

inline int Arm9::ldrhPt(uint32_t opcode, uint32_t op2) { // LDRH Rd,[Rn],op2
    // Half-word load, post-adjust
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *op0 = Memory::read<uint16_t>(address);

    // Handle pipelining
    if (op0 != registers[15]) return 1;
    flushPipeline();
    return 5;
}

inline int Arm9::strhPt(uint32_t opcode, uint32_t op2) { // STRH Rd,[Rn],op2
    // Half-word store, post-adjust
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint16_t>(*op1, op0);
    *op1 += op2;
    return 1;
}

inline int Arm9::ldrPt(uint32_t opcode, uint32_t op2) { // LDR Rd,[Rn],op2
    // Word load, post-adjust
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *op0 = Memory::read<uint32_t>(address);

    // Rotate misaligned reads
    if (address & 0x3) {
        uint8_t shift = (address & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }

    // Handle pipelining and THUMB switching
    if (op0 != registers[15]) return 1;
    cpsr |= (*op0 & 0x1) << 5;
    flushPipeline();
    return 5;
}

inline int Arm9::strPt(uint32_t opcode, uint32_t op2) { // STR Rd,[Rn],op2
    // Word store, post-adjust
    // When used as Rd, the program counter is read with +4
    uint32_t op0 = *registers[(opcode >> 12) & 0xF] + (((opcode & 0xF000) == 0xF000) << 2);
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(*op1, op0);
    *op1 += op2;
    return 1;
}

inline int Arm9::ldrdPt(uint32_t opcode, uint32_t op2) { // LDRD Rd,[Rn],op2
    // Double word load, post-adjust
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    uint32_t address = (*op1 += op2) - op2;
    *registers[op0] = Memory::read<uint32_t>(address);
    *registers[op0 + 1] = Memory::read<uint32_t>(address + 4);
    return 2;
}

inline int Arm9::strdPt(uint32_t opcode, uint32_t op2) { // STRD Rd,[Rn],op2
    // Double word store, post-adjust
    uint8_t op0 = (opcode >> 12) & 0xF;
    if (op0 == 15) return 1;
    uint32_t *op1 = registers[(opcode >> 16) & 0xF];
    Memory::write<uint32_t>(*op1, *registers[op0]);
    Memory::write<uint32_t>(*op1 + 4, *registers[op0 + 1]);
    *op1 += op2;
    return 2;
}

int Arm9::swpb(uint32_t opcode) { // SWPB Rd,Rm,[Rn]
    // Swap byte
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    uint32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint8_t>(op2);
    Memory::write<uint8_t>(op2, op1);
    return 2;
}

int Arm9::swp(uint32_t opcode) { // SWP Rd,Rm,[Rn]
    // Swap word
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    uint32_t op1 = *registers[opcode & 0xF];
    uint32_t op2 = *registers[(opcode >> 16) & 0xF];
    *op0 = Memory::read<uint32_t>(op2);
    Memory::write<uint32_t>(op2, op1);

    // Rotate misaligned reads
    if (op2 & 0x3) {
        uint8_t shift = (op2 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }
    return 2;
}

int Arm9::ldmda(uint32_t opcode) { // LDMDA Rn, <Rlist>
    // Block load, post-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(op0 += 4);
    }

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmda(uint32_t opcode) { // STMDA Rn, <Rlist>
    // Block store, post-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0 += 4, *registers[i]);
    }
    return m + (m < 2);
}

int Arm9::ldmia(uint32_t opcode) { // LDMIA Rn, <Rlist>
    // Block load, post-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(op0);
        op0 += 4;
    }

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmia(uint32_t opcode) { // STMIA Rn, <Rlist>
    // Block store, post-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0, *registers[i]);
        op0 += 4;
    }
    return m + (m < 2);
}

int Arm9::ldmdb(uint32_t opcode) { // LDMDB Rn, <Rlist>
    // Block load, pre-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(op0);
        op0 += 4;
    }

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdb(uint32_t opcode) { // STMDB Rn, <Rlist>
    // Block store, pre-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0, *registers[i]);
        op0 += 4;
    }
    return m + (m < 2);
}

int Arm9::ldmib(uint32_t opcode) { // LDMIB Rn, <Rlist>
    // Block load, pre-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(op0 += 4);
    }

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmib(uint32_t opcode) { // STMIB Rn, <Rlist>
    // Block store, pre-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0 += 4, *registers[i]);
    }
    return m + (m < 2);
}

int Arm9::ldmdaW(uint32_t opcode) { // LDMDA Rn!, <Rlist>
    // Block load, post-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] -= (m << 2));
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(address += 4);
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address - (m << 2);

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdaW(uint32_t opcode) { // STMDA Rn!, <Rlist>
    // Block store, post-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address += 4, *registers[i]);
    }
    *registers[op0] = address - (m << 2);
    return m + (m < 2);
}

int Arm9::ldmiaW(uint32_t opcode) { // LDMIA Rn!, <Rlist>
    // Block load, post-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] += (m << 2)) - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(address);
        address += 4;
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address;

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmiaW(uint32_t opcode) { // STMIA Rn!, <Rlist>
    // Block store, post-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, *registers[i]);
        address += 4;
    }
    *registers[op0] = address;
    return m + (m < 2);
}

int Arm9::ldmdbW(uint32_t opcode) { // LDMDB Rn!, <Rlist>
    // Block load, pre-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] -= (m << 2));
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(address);
        address += 4;
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address - (m << 2);

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdbW(uint32_t opcode) { // STMDB Rn!, <Rlist>
    // Block store, pre-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, *registers[i]);
        address += 4;
    }
    *registers[op0] = address - (m << 2);
    return m + (m < 2);
}

int Arm9::ldmibW(uint32_t opcode) { // LDMIB Rn!, <Rlist>
    // Block load, pre-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] += (m << 2)) - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(address += 4);
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address;

    // Handle pipelining and THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmibW(uint32_t opcode) { // STMIB Rn!, <Rlist>
    // Block store, pre-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address += 4, *registers[i]);
    }
    *registers[op0] = address;
    return m + (m < 2);
}

int Arm9::ldmdaU(uint32_t opcode) { // LDMDA Rn, <Rlist>^
    // User block load, post-decrement without writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(op0 += 4);
    }

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdaU(uint32_t opcode) { // STMDA Rn, <Rlist>^
    // User block store, post-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0 += 4, registersUsr[i]);
    }
    return m + (m < 2);
}

int Arm9::ldmiaU(uint32_t opcode) { // LDMIA Rn, <Rlist>^
    // User block load, post-increment without writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(op0);
        op0 += 4;
    }

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmiaU(uint32_t opcode) { // STMIA Rn, <Rlist>^
    // User block store, post-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0, registersUsr[i]);
        op0 += 4;
    }
    return m + (m < 2);
}

int Arm9::ldmdbU(uint32_t opcode) { // LDMDB Rn, <Rlist>^
    // User block load, pre-decrement without writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(op0);
        op0 += 4;
    }

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdbU(uint32_t opcode) { // STMDB Rn, <Rlist>^
    // User block store, pre-decrement without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0, registersUsr[i]);
        op0 += 4;
    }
    return m + (m < 2);
}

int Arm9::ldmibU(uint32_t opcode) { // LDMIB Rn, <Rlist>^
    // User block load, pre-increment without writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(op0 += 4);
    }

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmibU(uint32_t opcode) { // STMIB Rn, <Rlist>^
    // User block store, pre-increment without writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint32_t op0 = *registers[(opcode >> 16) & 0xF];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(op0 += 4, registersUsr[i]);
    }
    return m + (m < 2);
}

int Arm9::ldmdaUW(uint32_t opcode) { // LDMDA Rn!, <Rlist>^
    // User block load, post-decrement with writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] -= (m << 2));
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(address += 4);
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address - (m << 2);

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdaUW(uint32_t opcode) { // STMDA Rn!, <Rlist>^
    // User block store, post-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address += 4, registersUsr[i]);
    }
    *registers[op0] = address - (m << 2);
    return m + (m < 2);
}

int Arm9::ldmiaUW(uint32_t opcode) { // LDMIA Rn!, <Rlist>^
    // User block load, post-increment with writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] += (m << 2)) - (m << 2);
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(address);
        address += 4;
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address;

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmiaUW(uint32_t opcode) { // STMIA Rn!, <Rlist>^
    // User block store, post-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, registersUsr[i]);
        address += 4;
    }
    *registers[op0] = address;
    return m + (m < 2);
}

int Arm9::ldmdbUW(uint32_t opcode) { // LDMDB Rn!, <Rlist>^
    // User block load, pre-decrement with writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] -= (m << 2));
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(address);
        address += 4;
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address - (m << 2);

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmdbUW(uint32_t opcode) { // STMDB Rn!, <Rlist>^
    // User block store, pre-decrement with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0] - (m << 2);
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, registersUsr[i]);
        address += 4;
    }
    *registers[op0] = address - (m << 2);
    return m + (m < 2);
}

int Arm9::ldmibUW(uint32_t opcode) { // LDMIB Rn!, <Rlist>^
    // User block load, pre-increment with writeback; normal registers if branching
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = (*registers[op0] += (m << 2)) - (m << 2);
    uint32_t **regs = &registers[(~opcode & 0x8000) >> 11];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        *regs[i] = Memory::read<uint32_t>(address += 4);
    }

    // Load the writeback value if it's not last or is the only listed register
    if ((opcode & 0xFFFF & ~((2 << op0) - 1)) || (opcode & 0xFFFF) == (1 << op0))
        *registers[op0] = address;

    // Handle pipelining and mode/THUMB switching
    if (~opcode & 0x8000) return m + (m < 2);
    if (spsr) setCpsr(*spsr);
    cpsr |= (*registers[15] & 0x1) << 5;
    flushPipeline();
    return m + 4;
}

int Arm9::stmibUW(uint32_t opcode) { // STMIB Rn!, <Rlist>^
    // User block store, pre-increment with writeback
    uint8_t m = bitCount[opcode & 0xFF] + bitCount[(opcode >> 8) & 0xFF];
    uint8_t op0 = (opcode >> 16) & 0xF;
    uint32_t address = *registers[op0];
    for (int i = 0; i < 16; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address += 4, registersUsr[i]);
    }
    *registers[op0] = address;
    return m + (m < 2);
}

int Arm9::msrRc(uint32_t opcode) { // MSR CPSR,Rm
    // Write the first 8 bits of the status flags, only changing the CPU mode when not in user mode
    uint32_t op1 = *registers[opcode & 0xF];
    if (opcode & (1 << 16)) {
        uint8_t mask = ((cpsr & 0x1F) == 0x10) ? 0xE0 : 0xFF;
        setCpsr((cpsr & ~mask) | (op1 & mask));
    }

    // Write the remaining 8-bit blocks of the status flags
    for (int i = 1; i < 4; i++)
        if (opcode & (1 << (16 + i)))
            cpsr = (cpsr & ~(0xFF << (i << 3))) | (op1 & (0xFF << (i << 3)));
    return 1;
}

int Arm9::msrRs(uint32_t opcode) { // MSR SPSR,Rm
    // Write the saved status flags in 8-bit blocks
    if (!spsr) return 1;
    uint32_t op1 = *registers[opcode & 0xF];
    for (int i = 0; i < 4; i++)
        if (opcode & (1 << (16 + i)))
            *spsr = (*spsr & ~(0xFF << (i << 3))) | (op1 & (0xFF << (i << 3)));
    return 1;
}

int Arm9::msrIc(uint32_t opcode) { // MSR CPSR,#i
    // Rotate the immediate value
    uint32_t value = opcode & 0xFF;
    uint8_t shift = (opcode >> 7) & 0x1E;
    uint32_t op1 = (value << (32 - shift)) | (value >> shift);

    // Write the first 8 bits of the status flags, only changing the CPU mode when not in user mode
    if (opcode & (1 << 16)) {
        uint8_t mask = ((cpsr & 0x1F) == 0x10) ? 0xE0 : 0xFF;
        setCpsr((cpsr & ~mask) | (op1 & mask));
    }

    // Write the remaining 8-bit blocks of the status flags
    for (int i = 1; i < 4; i++)
        if (opcode & (1 << (16 + i)))
            cpsr = (cpsr & ~(0xFF << (i << 3))) | (op1 & (0xFF << (i << 3)));
    return 1;
}

int Arm9::msrIs(uint32_t opcode) { // MSR SPSR,#i
    // Rotate the immediate value
    if (!spsr) return 1;
    uint32_t value = opcode & 0xFF;
    uint8_t shift = (opcode >> 7) & 0x1E;
    uint32_t op1 = (value << (32 - shift)) | (value >> shift);

    // Write the saved status flags in 8-bit blocks
    for (int i = 0; i < 4; i++)
        if (opcode & (1 << (16 + i)))
            *spsr = (*spsr & ~(0xFF << (i << 3))) | (op1 & (0xFF << (i << 3)));
    return 1;
}

int Arm9::mrsRc(uint32_t opcode) { // MRS Rd,CPSR
    // Copy the status flags to a register
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    *op0 = cpsr;
    return 2;
}

int Arm9::mrsRs(uint32_t opcode) { // MRS Rd,SPSR
    // Copy the saved status flags to a register
    uint32_t *op0 = registers[(opcode >> 12) & 0xF];
    if (spsr) *op0 = *spsr;
    return 2;
}

int Arm9::mrc(uint32_t opcode) { // MRC Pn,<cpopc>,Rd,Cn,Cm,<cp>
    // Read from a CP15 register
    uint32_t *op2 = registers[(opcode >> 12) & 0xF];
    uint8_t op3 = (opcode >> 16) & 0xF;
    uint8_t op4 = opcode & 0xF;
    uint8_t op5 = (opcode >> 5) & 0x7;
    *op2 = 0;//Cp15::read(op3, op4, op5);
    return 1;
}

int Arm9::mcr(uint32_t opcode) { // MCR Pn,<cpopc>,Rd,Cn,Cm,<cp>
    // Write to a CP15 register
    uint32_t op2 = *registers[(opcode >> 12) & 0xF];
    uint8_t op3 = (opcode >> 16) & 0xF;
    uint8_t op4 = opcode & 0xF;
    uint8_t op5 = (opcode >> 5) & 0x7;
    //Cp15::write(op3, op4, op5, op2);
    return 1;
}

int Arm9::ldrsbRegT(uint16_t opcode) { // LDRSB Rd,[Rb,Ro]
    // Signed byte load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = (int8_t)Memory::read<uint8_t>(op1 + op2);
    return 1;
}

int Arm9::ldrshRegT(uint16_t opcode) { // LDRSH Rd,[Rb,Ro]
    // Signed half-word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = (int16_t)Memory::read<uint16_t>(op1 += op2);
    return 1;
}

int Arm9::ldrbRegT(uint16_t opcode) { // LDRB Rd,[Rb,Ro]
    // Byte load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = Memory::read<uint8_t>(op1 + op2);
    return 1;
}

int Arm9::strbRegT(uint16_t opcode) { // STRB Rd,[Rb,Ro]
    // Byte write, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    Memory::write<uint8_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrhRegT(uint16_t opcode) { // LDRH Rd,[Rb,Ro]
    // Half-word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = Memory::read<uint16_t>(op1 += op2);
    return 1;
}

int Arm9::strhRegT(uint16_t opcode) { // STRH Rd,[Rb,Ro]
    // Half-word write, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    Memory::write<uint16_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrRegT(uint16_t opcode) { // LDR Rd,[Rb,Ro]
    // Word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    *op0 = Memory::read<uint32_t>(op1 += op2);

    // Rotate misaligned reads
    if (op1 & 0x3) {
        uint8_t shift = (op1 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }
    return 1;
}

int Arm9::strRegT(uint16_t opcode) { // STR Rd,[Rb,Ro]
    // Word write, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = *registers[(opcode >> 6) & 0x7];
    Memory::write<uint32_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrbImm5T(uint16_t opcode) { // LDRB Rd,[Rb,#i]
    // Byte load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode & 0x07C0) >> 6;
    *op0 = Memory::read<uint8_t>(op1 + op2);
    return 1;
}

int Arm9::strbImm5T(uint16_t opcode) { // STRB Rd,[Rb,#i]
    // Byte store, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode & 0x07C0) >> 6;
    Memory::write<uint8_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrhImm5T(uint16_t opcode) { // LDRH Rd,[Rb,#i]
    // Half-word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 5) & 0x3E;
    *op0 = Memory::read<uint16_t>(op1 += op2);
    return 1;
}

int Arm9::strhImm5T(uint16_t opcode) { // STRH Rd,[Rb,#i]
    // Half-word store, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 5) & 0x3E;
    Memory::write<uint16_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrImm5T(uint16_t opcode) { // LDR Rd,[Rb,#i]
    // Word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 4) & 0x7C;
    *op0 = Memory::read<uint32_t>(op1 += op2);

    // Rotate misaligned reads
    if (op1 & 0x3) {
        uint8_t shift = (op1 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }
    return 1;
}

int Arm9::strImm5T(uint16_t opcode) { // STR Rd,[Rb,#i]
    // Word store, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[opcode & 0x7];
    uint32_t op1 = *registers[(opcode >> 3) & 0x7];
    uint32_t op2 = (opcode >> 4) & 0x7C;
    Memory::write<uint32_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldrPcT(uint16_t opcode) { // LDR Rd,[PC,#i]
    // PC-relative word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[15] & ~0x3;
    uint32_t op2 = (opcode & 0xFF) << 2;
    *op0 = Memory::read<uint32_t>(op1 += op2);

    // Rotate misaligned reads
    if (op1 & 0x3) {
        uint8_t shift = (op1 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }
    return 1;
}

int Arm9::ldrSpT(uint16_t opcode) { // LDR Rd,[SP,#i]
    // SP-relative word load, pre-adjust without writeback (THUMB)
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[13];
    uint32_t op2 = (opcode & 0xFF) << 2;
    *op0 = Memory::read<uint32_t>(op1 += op2);

    // Rotate misaligned reads
    if (op1 & 0x3) {
        uint8_t shift = (op1 & 0x3) << 3;
        *op0 = (*op0 << (32 - shift)) | (*op0 >> shift);
    }
    return 1;
}

int Arm9::strSpT(uint16_t opcode) { // STR Rd,[SP,#i]
    // SP-relative word store, pre-adjust without writeback (THUMB)
    uint32_t op0 = *registers[(opcode >> 8) & 0x7];
    uint32_t op1 = *registers[13];
    uint32_t op2 = (opcode & 0xFF) << 2;
    Memory::write<uint32_t>(op1 + op2, op0);
    return 1;
}

int Arm9::ldmiaT(uint16_t opcode) { // LDMIA Rb!,<Rlist>
    // Block load, post-increment with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF];
    uint32_t *op0 = registers[(opcode >> 8) & 0x7];
    uint32_t address = (*op0 += (m << 2)) - (m << 2);
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(address);
        address += 4;
    }
    return m + (m < 2);
}

int Arm9::stmiaT(uint16_t opcode) { // STMIA Rb!,<Rlist>
    // Block store, post-increment with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF];
    uint8_t op0 = (opcode >> 8) & 0x7;
    uint32_t address = *registers[op0];
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, *registers[i]);
        address += 4;
    }
    *registers[op0] = address;
    return m + (m < 2);
}

int Arm9::popT(uint16_t opcode) { // POP <Rlist>
    // SP-relative block load, post-increment with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF];
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(*registers[13]);
        *registers[13] += 4;
    }
    return m + (m < 2);
}

int Arm9::pushT(uint16_t opcode) { // PUSH <Rlist>
    // SP-relative block store, pre-decrement with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF];
    uint32_t address = (*registers[13] -= (m << 2));
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, *registers[i]);
        address += 4;
    }
    return m + (m < 2);
}

int Arm9::popPcT(uint16_t opcode) { // POP <Rlist>,PC
    // SP-relative block load, post-increment with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF] + 1;
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        *registers[i] = Memory::read<uint32_t>(*registers[13]);
        *registers[13] += 4;
    }

    // Load the program counter and handle pipelining
    *registers[15] = Memory::read<uint32_t>(*registers[13]);
    *registers[13] += 4;
    cpsr &= ~((~(*registers[15]) & 0x1) << 5);
    flushPipeline();
    return m + 4;
}

int Arm9::pushLrT(uint16_t opcode) { // PUSH <Rlist>,LR
    // SP-relative block store, pre-decrement with writeback (THUMB)
    uint8_t m = bitCount[opcode & 0xFF] + 1;
    uint32_t address = (*registers[13] -= (m << 2));
    for (int i = 0; i < 8; i++) {
        if (~opcode & (1 << i)) continue;
        Memory::write<uint32_t>(address, *registers[i]);
        address += 4;
    }

    // Store the link register
    Memory::write<uint32_t>(address, *registers[14]);
    return m + (m < 2);
}
