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


#include "CaptureDX.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9tex.h>

#include <winuser.h>

#include "Packet.h"
//#include "Server.h"

#define BITSPERPIXEL 32
#define SAFE_FREE(x) if(x != NULL){free(x); x = NULL;}

#include <fstream>

union INT_TO_CHARARR
{
    unsigned int _IntVal;
    char _CharArr[4];
};

struct RGB_565
{
    unsigned short _COLOR;
};

CCaptureDX::CCaptureDX(void)
{
}


CCaptureDX::~CCaptureDX(void)
{
}

// TODO: Make this run on its own thread
char* CCaptureDX::CaptureScreen()
{

    HRESULT err = m_pDevice->GetFrontBufferData(0, m_pBufferSurface);
	if(err != D3D_OK)
	{
	    switch(err)
	    {
	        case D3DERR_DRIVERINTERNALERROR:
	            printf("D3DERR_DRIVERINTERNALERROR - ");
	            break;
            case D3DERR_DEVICELOST:
	            printf("D3DERR_DEVICELOST - ");
	            break;
            case D3DERR_INVALIDCALL:
	            printf("D3DERR_INVALIDCALL - ");
	            break;
	    }
		printf("(%d) - Error on GetBackBuffer()\n", err);
	}


	err = D3DXLoadSurfaceFromSurface(m_pFinalSurface, NULL, NULL,
		m_pBufferSurface, NULL, NULL, D3DX_FILTER_LINEAR, 0);
	if(err != D3D_OK)
	{
		int invc = D3DERR_INVALIDCALL;
		int invd = D3DXERR_INVALIDDATA;
		printf("(%d) - Error on D3DXLoadSurfaceFromSurface()\n", err);
	}


    D3DSURFACE_DESC finalDesc;
    m_pFinalSurface->GetDesc(&finalDesc);

    int nW = finalDesc.Width;
    int nH = finalDesc.Height;
    int nPixelSize = 2; // in bytes
    m_FinalImageSize = nW * nH * nPixelSize;

    // return the screen data
    int nSizeSkip = sizeof(unsigned int);
    char* _ReturnData = (char*)calloc(1, m_FinalImageSize + nSizeSkip);
    memcpy(_ReturnData, (char*)&m_FinalImageSize, nSizeSkip);


    // RGB_565
    // Read the pixels
    D3DLOCKED_RECT bits;
    unsigned int nCurOffset = nSizeSkip;
    if(SUCCEEDED(m_pFinalSurface->LockRect(&bits, 0, D3DLOCK_READONLY)))
    {
        // have to loop to rotate -90 degrees
        for(int y = 0; y < nH; ++y)
        {
            unsigned short* pRow = (unsigned short*)(((BYTE*)bits.pBits) + (y * bits.Pitch));
            for(int x = 0; x < nW; ++x)
            {
                memcpy((_ReturnData + nSizeSkip) + ((nH * x + (nH - y)) * nPixelSize), pRow++, nPixelSize);
                nCurOffset += 3;
            }
        }
        m_pFinalSurface->UnlockRect();
    }

    return _ReturnData;
}

int CCaptureDX::Init(HWND windowHandle)
{
	// create the interface
	m_pInterface = Direct3DCreate9(D3D_SDK_VERSION);

	if(m_pInterface == NULL)
	{
		// couldnt create interface
		return 1;
	}

	// create the present params
	D3DPRESENT_PARAMETERS presentParams;
	memset(&presentParams, 0, sizeof(D3DPRESENT_PARAMETERS));
	presentParams.Windowed = true;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;

	// create the device
	UINT adapter = D3DADAPTER_DEFAULT;
	D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;
	DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	m_pWindowHandle = windowHandle;

	HRESULT error = m_pInterface->CreateDevice(adapter, deviceType, m_pWindowHandle, behaviorFlags, &presentParams, &m_pDevice);
	if(FAILED(error))
	{
		return error;
	}

/*
	// get the monitor the windows is a part of
	HMONITOR monitor = MonitorFromWindow(m_pWindowHandle, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	BOOL bGotMonitorInfo = GetMonitorInfo(monitor, &monitorInfo);

    // fill in the capture info
	// the screen size
	m_unScreenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	m_unScreenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    memcpy(&m_ScreenRect, &monitorInfo.rcMonitor, sizeof(RECT));
*/

    GetClientRect(m_pWindowHandle, &m_ScreenRect);
	m_unScreenWidth = m_ScreenRect.right - m_ScreenRect.left;
	m_unScreenHeight = m_ScreenRect.bottom - m_ScreenRect.top;


	// create the surface to draw into
	m_pDevice->CreateOffscreenPlainSurface(m_unScreenWidth, m_unScreenHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pBufferSurface, NULL);
	m_pDevice->CreateOffscreenPlainSurface(m_pSettings.m_unDestinationWidth, m_pSettings.m_unDestinationHeight,
                                        D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &m_pFinalSurface, NULL);

	return 0;
}

void CCaptureDX::Shutdown()
{
	// shutdown DirectX stuff
	if(m_pBufferSurface != NULL)
	{
		m_pBufferSurface->Release();
		m_pBufferSurface = NULL;
	}
	if(m_pFinalSurface != NULL)
	{
		m_pFinalSurface->Release();
		m_pFinalSurface = NULL;
	}
	if(m_pDevice != NULL)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if(m_pInterface != NULL)
	{
		m_pInterface->Release();
		m_pInterface = NULL;
	}
}

