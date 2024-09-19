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

#include <wx/wx.h>

#define MIN_SIZE wxSize(854, 480)
class gpCanvas;

class gpFrame: public wxFrame {
public:
    gpFrame();

private:
    gpCanvas *canvas;

    void close(wxCloseEvent &event);
    wxDECLARE_EVENT_TABLE();
};
