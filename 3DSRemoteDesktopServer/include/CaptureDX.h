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


#ifndef _CAPTUREDX_H_
#define _CAPTUREDX_H_

#include <Windows.h>

#include "ServerSettings.h"

struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
class CServer;

class CWindowData;
class CCaptureDX
{
private:
	// server settings
	CServerSettings m_pSettings;

	// to init DirectX
	IDirect3D9* m_pInterface;
	IDirect3DDevice9* m_pDevice;
	CWindowData* m_pWindow;
	HWND m_DesktopWindowHandle;


	unsigned int m_unScreenWidth;
	unsigned int m_unScreenHeight;
	RECT m_ScreenRect;

	// for capturing the screen;
	IDirect3DSurface9* m_pFinalSurface;
	IDirect3DSurface9* m_pBufferSurface;

	unsigned int m_FinalImageSize;

	// for resizing
	struct PixelColor
	{
		BYTE A, R, G, B;
	};

	// functions
	void ScaleImage();
public:
	CCaptureDX(void);
	~CCaptureDX(void);

	// NOTE: was public
	char* CaptureScreen();

	int Init(CWindowData* pWindow);
	void Shutdown();
};

#endif // _CAPTUREDX_H_
