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

#include "memory.h"
#include "display.h"
#include "dma.h"
#include "i2c.h"
#include "interrupts.h"
#include "spi.h"
#include "timers.h"

// Defines a 32-bit register in an I/O switch statement
#define DEF_IO32(addr, func) \
case addr + 0: case addr + 1: \
case addr + 2: case addr + 3: \
    base -= addr; \
    size = 4; \
    func; \
    break;

// Defines shared parameters for I/O register writes
#define IOWR_PARAMS mask << (base * 8), data << (base * 8)

namespace Memory {
    uint8_t ram[0x400000]; // 4MB RAM
    uint32_t counter;

    template <typename T> T ioRead(uint32_t address);
    template <typename T> void ioWrite(uint32_t address, T value);
}

void Memory::reset()
{
    // Reset the memory array
    memset(ram, 0, sizeof(ram));
    counter = 0;
}

template uint8_t Memory::read(uint32_t address);
template uint16_t Memory::read(uint32_t address);
template uint32_t Memory::read(uint32_t address);
template <typename T> T Memory::read(uint32_t address) {
    // Read an LSB-first value from an aligned RAM address or I/O register
    if ((address &= ~(sizeof(T) - 1)) < 0x40000000) {
        T value = 0;
        uint8_t *data = &ram[address & 0x3FFFFF];
        for (uint32_t i = 0; i < sizeof(T); i++)
            value |= data[i] << (i * 8);
        return value;
    }
    else if ((address >> 28) == 0xF) {
        return ioRead<T>(address);
    }

    // Handle unknown reads by returning nothing
    printf("Unmapped memory read: 0x%X\n", address);
    return 0;
}

template void Memory::write(uint32_t address, uint8_t value);
template void Memory::write(uint32_t address, uint16_t value);
template void Memory::write(uint32_t address, uint32_t value);
template <typename T> void Memory::write(uint32_t address, T value) {
    // Write an LSB-first value to an aligned RAM address or I/O register
    if ((address &= ~(sizeof(T) - 1)) < 0x40000000) {
        uint8_t *data = &ram[address & 0x3FFFFF];
        for (uint32_t i = 0; i < sizeof(T); i++)
            data[i] = value >> (i * 8);
        return;
    }
    else if ((address >> 28) == 0xF) {
        return ioWrite<T>(address, value);
    }

    // Handle unknown writes by doing nothing
    printf("Unmapped memory write: 0x%X\n", address);
}

