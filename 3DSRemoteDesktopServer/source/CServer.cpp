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

#include "CServer.h"
#include <stdio.h>
#include <iostream>

using namespace std;


// TODO: GET THESE OUT OF HERE!!!!!!!!
int nFakeState = -1;


CServer::CServer()
{
    m_bRunning = false;
    m_pTimer = NULL;
}

// retrieved from internet somewhere
int CServer::GetMyIP()
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        cout << "Error " << WSAGetLastError() <<
                " when getting local host name." << endl;
        return 1;
    }
    cout << "Host name is " << ac << "." << endl;

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) {
        cout << "Yow! Bad host lookup." << endl;
        return 1;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
    }

    return 0;
}


void CServer::StartHandshake()
{
    m_dHandShakeTime = GetTickCount();//m_pTimer->GetElapsedTime();
    SendData(PACKET_HANDSHAKEREQUEST, 0, NULL);
}


void CServer::SendData(unsigned int nPacketID, unsigned int nDataSize, char* Data)
{
    if(nPacketID == PACKET_SCREENRETRIEVED)
    {
        m_bReadyForScreen = false;
    }

    // send
    printf("[send]");
    int nSendAmmount = RNSendData(m_ClientSocket, nPacketID, nDataSize, Data);
    printf("[size: %d][type: %d]\n", nSendAmmount, nPacketID);
    if(nSendAmmount == SOCKET_ERROR)
    {
        int nErrorCode = WSAGetLastError();
        printf("Problem contacting the client or client disconnected.  Error Code Ox%X(%d)\n", nErrorCode, nErrorCode);
        closesocket(m_ClientSocket);
        nFakeState = -1;
    }
}

void CServer::RecievePacketData()
{
    int errno;
    Packet* pRecvPacket;
    int ret = RNRecieveData(m_ClientSocket, &pRecvPacket, &errno);
    if(ret > 0)
    {
        Dispatch(pRecvPacket);
        free(pRecvPacket);
    }
    else if(ret == 0)
    {
        printf("The Client Disconnected.\n");
        nFakeState = -1;
        closesocket(m_ClientSocket);
        m_bReadyForScreen = false;
    }
    else
    {
        int errorNumber = WSAGetLastError();
        if(errorNumber != WSAEWOULDBLOCK && errorNumber != WSAECONNRESET)
        {
            printf("An error has occurred or the client timed out.  Error Code Ox%X(%d)\n", errorNumber, errorNumber);
            closesocket(m_ClientSocket);
            nFakeState = -1;
        }
    }
}


void CServer::Dispatch(Packet* pPacket)
{
    int ret = sizeof(Packet) + pPacket->_PacketSize;
    unsigned int nInput = 0;
    switch(pPacket->_PacketID)
    {
    case PACKET_INPUT_PRESSED:
        memcpy(&nInput, pPacket->_PacketData, sizeof(unsigned int));
        printf("[recv][size: %d][type: %d][key: %u]\n", ret, pPacket->_PacketID, nInput);
        m_KeyCommander.Translate3DSButtonToKeyState(nInput, true);
        break;
    case PACKET_INPUT_RELEASED:
        memcpy(&nInput, pPacket->_PacketData, sizeof(unsigned int));
        printf("[recv][size: %d][type: %d][key: %u]\n", ret, pPacket->_PacketID, nInput, nInput);
        m_KeyCommander.Translate3DSButtonToKeyState(nInput, false);
        break;
    case PACKET_INPUT_CIRCLEPAD:
        printf("[recv][size: %d][type: %d][x: %d | y: %d]\n", ret, pPacket->_PacketID, ((short*)pPacket->_PacketData)[0], ((short*)pPacket->_PacketData)[1]);

        m_KeyCommander.Translate3DSAnalogToKeyState(((short*)pPacket->_PacketData)[0], ((short*)pPacket->_PacketData)[1]);

        break;
    case PACKET_SCREEN:
        printf("[recv][size: %d][type: %d]\n", ret, pPacket->_PacketID);
        break;
    case PACKET_SCREENRETRIEVED:
        printf("[recv][size: %d][type: %d]\n", ret, pPacket->_PacketID);
        m_bReadyForScreen = true;
        break;
    case PACKET_HANDSHAKEREQUEST:
        SendData(PACKET_HANDSHAKERESPONSE, 0, NULL);
        printf("[recv][size: %d][type: %d]\n", ret, pPacket->_PacketID);
        break;
    case PACKET_HANDSHAKERESPONSE:
        {
            printf("[recv][size: %d][type: %d]\n", ret, pPacket->_PacketID);
            double dCurTime = GetTickCount();//m_pTimer->GetElapsedTime();
            double dDelta = dCurTime - m_dHandShakeTime;
            printf("Handshake Response Time: %f ms\n", dDelta);
        }
        break;
    case PACKET_MESSAGE:
        printf("[recv][size: %d][type: %d][message: %s]\n", ret, pPacket->_PacketID, (char*)&pPacket->_PacketData);
        break;
    };

    // release the packet
    free(pPacket);
}

bool CServer::Init(short sPort)
{
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &m_wsa) != 0)
    {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    if((m_ServerSocket = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = INADDR_ANY;
    m_ServerAddr.sin_port = htons( sPort );

    //Bind the socket to make it a server
    if( bind(m_ServerSocket ,(struct sockaddr *)&m_ServerAddr , sizeof(m_ServerAddr)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n" , WSAGetLastError());
    }
    printf("Bind done. using port %d \n", sPort);

    // stop blocking
    u_long NoBlock = 1;
    ioctlsocket(m_ServerSocket, FIONBIO, &NoBlock);

    // start listening
    listen(m_ServerSocket, 3);
    m_bRunning = true;

    // start any utilities
    m_bReadyForScreen = false;
    m_pTimer = CTimer::GetInstance();
    m_pTimer->Start();

    return 0;

}

bool CServer::HasClient()
{
    return nFakeState == 1;
}


bool CServer::ReadyForScreen()
{
    if(m_bReadyForScreen)
    {
        m_bReadyForScreen = false;
        return true;
    }
    return false;
}

void CServer::Run()
{
    switch(nFakeState)
    {
    case -1:
        printf("Awaiting connection... ");
        nFakeState = 0;
        break;
    case 0: // wait for connection
        {
            int c = sizeof(m_ClientAddr);
            m_ClientSocket = accept(m_ServerSocket, (struct sockaddr *)&m_ClientAddr, &c);
            if (m_ClientSocket != INVALID_SOCKET)
            {
                //printf("accept failed with error code : %d\n" , WSAGetLastError());
                printf("established\n");

                // handshake hte client
                StartHandshake();
                nFakeState = 1;
            }
        }
        break;
    case 1: // has connection
        {
            RecievePacketData();


            double dCurTime = GetTickCount();
            double dDelta = dCurTime - m_dHandShakeTime;
            if(dDelta > 15000)
            {
                StartHandshake();
            }
        }

        break;
    default: // dafuq
        break;
    };

}

void CServer::Shutdown()
{
    if(!m_bRunning)
    {
        return;
    }

    // disconnect all clients
    if(m_ClientSocket != INVALID_SOCKET)
    {
        shutdown(m_ClientSocket, 0);
        closesocket(m_ClientSocket);
    }

    closesocket(m_ServerSocket);
    WSACleanup();
    m_bRunning = false;

    // stop any utilities
    CTimer::ReleaseInstance();
    m_pTimer = NULL;
}
