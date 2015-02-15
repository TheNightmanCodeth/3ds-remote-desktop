/*
Copyright (c) 2015, Erick<RetroZelda>Folckemer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ExtendedConsole.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define CONSOLE_STRING_BUFFER_SIZE 64

gfxScreen_t _Screen;

PrintConsole* _TopConsole;
PrintConsole* _BottomConsole;
const char* _szMoveCursor = "\x1b[%d;%dH";

PrintConsole TopConsoleSettings;

PrintConsole BottomConsoleSettings;


void OpenExtendedConsole(gfxScreen_t DefaultScreen)
{
    PrintConsole* def = consoleGetDefault();
    memcpy(&TopConsoleSettings, def, sizeof(PrintConsole));
    memcpy(&BottomConsoleSettings, def, sizeof(PrintConsole));

    _TopConsole = consoleInit(GFX_TOP, &TopConsoleSettings);
    _BottomConsole = consoleInit(GFX_BOTTOM, &BottomConsoleSettings);
    _Screen = DefaultScreen;

    switch(DefaultScreen)
    {
    case GFX_TOP:
        consoleSelect(_TopConsole);
        break;
    case GFX_BOTTOM:
        consoleSelect(_BottomConsole);
        break;
    }
}

// void PrintToScreen(gfxScreen_t TargetScreen, u16 X, u16 Y, char* szString, ...){}

void PrintToScreen(gfxScreen_t TargetScreen, u16 X, u16 Y, char* szString, ...)
{
    PrintConsole* _Console;
    PrintConsole* _RetConsole;
    switch(TargetScreen)
    {
    case GFX_TOP:
        _Console = _TopConsole;
        _RetConsole = consoleSelect(_TopConsole);
        break;
    case GFX_BOTTOM:
        _Console = _BottomConsole;
        _RetConsole = consoleSelect(_BottomConsole);
        break;
    }

    int retX = _Console->cursorX;
    int retY = _Console->cursorY;
    _Console->cursorX = Y;
    _Console->cursorY = X;

    // print it
    va_list arguments;
    va_start ( arguments, szString );

    vprintf(szString, arguments);

    va_end ( arguments );


    _Console->cursorX = retX;
    _Console->cursorY = retY;

    consoleSelect(_RetConsole);
}
