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

#include "KeyboardCommand.h"
#include "ServerSettings.h"

#define WINVER 0x0500 // cuz this is old crap
#include <windows.h>

CKeyboardCommand::CKeyboardCommand()
{
    m_pSettings = CServerSettings::GetInstance();
    m_3DSKeyMap = m_pSettings->GetKeybindingMapRef();
}

void CKeyboardCommand::SetKeyboardKeyState(char cKeyCode, bool bPressed)
{
    if(bPressed)
    {
        keybd_event( cKeyCode, 0, KEYEVENTF_EXTENDEDKEY, 0 );
    }
    else
    {
        keybd_event( cKeyCode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}

void CKeyboardCommand::Translate3DSButtonToKeyState(unsigned int BTN, bool bPressed)
{
    this->SetKeyboardKeyState(m_3DSKeyMap[BTN], bPressed);
}

void CKeyboardCommand::Translate3DSAnalogToKeyState(short sX, short sY)
{
    int sDeadzone = m_pSettings->GetDeadzone();
    // left
    if(sX < -sDeadzone)
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_LEFT, true);
    }
    else
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_LEFT, false);
    }

    // right
    if(sX > sDeadzone)
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_RIGHT, true);
    }
    else
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_RIGHT, false);
    }

    // up
    if(sY > sDeadzone)
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_UP, true);
    }
    else
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_UP, false);
    }

    // down
    if(sY < -sDeadzone)
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_DOWN, true);
    }
    else
    {
        Translate3DSButtonToKeyState(CServerSettings::KEY_CPAD_DOWN, false);
    }
}
#undef WINVER
