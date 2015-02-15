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

#define LEAN_AND_MEAN
#include <windows.h>
#include "Timer.h"


int CTimer::m_InstanceCount = 0;
CTimer* CTimer::m_pInstance = NULL;

CTimer* CTimer::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CTimer();
    }
    m_InstanceCount++;
    return m_pInstance;
}

void CTimer::ReleaseInstance()
{
    m_InstanceCount--;
    if(m_InstanceCount == 0)
    {
        if(m_pInstance != NULL)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }
}

CTimer::CTimer(void)
{
	m_bIsRunning = false;
	m_dElapsedTime = 0.0;

#ifdef _3DS_
    // TODO: startup anything for 3DS timer
#else
	QueryPerformanceFrequency( (LARGE_INTEGER*)&m_llFrequency );
	QueryPerformanceCounter( (LARGE_INTEGER*)&m_llStartTick );
	m_dFPSTimeStamp	= GetTickCount();
#endif // _3DS_

	m_nFrameCount		= 0;
	m_nFPS				= 0;
}

CTimer::~CTimer(void)
{

}

void CTimer::Start(void)
{
	if( !m_bIsRunning )
	{
		m_bIsRunning = true;
		Reset();
	}
}

void CTimer::Stop(void)
{
	if( m_bIsRunning )
	{
		m_bIsRunning = false;

		m_dElapsedTime = GetElapsedTime();
	}
}

void CTimer::Reset(void)
{
	m_dElapsedTime = 0.0;
	m_dFPSTimeStamp = 0;
#ifdef _3DS_
    // query the time for 3DS
#else
	QueryPerformanceCounter( (LARGE_INTEGER*)&m_llStartTick );
#endif _3DS_
}

// TODO: THis functionality
void CTimer::Pause(bool bPause)
{
}

void CTimer::Update(void)
{
	if( m_bIsRunning )
	{

#ifdef _3DS_
double dCurTime = 0.0f;
#else
		// query the tick count
		LONGLONG	llStopTick;
		QueryPerformanceCounter( (LARGE_INTEGER*)&llStopTick );

		// convert to the time
		double dCurTime = (double)(llStopTick - m_llStartTick) / (double)m_llFrequency;
#endif // _3DS_

		m_dDeltaTime = dCurTime - m_dElapsedTime;
		m_dElapsedTime = dCurTime;


		// Count FPS
		m_nFrameCount++;

		// check if 1 second has passed
		if( m_dElapsedTime - m_dFPSTimeStamp > 1.0 )
		{
			// remember the frame count
			m_nFPS = m_nFrameCount;

			// reset the current frame count
			m_nFrameCount = 0;

			// get a new time stamp
			m_dFPSTimeStamp = m_dElapsedTime;
		}
	}
}

double CTimer::GetElapsedTime(void)
{
	return m_dElapsedTime;
}

double CTimer::GetDeltaTime(void)
{
	return m_dDeltaTime;
}

int CTimer::FPS(void)
{
	return m_nFPS;
}
