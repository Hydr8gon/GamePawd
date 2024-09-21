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

#pragma once

#include <cstdint>

namespace Dma {
    void reset();

    void writeSpiEnable(uint32_t mask, uint32_t value);
    void writeSpiControl(uint32_t mask, uint32_t value);
    void writeSpiCount(uint32_t mask, uint32_t value);
    void writeSpiAddress(uint32_t mask, uint32_t value);
}
