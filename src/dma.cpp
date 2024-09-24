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

#include <cstring>

#include "dma.h"
#include "interrupts.h"
#include "memory.h"
#include "spi.h"

namespace Dma {
    uint32_t counts[3];
    uint32_t srcAddrs[3];
    uint32_t dstAddrs[3];
    uint32_t spiControl;
    uint32_t spiCount;
    uint32_t spiAddress;
}

void Dma::reset() {
    // Reset the registers
    memset(counts, 0, sizeof(counts));
    memset(srcAddrs, 0, sizeof(srcAddrs));
    memset(dstAddrs, 0, sizeof(dstAddrs));
    spiCount = 0;
    spiAddress = 0;
}

uint32_t Dma::readSpiCount() {
    // Read from the SPI count register
    return spiCount;
}

uint32_t Dma::readCount(int i) {
    // Read from one of the general count registers
    return counts[i];
}

void Dma::writeSpiEnable(uint32_t mask, uint32_t value) {
    // Do nothing if the enable bit isn't set
    if (!(mask & value & 0x1))
        return;

    // Transfer bytes to or from the SPI
    if (spiControl & 0x1) // Write
        for (; spiCount != -1; spiCount--)
            Spi::writeData(0xFF, Memory::read<uint8_t>(spiAddress++));
    else // Read
        for (; spiCount != -1; spiCount--)
            Memory::write<uint8_t>(spiAddress++, Spi::readData());

    // Finish instantly and trigger an interrupt
    Interrupts::requestIrq(8);
}

void Dma::writeSpiControl(uint32_t mask, uint32_t value) {
    // Write to the SPI control register
    spiControl = (spiControl & ~mask) | (value & mask);
}

void Dma::writeSpiCount(uint32_t mask, uint32_t value) {
    // Write to the SPI count register
    spiCount = (spiCount & ~mask) | (value & mask);
}

void Dma::writeSpiAddress(uint32_t mask, uint32_t value) {
    // Write to the SPI address register
    spiAddress = (spiAddress & ~mask) | (value & mask);
}

void Dma::writeEnable(int i, uint32_t mask, uint32_t value) {
    // Do nothing if the enable bit isn't set
    if (!(mask & value & 0x1))
        return;

    // Transfer bytes from one memory address to another
    for (; counts[i] != -1; counts[i]--)
        Memory::write<uint8_t>(dstAddrs[i]++, Memory::read<uint8_t>(srcAddrs[i]++));

    // Finish instantly and trigger an interrupt
    Interrupts::requestIrq(10 + i);
}

void Dma::writeCount(int i, uint32_t mask, uint32_t value) {
    // Write to one of the general count registers
    counts[i] = (counts[i] & ~mask) | (value & mask);
}

void Dma::writeSrcAddr(int i, uint32_t mask, uint32_t value) {
    // Write to one of the general source address registers
    srcAddrs[i] = (srcAddrs[i] & ~mask) | (value & mask);
}

void Dma::writeDstAddr(int i, uint32_t mask, uint32_t value) {
    // Write to one of the general destination address registers
    dstAddrs[i] = (dstAddrs[i] & ~mask) | (value & mask);
}