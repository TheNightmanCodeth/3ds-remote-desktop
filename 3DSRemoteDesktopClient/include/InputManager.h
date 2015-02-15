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

#ifndef _INPUTMANAGER_H
#define _INPUTMANAGER_H

#include <3ds.h>


// TODO: Handle 3DS button, touch, motion, and microphone input
// NOTE: 3DS input info in all found in ctrulib's 3ds/services/hid.h
typedef enum _3DSButtons_T{
               BTN_A        = KEY_A,
               BTN_B        = KEY_B,
               BTN_X        = KEY_X,
               BTN_Y        = KEY_Y,
               BTN_L        = KEY_L,
               BTN_R        = KEY_R,
               BTN_D_UP     = KEY_DUP,
               BTN_D_DOWN   = KEY_DDOWN,
               BTN_D_LEFT   = KEY_DLEFT,
               BTN_D_RIGHT  = KEY_DRIGHT,
               BTN_START    = KEY_START,
               BTN_SELECT   = KEY_SELECT} _3DSButtons;



void InitInput();
void UpdateInput();

int KeyDown(_3DSButtons _BTN);
int KeyUp(_3DSButtons _BTN);
int KeyPressed(_3DSButtons _BTN);
int KeyReleased(_3DSButtons _BTN);

circlePosition GetCirclepadState();


#endif // CINPUTMANAGER_H
