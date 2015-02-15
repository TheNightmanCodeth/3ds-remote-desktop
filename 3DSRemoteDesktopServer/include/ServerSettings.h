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

#ifndef _SERVERSETTINGS_H_
#define _SERVERSETTINGS_H_

//#define LEAN_AND_MEAN
#include <Windows.h>

#include <map>
//#include "VitaInput.h"


struct ImageInfo
{
    int imageWidth;
    int imageHeight;
};

class CCaptureDX;
class CServerSettings
{
	friend class CCaptureDX;
	//friend class CVitaInput;

public:
    // TODO: include proper header file
    #define BIT(n) (1U<<(n)) // frop "types.h"
    // from "hid.h"
    enum PAD_KEY
    {
        KEY_A       = BIT(0),
        KEY_B       = BIT(1),
        KEY_SELECT  = BIT(2),
        KEY_START   = BIT(3),
        KEY_DRIGHT  = BIT(4),
        KEY_DLEFT   = BIT(5),
        KEY_DUP     = BIT(6),
        KEY_DDOWN   = BIT(7),
        KEY_R       = BIT(8),
        KEY_L       = BIT(9),
        KEY_X       = BIT(10),
        KEY_Y       = BIT(11),
        KEY_ZL      = BIT(14), // (new 3DS only)
        KEY_ZR      = BIT(15), // (new 3DS only)
        KEY_TOUCH   = BIT(20), // Not actually provided by HID
        KEY_CSTICK_RIGHT = BIT(24), // c-stick (new 3DS only)
        KEY_CSTICK_LEFT  = BIT(25), // c-stick (new 3DS only)
        KEY_CSTICK_UP    = BIT(26), // c-stick (new 3DS only)
        KEY_CSTICK_DOWN  = BIT(27), // c-stick (new 3DS only)
        KEY_CPAD_RIGHT = BIT(28), // circle pad
        KEY_CPAD_LEFT  = BIT(29), // circle pad
        KEY_CPAD_UP    = BIT(30), // circle pad
        KEY_CPAD_DOWN  = BIT(31), // circle pad

        // Generic catch-all directions
        KEY_UP    = KEY_DUP    | KEY_CPAD_UP,
        KEY_DOWN  = KEY_DDOWN  | KEY_CPAD_DOWN,
        KEY_LEFT  = KEY_DLEFT  | KEY_CPAD_LEFT,
        KEY_RIGHT = KEY_DRIGHT | KEY_CPAD_RIGHT,
    };
private:
    std::map<unsigned int, char> m_KeyBinding;
    short m_sDeadzone;
    short m_sPort;
	unsigned int m_unScreenOffsetX;
	unsigned int m_unScreenOffsetY;
	unsigned int m_unCropWidth;
	unsigned int m_unCropHeight;
	unsigned int m_unDestinationWidth;
	unsigned int m_unDestinationHeight;

	RECT m_CaptureRect;
	RECT m_DestinationRect; // the vita screen size of 960x544

	// hide constructors to make singleton
	CServerSettings(void);
	CServerSettings(CServerSettings&);
	CServerSettings& operator=(CServerSettings&);
public:
	~CServerSettings(void);
	static CServerSettings* GetInstance();

    bool LoadSettings(const char* szConfigFile);

    short GetDeadzone() { return m_sDeadzone;}
    short GetPort() { return m_sPort;}
    std::map<unsigned int, char>& GetKeybindingMapRef() { return m_KeyBinding;}

};

#endif
