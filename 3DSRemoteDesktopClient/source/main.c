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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <3ds.h>

#include "Client.h"
#include "Packet.h"
#include "InputManager.h"
#include "ExtendedConsole.h"

#include "lz4.h"

#define HANDLE_BUTTON(BTN)\
    temp = (unsigned int)(BTN);\
    if(KeyPressed(BTN))\
        SendData(PACKET_INPUT_PRESSED, sizeof(unsigned int), &temp);\
    else if(KeyReleased(BTN))\
        SendData(PACKET_INPUT_RELEASED, sizeof(unsigned int), &temp);

circlePosition _PrevCirclePad;
void DoInput()
{
    unsigned int temp = 0;
    HANDLE_BUTTON(BTN_A);
    HANDLE_BUTTON(BTN_B);
    HANDLE_BUTTON(BTN_X);
    HANDLE_BUTTON(BTN_Y);
    HANDLE_BUTTON(BTN_L);
    HANDLE_BUTTON(BTN_R);
    HANDLE_BUTTON(BTN_D_UP);
    HANDLE_BUTTON(BTN_D_DOWN);
    HANDLE_BUTTON(BTN_D_LEFT);
    HANDLE_BUTTON(BTN_D_RIGHT);
    HANDLE_BUTTON(BTN_START);
    HANDLE_BUTTON(BTN_SELECT);

    circlePosition circlePad = GetCirclepadState();
    if(memcmp(&_PrevCirclePad, &circlePad, sizeof(circlePosition)) != 0)
    {
        SendData(PACKET_INPUT_CIRCLEPAD, sizeof(circlePosition), (void*)&circlePad);
        memcpy(&_PrevCirclePad, &circlePad, sizeof(circlePosition));
    }
}

char* szIP;
short sPort = 8888;

int nCurInputIndex = 0;
void IPInput()
{
    if(KeyPressed(BTN_D_LEFT))
        nCurInputIndex--;
    if(KeyPressed(BTN_D_RIGHT))
        nCurInputIndex++;
    if(KeyPressed(BTN_L))
        sPort--;
    if(KeyPressed(BTN_R))
        sPort++;

    if(nCurInputIndex < 0)
        nCurInputIndex = 0;
    if(nCurInputIndex > 14)
        nCurInputIndex = 14;


    int curChar = (int)szIP[nCurInputIndex];
    if(curChar != '.')
    {
        if(KeyPressed(BTN_D_UP))
        {
            curChar++;
            if(curChar > (int)'9')
                curChar = (int)'9';
        }
        if(KeyPressed(BTN_D_DOWN))
        {
            curChar--;
            if(curChar < (int)'0')
                curChar = (int)'0';
        }
        szIP[nCurInputIndex] = (char)curChar;
    }

    PrintToScreen(GFX_TOP, 16, 11, "Connect to: %s:%d", szIP, sPort);
    PrintToScreen(GFX_TOP, 17, 23 + (nCurInputIndex), "^");


    if(KeyPressed(BTN_A))
    {
        ConnectClientToServer(szIP, sPort);
    }
}

u8 getModel()
{
	u8 model = 0;
	CFGU_GetSystemModel(&model);
	
	return model;
}

int main()
{
    szIP = (char*)malloc(15);
    memcpy(szIP, "192.168.000.004", 15);
    szIP[15] = '\0';

	// Initialize services
	srvInit();
	aptInit();
	cfguInit();
	hidInit();
	gfxInit(GSP_RGB565_OES, GSP_RGB565_OES, false);
	//gfxSet3D(true); // uncomment if using stereoscopic 3D
	
	if ((getModel() == 2) || (getModel() == 4))
		osSetSpeedupEnable(true);

    // start console
    //consoleDebugInit(debugDevice_CONSOLE);
    OpenExtendedConsole(GFX_BOTTOM); // NOTE: Built-in colsole forces GSP_RGB565_OES
    // printf("Opened\tConsole!");

    //printf("LZ4 Library version = %d\n", LZ4_versionNumber());

    InitInput();
    InitClient();

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();

		// clear the screens
        u8* TopFrameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
       // u8* BottomFrameBuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL); // commenting this as it is unused

        //memset(TopFrameBuffer, 0, 240*400*3);
        //memset(BottomFrameBuffer, 0, 240*320*3);

        // client
        UpdateInput();
        RunClient();

        if(IsClientConnected())
        {
            DoInput();
        }
        else
        {
            memset(TopFrameBuffer, 0, 240*400*4);
            IPInput();
        }


		if (KeyPressed(BTN_START))// && KeyDown(BTN_L) && KeyDown(BTN_R))
			break; // break in order to return to hbmenu

        /////////////////////////////////////////////////////////////////////////////

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

    DisconnectClient();
    ShutdownClient();


	free(szIP);

	// Exit services
	gfxExit();
	hidExit();
	cfguExit();
	aptExit();
	srvExit();
	return 0;
}