template <typename T> T Memory::ioRead(uint32_t address) {
    // Read a value from one or more I/O registers
    T value = 0;
    for (uint32_t i = 0; i < sizeof(T);) {
        // Load data from a register
        uint32_t base, size, data;
        switch (base = address + i) {
            DEF_IO32(0xF0000000, data = 0x41040) // Hardware ID
            DEF_IO32(0xF0000408, data = Timers::readCounter())
            DEF_IO32(0xF0000410, data = Timers::readControl(0))
            DEF_IO32(0xF0000414, data = Timers::readTimer(0))
            DEF_IO32(0xF0000420, data = Timers::readControl(1))
            DEF_IO32(0xF0000424, data = Timers::readTimer(1))
            DEF_IO32(0xF0001208, data = Interrupts::readIrqEnable(0))
            DEF_IO32(0xF000120C, data = Interrupts::readIrqEnable(1))
            DEF_IO32(0xF0001210, data = Interrupts::readIrqEnable(2))
            DEF_IO32(0xF0001214, data = Interrupts::readIrqEnable(3))
            DEF_IO32(0xF0001218, data = Interrupts::readIrqEnable(4))
            DEF_IO32(0xF000121C, data = Interrupts::readIrqEnable(5))
            DEF_IO32(0xF0001220, data = Interrupts::readIrqEnable(6))
            DEF_IO32(0xF0001224, data = Interrupts::readIrqEnable(7))
            DEF_IO32(0xF0001228, data = Interrupts::readIrqEnable(8))
            DEF_IO32(0xF000122C, data = Interrupts::readIrqEnable(9))
            DEF_IO32(0xF0001230, data = Interrupts::readIrqEnable(10))
            DEF_IO32(0xF0001234, data = Interrupts::readIrqEnable(11))
            DEF_IO32(0xF0001238, data = Interrupts::readIrqEnable(12))
            DEF_IO32(0xF000123C, data = Interrupts::readIrqEnable(13))
            DEF_IO32(0xF0001240, data = Interrupts::readIrqEnable(14))
            DEF_IO32(0xF0001244, data = Interrupts::readIrqEnable(15))
            DEF_IO32(0xF0001248, data = Interrupts::readIrqEnable(16))
            DEF_IO32(0xF000124C, data = Interrupts::readIrqEnable(17))
            DEF_IO32(0xF0001250, data = Interrupts::readIrqEnable(18))
            DEF_IO32(0xF0001254, data = Interrupts::readIrqEnable(19))
            DEF_IO32(0xF0001258, data = Interrupts::readIrqEnable(20))
            DEF_IO32(0xF000125C, data = Interrupts::readIrqEnable(21))
            DEF_IO32(0xF0001260, data = Interrupts::readIrqEnable(22))
            DEF_IO32(0xF0001264, data = Interrupts::readIrqEnable(23))
            DEF_IO32(0xF0001268, data = Interrupts::readIrqEnable(24))
            DEF_IO32(0xF000126C, data = Interrupts::readIrqEnable(25))
            DEF_IO32(0xF0001270, data = Interrupts::readIrqEnable(26))
            DEF_IO32(0xF0001274, data = Interrupts::readIrqEnable(27))
            DEF_IO32(0xF0001278, data = Interrupts::readIrqEnable(28))
            DEF_IO32(0xF000127C, data = Interrupts::readIrqEnable(29))
            DEF_IO32(0xF0001280, data = Interrupts::readIrqEnable(30))
            DEF_IO32(0xF0001284, data = Interrupts::readIrqEnable(31))
            DEF_IO32(0xF00013F0, data = Interrupts::readIrqIndex())
            DEF_IO32(0xF00013F8, data = Interrupts::readPrioMask())
            DEF_IO32(0xF00013FC, data = Interrupts::readPrioClear())
            DEF_IO32(0xF00019F8, data = Interrupts::readPrioMask())
            DEF_IO32(0xF00019FC, data = Interrupts::readPrioClear())
            DEF_IO32(0xF0004050, data = Dma::readSpiCount())
            DEF_IO32(0xF0004114, data = Dma::readCount(0))
            DEF_IO32(0xF0004154, data = Dma::readCount(1))
            DEF_IO32(0xF0004194, data = Dma::readCount(2))
            DEF_IO32(0xF0004404, data = Spi::readControl())
            DEF_IO32(0xF0004408, data = Spi::readIrqFlags())
            DEF_IO32(0xF000440C, data = Spi::readFifoStat())
            DEF_IO32(0xF0004410, data = Spi::readData())
            DEF_IO32(0xF0004418, data = Spi::readIrqEnable())
            DEF_IO32(0xF0005800, data = I2c::readIrqFlags())
            DEF_IO32(0xF0005804, data = I2c::readIrqEnable())
            DEF_IO32(0xF0005C00, data = 0x1) // I2C stub
            DEF_IO32(0xF0005C04, data = I2c::readData(0))
            DEF_IO32(0xF0005C08, data = 0x20) // I2C stub
            DEF_IO32(0xF0005C18, data = I2c::readStatus(0))
            DEF_IO32(0xF0006000, data = 0x1) // I2C stub
            DEF_IO32(0xF0006004, data = I2c::readData(1))
            DEF_IO32(0xF0006008, data = 0x20) // I2C stub
            DEF_IO32(0xF0006018, data = I2c::readStatus(1))
            DEF_IO32(0xF0006400, data = 0x1) // I2C stub
            DEF_IO32(0xF0006404, data = I2c::readData(2))
            DEF_IO32(0xF0006408, data = 0x20) // I2C stub
            DEF_IO32(0xF0006418, data = I2c::readStatus(2))
            DEF_IO32(0xF0006800, data = 0x1) // I2C stub
            DEF_IO32(0xF0006804, data = I2c::readData(3))
            DEF_IO32(0xF0006808, data = 0x20) // I2C stub
            DEF_IO32(0xF0006818, data = I2c::readStatus(3))

        default:
            // Handle unknown reads by returning nothing
            if (i == 0) {
                printf("Unknown I/O register read: 0x%X\n", address);
                return 0;
            }

            // Ignore unknown reads after the first byte
            i++;
            continue;
        }

        // Add data to the return value and adjust byte offset
        value |= (data >> (base * 8)) << (i * 8);
        i += size - base;
    }
    return value;
}

