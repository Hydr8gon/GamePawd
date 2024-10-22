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

#include "gp_app.h"

enum AppEvent {
    UPDATE = 1
};

wxBEGIN_EVENT_TABLE(gpApp, wxApp)
EVT_TIMER(UPDATE, gpApp::update)
wxEND_EVENT_TABLE()

int gpApp::keyBinds[] = { 'S', 'W', 'D', 'A', 'I', 'O', 'K', 'L', 'V', 'B', 'G', 'H', 'P', 'Q', '0', '1' };

bool gpApp::OnInit() {
    // Create the app's frame
    SetAppName("GamePawd");
    frame = new gpFrame();

    // Set up the update timer
    timer = new wxTimer(this, UPDATE);
    timer->Start(6);
    return true;
}

int gpApp::OnExit() {
    // Stop the timer before exiting
    timer->Stop();
    return wxApp::OnExit();
}

void gpApp::update(wxTimerEvent &event) {
    // Continuously refresh the frame
    frame->Refresh();
}
