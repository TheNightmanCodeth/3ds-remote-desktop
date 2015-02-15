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


// This class is used to Init DirextX to capture the screen
//#define LEAN_AND_MEAN
#define SPLIT_IMAGE 0
#define AS_BITMAP 1

#include <Windows.h>

#include "ServerSettings.h"

struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
class CServer;


class CCaptureDX
{
private:
	// server settings
	CServerSettings m_pSettings;

	// to init DirectX
	IDirect3D9* m_pInterface;
	IDirect3DDevice9* m_pDevice;
	HWND m_pWindowHandle;


	unsigned int m_unScreenWidth;
	unsigned int m_unScreenHeight;
	RECT m_ScreenRect;

	// for capturing the screen;
	IDirect3DSurface9* m_pFinalSurface;
	IDirect3DSurface9* m_pBufferSurface;

#if SPLIT_IMAGE
	void* m_pPixelDataLeft;
	void* m_pPixelDataRight;
	void* m_pScaledPixelDataLeft;
	void* m_pScaledPixelDataRight;
#else
#if !AS_BITMAP
	void* m_pPixelData;
	void* m_pScaledPixelData;
#endif
#endif

#if AS_BITMAP
	unsigned int m_FinalImageSize;
#else
	unsigned int m_unPixelDataBufferSize;
	unsigned int m_unScaledPixelDataBufferSize;
#endif

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

	int Init(HWND windowHandle);
	void Shutdown();
};

#endif // _CAPTUREDX_H_