template <typename T> void Memory::ioWrite(uint32_t address, T value) {
    // Write a value to one or more I/O registers
    for (uint32_t i = 0; i < sizeof(T);) {
        // Store data to a register
        uint32_t base, size, data = value >> (i * 8);
        uint32_t mask = (1ULL << ((sizeof(T) - i) * 8)) - 1;
        switch (base = address + i) {
            DEF_IO32(0xF0000400, Timers::writeTimerScale(IOWR_PARAMS))
            DEF_IO32(0xF0000404, Timers::writeCountScale(IOWR_PARAMS))
            DEF_IO32(0xF0000408, Timers::writeCounter(IOWR_PARAMS))
            DEF_IO32(0xF0000410, Timers::writeControl(0, IOWR_PARAMS))
            DEF_IO32(0xF0000414, Timers::writeTimer(0, IOWR_PARAMS))
            DEF_IO32(0xF0000418, Timers::writeTarget(0, IOWR_PARAMS))
            DEF_IO32(0xF0000420, Timers::writeControl(1, IOWR_PARAMS))
            DEF_IO32(0xF0000424, Timers::writeTimer(1, IOWR_PARAMS))
            DEF_IO32(0xF0000428, Timers::writeTarget(1, IOWR_PARAMS))
            DEF_IO32(0xF0001208, Interrupts::writeIrqEnable(0, IOWR_PARAMS))
            DEF_IO32(0xF000120C, Interrupts::writeIrqEnable(1, IOWR_PARAMS))
            DEF_IO32(0xF0001210, Interrupts::writeIrqEnable(2, IOWR_PARAMS))
            DEF_IO32(0xF0001214, Interrupts::writeIrqEnable(3, IOWR_PARAMS))
            DEF_IO32(0xF0001218, Interrupts::writeIrqEnable(4, IOWR_PARAMS))
            DEF_IO32(0xF000121C, Interrupts::writeIrqEnable(5, IOWR_PARAMS))
            DEF_IO32(0xF0001220, Interrupts::writeIrqEnable(6, IOWR_PARAMS))
            DEF_IO32(0xF0001224, Interrupts::writeIrqEnable(7, IOWR_PARAMS))
            DEF_IO32(0xF0001228, Interrupts::writeIrqEnable(8, IOWR_PARAMS))
            DEF_IO32(0xF000122C, Interrupts::writeIrqEnable(9, IOWR_PARAMS))
            DEF_IO32(0xF0001230, Interrupts::writeIrqEnable(10, IOWR_PARAMS))
            DEF_IO32(0xF0001234, Interrupts::writeIrqEnable(11, IOWR_PARAMS))
            DEF_IO32(0xF0001238, Interrupts::writeIrqEnable(12, IOWR_PARAMS))
            DEF_IO32(0xF000123C, Interrupts::writeIrqEnable(13, IOWR_PARAMS))
            DEF_IO32(0xF0001240, Interrupts::writeIrqEnable(14, IOWR_PARAMS))
            DEF_IO32(0xF0001244, Interrupts::writeIrqEnable(15, IOWR_PARAMS))
            DEF_IO32(0xF0001248, Interrupts::writeIrqEnable(16, IOWR_PARAMS))
            DEF_IO32(0xF000124C, Interrupts::writeIrqEnable(17, IOWR_PARAMS))
            DEF_IO32(0xF0001250, Interrupts::writeIrqEnable(18, IOWR_PARAMS))
            DEF_IO32(0xF0001254, Interrupts::writeIrqEnable(19, IOWR_PARAMS))
            DEF_IO32(0xF0001258, Interrupts::writeIrqEnable(20, IOWR_PARAMS))
            DEF_IO32(0xF000125C, Interrupts::writeIrqEnable(21, IOWR_PARAMS))
            DEF_IO32(0xF0001260, Interrupts::writeIrqEnable(22, IOWR_PARAMS))
            DEF_IO32(0xF0001264, Interrupts::writeIrqEnable(23, IOWR_PARAMS))
            DEF_IO32(0xF0001268, Interrupts::writeIrqEnable(24, IOWR_PARAMS))
            DEF_IO32(0xF000126C, Interrupts::writeIrqEnable(25, IOWR_PARAMS))
            DEF_IO32(0xF0001270, Interrupts::writeIrqEnable(26, IOWR_PARAMS))
            DEF_IO32(0xF0001274, Interrupts::writeIrqEnable(27, IOWR_PARAMS))
            DEF_IO32(0xF0001278, Interrupts::writeIrqEnable(28, IOWR_PARAMS))
            DEF_IO32(0xF000127C, Interrupts::writeIrqEnable(29, IOWR_PARAMS))
            DEF_IO32(0xF0001280, Interrupts::writeIrqEnable(30, IOWR_PARAMS))
            DEF_IO32(0xF0001284, Interrupts::writeIrqEnable(31, IOWR_PARAMS))
            DEF_IO32(0xF00013F8, Interrupts::writePrioMask(IOWR_PARAMS))
            DEF_IO32(0xF00019F8, Interrupts::writePrioMask(IOWR_PARAMS))
            DEF_IO32(0xF0004040, Dma::writeSpiEnable(IOWR_PARAMS))
            DEF_IO32(0xF0004044, Dma::writeSpiControl(IOWR_PARAMS))
            DEF_IO32(0xF0004050, Dma::writeSpiCount(IOWR_PARAMS))
            DEF_IO32(0xF0004054, Dma::writeSpiAddress(IOWR_PARAMS))
            DEF_IO32(0xF0004100, Dma::writeEnable(0, IOWR_PARAMS))
            DEF_IO32(0xF0004114, Dma::writeCount(0, IOWR_PARAMS))
            DEF_IO32(0xF0004118, Dma::writeSrcAddr(0, IOWR_PARAMS))
            DEF_IO32(0xF000411C, Dma::writeDstAddr(0, IOWR_PARAMS))
            DEF_IO32(0xF0004140, Dma::writeEnable(1, IOWR_PARAMS))
            DEF_IO32(0xF0004154, Dma::writeCount(1, IOWR_PARAMS))
            DEF_IO32(0xF0004158, Dma::writeSrcAddr(1, IOWR_PARAMS))
            DEF_IO32(0xF000415C, Dma::writeDstAddr(1, IOWR_PARAMS))
            DEF_IO32(0xF0004180, Dma::writeEnable(2, IOWR_PARAMS))
            DEF_IO32(0xF0004194, Dma::writeCount(2, IOWR_PARAMS))
            DEF_IO32(0xF0004198, Dma::writeSrcAddr(2, IOWR_PARAMS))
            DEF_IO32(0xF000419C, Dma::writeDstAddr(2, IOWR_PARAMS))
            DEF_IO32(0xF0004404, Spi::writeControl(IOWR_PARAMS))
            DEF_IO32(0xF0004408, Spi::writeIrqFlags(IOWR_PARAMS))
            DEF_IO32(0xF0004410, Spi::writeData(IOWR_PARAMS))
            DEF_IO32(0xF0004418, Spi::writeIrqEnable(IOWR_PARAMS))
            DEF_IO32(0xF0004420, Spi::writeReadCount(IOWR_PARAMS))
            DEF_IO32(0xF0004424, Spi::writeDevSelect(IOWR_PARAMS))
            DEF_IO32(0xF00050F8, Spi::writeGpioFlash(IOWR_PARAMS))
            DEF_IO32(0xF00050FC, Spi::writeGpioUic(IOWR_PARAMS))
            DEF_IO32(0xF0005804, I2c::writeIrqEnable(IOWR_PARAMS))
            DEF_IO32(0xF0005808, I2c::writeIrqAck(IOWR_PARAMS))
            DEF_IO32(0xF0005C04, I2c::writeData(0, IOWR_PARAMS))
            DEF_IO32(0xF0005C08, I2c::writeControl(0, IOWR_PARAMS))
            DEF_IO32(0xF0006004, I2c::writeData(1, IOWR_PARAMS))
            DEF_IO32(0xF0006008, I2c::writeControl(1, IOWR_PARAMS))
            DEF_IO32(0xF0006404, I2c::writeData(2, IOWR_PARAMS))
            DEF_IO32(0xF0006408, I2c::writeControl(2, IOWR_PARAMS))
            DEF_IO32(0xF0006804, I2c::writeData(3, IOWR_PARAMS))
            DEF_IO32(0xF0006808, I2c::writeControl(3, IOWR_PARAMS))
            DEF_IO32(0xF0009474, Display::writeFbAddr(IOWR_PARAMS))
            DEF_IO32(0xF0009500, Display::writePalAddr(IOWR_PARAMS))
            DEF_IO32(0xF0009504, Display::writePalData(IOWR_PARAMS))

        default:
            // Handle unknown writes by doing nothing
            if (i == 0) {
                printf("Unknown I/O register write: 0x%X @ 0x%X\n", value, address);
                return;
            }

            // Ignore unknown writes after the first byte
            i++;
            continue;
        }

        // Adjust the byte offset
        i += size - base;
    }
}
