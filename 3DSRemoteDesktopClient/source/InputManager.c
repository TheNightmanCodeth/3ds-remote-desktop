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

#include "InputManager.h"

#define INPUT_PRESSED 1
#define INPUT_RELEASED 0
#define INPUT_STATE char

u32 m_Cur3DSButtonState;
u32 m_Prev3DSButtonState;

circlePosition m_CirclepadPosition;

void Poll3DSButtons()
{
    hidScanInput();
    hidCircleRead(&m_CirclepadPosition);
    m_Prev3DSButtonState = m_Cur3DSButtonState;
    m_Cur3DSButtonState = hidKeysHeld();
}

// TODO: Init 3DS stuff
void InitInput()
{
    memset(&m_CirclepadPosition, 0, sizeof(circlePosition));
    Poll3DSButtons();
}

void UpdateInput()
{
    Poll3DSButtons();
}

INPUT_STATE CurBtnStatus(_3DSButtons _BTN)
{
    return (m_Cur3DSButtonState & _BTN) == 0 ? INPUT_RELEASED : INPUT_PRESSED;
}

INPUT_STATE PrevBtnStatus(_3DSButtons _BTN)
{
    return (m_Prev3DSButtonState & _BTN) == 0 ? INPUT_RELEASED : INPUT_PRESSED;
}

int KeyDown(_3DSButtons _BTN)
{
    // check if the key is being held down
    return CurBtnStatus(_BTN) == INPUT_PRESSED;
}

int KeyUp(_3DSButtons _BTN)
{
    // check if the key is not down
    return CurBtnStatus(_BTN) == INPUT_RELEASED;
}

int KeyPressed(_3DSButtons _BTN)
{
    // check if the key has just been pressed
    return CurBtnStatus(_BTN) == INPUT_PRESSED && PrevBtnStatus(_BTN) == INPUT_RELEASED;
}

int KeyReleased(_3DSButtons _BTN)
{
    // check if the key has just been released
    return CurBtnStatus(_BTN) == INPUT_RELEASED && PrevBtnStatus(_BTN) == INPUT_PRESSED;
}

circlePosition GetCirclepadState()
{
    return m_CirclepadPosition;
}
