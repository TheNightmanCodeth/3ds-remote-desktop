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

#ifndef _CSERVER_H_
#define _CSERVER_H_

#include "RetroNetwork.h"

#include "Timer.h"
#include "Packet.h"
#include "KeyboardCommand.h"

class CServer
{
    // utilities
private:
    CTimer* m_pTimer;

    // Server connection
private:
    WSADATA m_wsa;
    SOCKET m_ServerSocket;
    SOCKET m_ClientSocket;
    sockaddr_in m_ServerAddr;
    sockaddr_in m_ClientAddr;

    bool m_bRunning;
    bool m_bReadyForScreen;

    //server operations
private:

    CKeyboardCommand m_KeyCommander;
    double m_dHandShakeTime;

    void StartHandshake();

    void RecievePacketData();
    void Dispatch(Packet* pPacket);

public:

    CServer();

    int GetMyIP();
    bool Init(short sPort);
    void Run();
    void Shutdown();

    void SendData(unsigned int nPacketID, unsigned int nDataSize, char* Data);

    bool HasClient();
    bool ReadyForScreen();
};

#endif // _CSERVER_H_
