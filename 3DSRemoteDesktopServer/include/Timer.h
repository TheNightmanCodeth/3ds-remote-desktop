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

#pragma once

#ifdef _3DS_
#define LONGLONG unsigned long
#else
//#define LEAN_AND_MEAN
#include <WinDef.h>
#endif // _3DS_

class CTimer
{
private:
	double		m_dElapsedTime; // stored time
	double		m_dDeltaTime;
	bool		m_bIsRunning;
	LONGLONG	m_llStartTick;
	LONGLONG	m_llFrequency; // how many ticks per second

	// for fps
	int			m_nFrameCount;
	double		m_dFPSTimeStamp;
	int			m_nFPS;


	CTimer(void);
	~CTimer(void);

	static int m_InstanceCount;
	static CTimer* m_pInstance;

public:

    static CTimer* GetInstance();
    static void ReleaseInstance();


	//stop watch functions
	void Start(void);
	void Stop(void);
	void Update(void);
	void Reset(void);

	// modifiers
	void Pause(bool bPause);

	// accessors
	double GetElapsedTime(void);
	double GetDeltaTime(void);
	int  FPS(void);
};
