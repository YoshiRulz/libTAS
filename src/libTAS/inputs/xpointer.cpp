/*
    Copyright 2015-2018 Clément Gallet <clement.gallet@ens-lyon.org>

    This file is part of libTAS.

    libTAS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libTAS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libTAS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xpointer.h"
#include "../logging.h"
#include "inputs.h"
#include "../../shared/AllInputs.h"
#include "../DeterministicTimer.h"

namespace libtas {

/* Override */ Bool XQueryPointer( Display* display, Window w,
        Window* root_return, Window* child_return,
        int* root_x_return, int* root_y_return,
        int* win_x_return, int* win_y_return,
        unsigned int* mask_return)
{
    DEBUGLOGCALL(LCF_MOUSE);
    *win_x_return = game_ai.pointer_x;
    *win_y_return = game_ai.pointer_y;
    *mask_return = SingleInput::toXlibPointerMask(game_ai.pointer_mask);

    /* I don't know what it should return */
    return 1;
}

/* Override */ int XGrabPointer(Display*, Window, Bool, unsigned int, int, int,
    Window, Cursor, Time)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return GrabSuccess;
}

/* Override */ int XUngrabPointer(Display*, Time)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return 0; // Not sure what to return
}

/* Override */ int XChangeActivePointerGrab(Display*, unsigned int, Cursor, Time)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return 0; // Not sure what to return
}

/* Override */ int XGrabButton(Display*, unsigned int, unsigned int, Window,
    Bool, unsigned int, int, int, Window, Cursor)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return GrabSuccess;
}

/* Override */ int XUngrabButton(Display*, unsigned int, unsigned int, Window)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return 0; // Not sure what to return
}

/* Override */ int XDefineCursor(Display*, Window, Cursor)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return 0; // Not sure what to return
}

/* Override */ int XUndefineCursor(Display*, Window)
{
    DEBUGLOGCALL(LCF_MOUSE);
    return 0; // Not sure what to return
}

/* Override */ int XWarpPointer( Display*, Window src_w, Window dest_w,
    int src_x, int src_y, unsigned int src_width, unsigned int src_height,
    int dest_x, int dest_y)
{
    debuglog(LCF_MOUSE, __func__, " called with dest_w ", dest_w, " and dest_x ", dest_x, " and dest_y ", dest_y);

    /* We have to generate an MotionNotify event. */
    XEvent event;
    event.xmotion.type = MotionNotify;
    event.xmotion.state = SingleInput::toXlibPointerMask(ai.pointer_mask);
    if (dest_w == None) {
        /* Relative warp */
        event.xmotion.x = game_ai.pointer_x + dest_x;
        event.xmotion.y = game_ai.pointer_y + dest_y;
    }
    else {
        /* Absolute warp */
        event.xmotion.x = dest_x;
        event.xmotion.y = dest_y;
    }
    event.xmotion.x_root = event.xmotion.x;
    event.xmotion.y_root = event.xmotion.y;
    event.xmotion.window = gameXWindow;

    struct timespec time = detTimer.getTicks();
    event.xmotion.time = time.tv_sec * 1000 + time.tv_nsec / 1000000;

    OWNCALL(XSendEvent(gameDisplay, gameXWindow, False, 0, &event));
    debuglog(LCF_EVENTS | LCF_MOUSE | LCF_UNTESTED, "Generate Xlib event MotionNotify with new position (", game_ai.pointer_x, ",", game_ai.pointer_y,")");

    /* Update the pointer coordinates */
    game_ai.pointer_x = event.xmotion.x;
    game_ai.pointer_y = event.xmotion.y;

    return 0; // Not sure what to return
}



}
