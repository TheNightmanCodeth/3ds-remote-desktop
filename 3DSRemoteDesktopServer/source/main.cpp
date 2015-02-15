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

#include <windows.h>
#include <iostream>
#include <stdio.h>

#include <lz4.h>

#include <queue>

#include "CServer.h"
#include "CaptureDX.h"

using namespace std;
unsigned int g_nBufferFrameLimit = 2;
std::queue<ScreenPacketData*> g_ScreenBuffer;
 CCaptureDX g_ScreenCapture;


ScreenPacketData* CaptureScreenPacket()
{
    // poll the screen
    unsigned int nScreenSize = 0;
    char* pScreenInfo = g_ScreenCapture.CaptureScreen();

    // get the information to prep for things
    memcpy((char*)&nScreenSize, pScreenInfo, sizeof(unsigned int));

    // compress the screen data into the final packet
    // NOTE: we are allocate enough for hte full uncompressed so we know it will fit
    ScreenPacketData* pCompressedPacket = (ScreenPacketData*)malloc(sizeof(ScreenPacketData) + nScreenSize);
    int nCompressedSize = LZ4_compress(pScreenInfo + sizeof(unsigned int), pCompressedPacket->_ScreenData, nScreenSize);

    if(nCompressedSize > 0)
    {
        printf("Compressed packet %d/%d(%d%%)\n", nCompressedSize, nScreenSize, (int)(((float)nCompressedSize / (float)nScreenSize) * 100.0f) );

        // set the final packet data before send
        pCompressedPacket->_TotalSize = nScreenSize;
        pCompressedPacket->_CompressedSize = nCompressedSize;
    }
    else
    {
        printf("Problem compressing screen(%d)\n", nCompressedSize);
        free(pCompressedPacket);
        return NULL;
    }


    // free the data
    free(pScreenInfo);
    return pCompressedPacket;
}

ScreenPacketData* RequestScreenPacket()
{
    ScreenPacketData* pPacketToSend = NULL;

    if(g_ScreenBuffer.size() > 0)
    {
        pPacketToSend = g_ScreenBuffer.front();
        g_ScreenBuffer.pop();
    }
    else
    {
        pPacketToSend = CaptureScreenPacket();
    }
    return pPacketToSend;
}

int main()
{
    printf("LZ4 Library version = %d\n", LZ4_versionNumber());

	printf("Initializing the Capture Device...\n");
    HWND consoleHandle;
	HRESULT error = 0;
	CServerSettings* settings = CServerSettings::GetInstance();
	settings->LoadSettings("Config.ini");

	//consoleHandle = FindWindow(0, "Minecraft");
	consoleHandle = GetDesktopWindow();
	error = g_ScreenCapture.Init(consoleHandle);

	if(error != 0)
	{
		printf("Error #%d has occurred... Now exiting\n", error);
		return error;
	}
	printf("Initialization Complete!\n");


    CServer _server;
    _server.Init(settings->GetPort());
    _server.GetMyIP();


    // run the server
    bool bExit = false;
    while(!bExit)
    {
        _server.Run();

        if(_server.HasClient() )
        {
            if(_server.ReadyForScreen())
            {
                ScreenPacketData* pPacket = RequestScreenPacket();

                if(pPacket != NULL)
                {
                    // send it
                    int nFinalPacketSize = sizeof(ScreenPacketData) + pPacket->_CompressedSize;
                    _server.SendData(PACKET_SCREEN, nFinalPacketSize, (char*)pPacket);
                    free(pPacket);
                }
            }
            else
            {
                ScreenPacketData* pPacket = NULL;

                // dont allow more than the set ammount in the queue
                if(g_ScreenBuffer.size() == g_nBufferFrameLimit)
                {
                    pPacket = g_ScreenBuffer.front();
                    g_ScreenBuffer.pop();
                    free(pPacket);
                    pPacket = NULL;
                }

                pPacket = CaptureScreenPacket();
                if(pPacket != NULL)
                {
                    g_ScreenBuffer.push(pPacket);
                }
            }
        }

        if(GetAsyncKeyState(VK_ESCAPE))
        {
            bExit = true;
            cout << "Exiting..." << endl;
        }
    }

    // free the packet queue
    while(g_ScreenBuffer.size() > 0)
    {
        free(g_ScreenBuffer.front());
        g_ScreenBuffer.pop();
    }

    _server.Shutdown();
}
