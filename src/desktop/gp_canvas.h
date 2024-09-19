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

#include <chrono>
#include <wx/wx.h>
#include <wx/glcanvas.h>

class gpFrame;

class gpCanvas: public wxGLCanvas {
public:
    gpCanvas(gpFrame *frame);

private:
    gpFrame *frame;
    wxGLContext *context;

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t x = 0;
    uint32_t y = 0;

    int frameCount = 0;
    int swapInterval = 0;
    int refreshRate = 0;
    std::chrono::steady_clock::time_point lastRateTime;

    void draw(wxPaintEvent &event);
    void resize(wxSizeEvent &event);
    wxDECLARE_EVENT_TABLE();
};
