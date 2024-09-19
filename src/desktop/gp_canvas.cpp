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

#include "gp_canvas.h"
#include "gp_app.h"
#include "../display.h"

#ifdef _WIN32
#include <GL/gl.h>
#include <GL/glext.h>
#endif

wxBEGIN_EVENT_TABLE(gpCanvas, wxGLCanvas)
EVT_PAINT(gpCanvas::draw)
EVT_SIZE(gpCanvas::resize)
wxEND_EVENT_TABLE()

gpCanvas::gpCanvas(gpFrame *frame): wxGLCanvas(frame, wxID_ANY, nullptr), frame(frame) {
    // Prepare the OpenGL context
    context = new wxGLContext(this);
    SetFocus();
}

void gpCanvas::draw(wxPaintEvent &event) {
    // Set the render context and clear the screen
    SetCurrent(*context);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Run initial setup once
    static bool setup = false;
    if (!setup) {
        // Prepare a texture for the framebuffer
        GLuint texture;
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Finish initial setup
        frame->SendSizeEvent();
        setup = true;
    }

    // At the swap interval, get the framebuffer as a texture
    if (++frameCount >= swapInterval) {
        if (uint32_t *buffer = Display::getBuffer()) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MIN_SIZE.x, MIN_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            frameCount = 0;
            delete buffer;
        }
    }

    // Submit the polygon vertices
    glBegin(GL_QUADS);
    glTexCoord2i(1, 1);
    glVertex2i(x + width, y + height);
    glTexCoord2i(0, 1);
    glVertex2i(x, y + height);
    glTexCoord2i(0, 0);
    glVertex2i(x, y);
    glTexCoord2i(1, 0);
    glVertex2i(x + width, y);
    glEnd();

    // Track the refresh rate and update the swap interval every second
    // Speed is limited by drawing, so this tries to keep it at 60 Hz
    refreshRate++;
    std::chrono::duration<double> rateTime = std::chrono::steady_clock::now() - lastRateTime;
    if (rateTime.count() >= 1.0f) {
        swapInterval = (refreshRate + 5) / 60; // Margin of 5
        refreshRate = 0;
        lastRateTime = std::chrono::steady_clock::now();
    }

    // Finish the frame
    glFinish();
    SwapBuffers();
}

void gpCanvas::resize(wxSizeEvent &event) {
    // Update the canvas dimensions
    SetCurrent(*context);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    wxSize size = GetSize();
    glOrtho(0, size.x, size.y, 0, -1, 1);
    glViewport(0, 0, size.x, size.y);
    wxSize min = MIN_SIZE;

    // Set the layout to be centered and as large as possible
    if ((float(size.x) / size.y) > (float(min.x) / min.y)) { // Wide
        width = min.x * size.y / min.y;
        height = size.y;
        x = (size.x - width) / 2;
        y = 0;
    }
    else { // Tall
        width = size.x;
        height = min.y * size.x / min.x;
        x = 0;
        y = (size.y - height) / 2;
    }
}
