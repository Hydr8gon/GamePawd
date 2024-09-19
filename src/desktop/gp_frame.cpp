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

#include "gp_frame.h"
#include "gp_canvas.h"
#include "../core.h"

wxBEGIN_EVENT_TABLE(gpFrame, wxFrame)
EVT_CLOSE(gpFrame::close)
wxEND_EVENT_TABLE()

gpFrame::gpFrame(): wxFrame(nullptr, wxID_ANY, "GamePawd") {
    // Set up a canvas for drawing the framebuffer
    gpCanvas *canvas = new gpCanvas(this);
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(canvas, 1, wxEXPAND);
    SetSizer(sizer);

    // Set up and show the window
    SetClientSize(MIN_SIZE);
    SetMinClientSize(MIN_SIZE);
    Centre();
    Show(true);

    // Boot the firmware
    Core::reset();
    Core::start();
}

void gpFrame::close(wxCloseEvent &event) {
    // Stop emulation before exiting
    Core::stop();
    event.Skip(true);
}
