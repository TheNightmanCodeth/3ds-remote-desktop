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

#include "ServerSettings.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

CServerSettings::CServerSettings(void)
{
	// the "crop" dimentions
	m_unScreenOffsetX = 0;
	m_unScreenOffsetY = 0;
	m_unCropWidth = 1920;
	m_unCropHeight = 1080;

	// the 3ds screen size
	m_unDestinationWidth = 400;
	m_unDestinationHeight = 240;

/*
	// TODO: Load these in from a file
	// the key bindings
	m_KeyBinding[CVitaInput::DPadLeft]	= 'a';
	m_KeyBinding[CVitaInput::DPadUp]	= 'w';
	m_KeyBinding[CVitaInput::DPadRight] = 'd';
	m_KeyBinding[CVitaInput::DPadDown]	= 's';
	m_KeyBinding[CVitaInput::Square]	= '2';
	m_KeyBinding[CVitaInput::Triangle]	= '4';
	m_KeyBinding[CVitaInput::Circle]	= '3';
	m_KeyBinding[CVitaInput::Cross]		= '1';
	m_KeyBinding[CVitaInput::Start]		= VK_RETURN;
	m_KeyBinding[CVitaInput::Select]	= VK_SHIFT;
	m_KeyBinding[CVitaInput::L]			= 'q';
	m_KeyBinding[CVitaInput::R]			= 'e';
*/
}

CServerSettings::~CServerSettings(void)
{
}

CServerSettings::CServerSettings(CServerSettings&)
{
}

CServerSettings& CServerSettings::operator=(CServerSettings& other)
{
	return other;
}

CServerSettings* CServerSettings::GetInstance()
{
	static CServerSettings pInstance;
	return &pInstance;
}

bool CServerSettings::LoadSettings(const char* szConfigFile)
    {
    string option, equals, value;
    ifstream myfile (szConfigFile);
    if (myfile.is_open())
    {
        while ( myfile >> option >> equals >> value )
        {
            int nTrueValue = atoi(value.c_str());
            if(option == "PORT")
            {
                m_sPort = (short)nTrueValue;
            }
            else if(option == "CPDEADZONE")
            {
                m_sDeadzone = (short)nTrueValue;
            }
            else if(option == "SCREEDWIDTH")
            {
                m_unDestinationWidth = nTrueValue;
            }
            else if(option == "SCREENHEIGHT")
            {
                m_unDestinationHeight = nTrueValue;
            }
            else if(option.find("BTN_", 0) != string::npos)
            {
                int nBtnValue = -1;
                if(option == "BTN_A")
                {
                    nBtnValue = KEY_A;
                }
                else if(option == "BTN_B")
                {
                    nBtnValue = KEY_B;
                }
                else if(option == "BTN_X")
                {
                    nBtnValue = KEY_X;
                }
                else if(option == "BTN_Y")
                {
                    nBtnValue = KEY_Y;
                }
                else if(option == "BTN_L")
                {
                    nBtnValue = KEY_L;
                }
                else if(option == "BTN_R")
                {
                    nBtnValue = KEY_R;
                }
                else if(option == "BTN_DUP")
                {
                    nBtnValue = KEY_DUP;
                }
                else if(option == "BTN_DDOWN")
                {
                    nBtnValue = KEY_DDOWN;
                }
                else if(option == "BTN_DLEFT")
                {
                    nBtnValue = KEY_DLEFT;
                }
                else if(option == "BTN_DRIGHT")
                {
                    nBtnValue = KEY_DRIGHT;
                }
                else if(option == "BTN_CPUP")
                {
                    nBtnValue = KEY_CPAD_UP;
                }
                else if(option == "BTN_CPDOWN")
                {
                    nBtnValue = KEY_CPAD_DOWN;
                }
                else if(option == "BTN_CPLEFT")
                {
                    nBtnValue = KEY_CPAD_LEFT;
                }
                else if(option == "BTN_CPRIGHT")
                {
                    nBtnValue = KEY_CPAD_RIGHT;
                }
                else
                {
                    cout << "Unhandled button [" << option << equals << value << "]\n";
                }
                m_KeyBinding[nBtnValue] = nTrueValue;
            }
            else
            {
                cout << "Unhandled setting [" << option << equals << value << "]\n";
            }
        }
        myfile.close();

        // the RECT that represents the destination's screen
        m_DestinationRect.left = 0;
        m_DestinationRect.right = m_unDestinationWidth;
        m_DestinationRect.top = 0;
        m_DestinationRect.bottom = m_unDestinationHeight;

        // the RECT that represents the "crop" location
        m_CaptureRect.left = m_unScreenOffsetX;
        m_CaptureRect.right = m_unScreenOffsetX + m_unCropWidth;
        m_CaptureRect.top = m_unScreenOffsetY;
        m_CaptureRect.bottom = m_unScreenOffsetY + m_unCropHeight;

    }

    else
    {
        cout << "Unable to open file";
    }

}







