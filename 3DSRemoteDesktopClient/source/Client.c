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

#include <3ds.h>
#include <lz4.h>

#include "Client.h"
#include "RetroNetwork.h"

// other stuff
#include "ExtendedConsole.h"
#include "InputManager.h"
#include "thread.h"
#include "mutex.h"
#include "List.h"

volatile u32 _IP;
volatile struct sockaddr_in _ServerAddr;
volatile int _ClientSocket;
volatile bool _bHasServerInfo = false;
volatile char* _szServerIP;
volatile short _sServerPort;
volatile bool _bConnected = false;

volatile LinkedList* pPacketQueue = NULL;

typedef enum {CONNECTED, DISCONNECTED} ConnectionStatus_t;
volatile ConnectionStatus_t _Status;

// threads
ThreadData* pThread;
mutex* pMutex;

void Dispatch(Packet* pPacket)
{
    printf_safe("[recv][size: %d][type: %d]\n", pPacket->_PacketSize, pPacket->_PacketID);

    switch(pPacket->_PacketID)
    {
    case PACKET_INPUT_PRESSED:
        break;
    case PACKET_INPUT_RELEASED:
        break;
    case PACKET_SCREEN:
        {
            // We recieved the screen
            ScreenPacketData* pScreenShot = (ScreenPacketData*)pPacket->_PacketData;

            // Decompress the screen data
            // TODO: display image in the correct place... not in client!
            // NOTE: decompressing directly into the screen for speed and to avoid allocations

            u8* TopFrameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
            int nDecompressSize = LZ4_decompress_safe(pScreenShot->_ScreenData, TopFrameBuffer,
                                                      pScreenShot->_CompressedSize, pScreenShot->_TotalSize);
            if(nDecompressSize < 0)
            {
                printf_safe("Problem decompressing screen(%d)\n", nDecompressSize);
            }




            // request a new screen
            SendData(PACKET_SCREENRETRIEVED, 0, NULL);

        }
        break;
    case PACKET_HANDSHAKEREQUEST:
        SendData(PACKET_HANDSHAKERESPONSE, 0, NULL);
        break;
    case PACKET_HANDSHAKERESPONSE:
        {
            /*
            double dCurTime = m_pTimer.GetElapsedTime();
            double dDelta = dCurTime - m_dHandShakeTime;
            printf_safe("Handshake Response Time: %f seconds\n", dDelta);
            */
        }
        break;
    case PACKET_MESSAGE:
            printf_safe("Server: %s\n", pPacket->_PacketData);
        break;
    };

}

int nPrevRecvError = 0;
void RecievePacketData()
{
    int errno;
    Packet* pRecvPacket;
    //mutex_lock(pMutex);
    int ret = RNRecieveData(_ClientSocket, &pRecvPacket, &errno);
    //mutex_unlock(pMutex);
    if(ret > 0)
    {
        Dispatch(pRecvPacket);
        free(pRecvPacket);
    }
    else if(ret == 0)
    {
        printf_safe("We were disconnected from the server.\n");
        _Status = DISCONNECTED;
        closesocket(_ClientSocket);
    }
    else if(ret == -2)
    {
        PrintToScreen(GFX_TOP, 0, 0, "recv() failed(%d). Unable to Allocate space!");
    }
    else
    {
        #define PRINT_CRAP(CASE, REPORT) \
        case CASE:\
        PrintToScreen(GFX_TOP, 0, 0, "[ERR %d] %s", ret, REPORT);\
        if(nPrevRecvError != errno){\
        printf_safe("[ERR %d] %s\n", ret, REPORT);\
        nPrevRecvError = errno;\
        }\
        break;
        switch(-errno)
        {
            // PRINT_CRAP(EAGAIN, "EAGAIN: Trying again..."); // NOTE: Same code as EWOULDBLOCK
            PRINT_CRAP(EBADF, "The socket argument is not a valid file descriptor.");
            PRINT_CRAP(ECONNRESET, "A connection was forcibly closed by a peer.");
            PRINT_CRAP(EINTR, "The recv() function was interrupted by a signal that was caught, before any data was available.");
            PRINT_CRAP(EINVAL, "The MSG_OOB flag is set and no out-of-band data is available.");
            PRINT_CRAP(ENOTCONN, "A receive is attempted on a connection-mode socket that is not connected.");
            PRINT_CRAP(ENOTSOCK, "The socket argument does not refer to a socket.");
            PRINT_CRAP(EOPNOTSUPP, "The specified flags are not supported for this socket type or protocol.");
            PRINT_CRAP(ETIMEDOUT, "The connection timed out during connection establishment, or due to a transmission timeout on active connection.");
            PRINT_CRAP(EIO, "An I/O error occurred while reading from or writing to the file system.");
            PRINT_CRAP(ENOBUFS, "Insufficient resources were available in the system to perform the operation.");
            PRINT_CRAP(ENOMEM, "Insufficient memory was available to fulfill the request.");
        case EWOULDBLOCK:
            break;
        default:
            PrintToScreen(GFX_TOP, 0, 0, "recv() failed(%d) with code: 0X%X(%d)", ret, errno, errno);
            break;
        }
        #undef PRINT_CRAP
    }
}

void FlushSendQueue()
{
    Packet* pToSend = (Packet*)ListPop(pPacketQueue);
    int error = 0;
    while(pToSend != NULL)
    {
        int nPacketLength = sizeof(Packet) + pToSend->_PacketSize;

        printf_safe("[send][size: %d][type: %d]", nPacketLength, pToSend->_PacketID);

        mutex_lock(pMutex);
        int nSendAmmount = send(_ClientSocket, pToSend, nPacketLength, 0);
        mutex_unlock(pMutex);

        if(nSendAmmount < 0)
        {
            error = SOC_GetErrno();
            printf_safe("Problem contacting server. Error code: 0X%X(%d)", error, error);
        }

        // release and refresh
        free(pToSend);
        pToSend = (Packet*)ListPop(pPacketQueue);
    }
}


void StartThread()
{
    printf_safe("Thread starting...\n");
    thread_start(pThread);
    printf_safe("Thread Status: OX%X\n", pThread->ThreadStatus);
}

void ClientThread()
{
    while(1)
    {
        // recieve from server
        RecievePacketData();
        // printf_safe("Spam!\n");

        if(_Status != CONNECTED)
            break;
    }
}


void InitClient()
{
    _Status = DISCONNECTED;
    _bHasServerInfo = false;

    printf_safe("Initializing SOC.");
    SOC_Initialize((u32*)memalign(0x1000, 0x100000), 0x100000);

    // create the packet queue
    pPacketQueue = CreateList();

    // dont clsoe process when error occurs or something
    // signal(SIGPIPE, SIG_IGN);

    // create our thread stuff
    pMutex = mutex_request(false);
    pThread = thread_request(ClientThread, 0);
}

void ShutdownClient()
{
    thread_close(pThread);
    mutex_return(pMutex);

    if(_ClientSocket != INVALID_SOCKET)
    {
        closesocket(_ClientSocket);
    }

	SOC_Shutdown();
}

void ConnectClientToServer(char* szServerIP, short sPort)
{
    // close socket if exists
    if(_ClientSocket != INVALID_SOCKET || _bHasServerInfo)
    {
        closesocket(_ClientSocket);
    }

    // open a new socket
    printf_safe("Opening Socket.\n");
    _ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(_ClientSocket == INVALID_SOCKET)
    {
        printf_safe("Unable to create socket.\n");
        return;
    }

    // stop blocking
    long NoBlock = 1L;
    if (ioctl(_ClientSocket, (int)FIONBIO, (char *)&NoBlock))
    {
        printf_safe("ioctl FIONBIO call failed.  Unable to make non-blocking.\n");
    }


    // ensure we are freed
    if(_szServerIP != NULL)
    {
        free(_szServerIP);
    }

    // copy over new server info
    _sServerPort = sPort;
    _szServerIP = malloc(strlen(szServerIP));
    strcpy(_szServerIP, szServerIP);
    printf_safe("Set Server: %s:%d\n", _szServerIP, _sServerPort);

    // set the server info
    _ServerAddr.sin_family = AF_INET;
    _ServerAddr.sin_port = htons(sPort);
    _ServerAddr.sin_addr.s_addr = inet_addr(szServerIP);

    _bHasServerInfo = true;
}

void DisconnectClient()
{

    shutdown(_ClientSocket, 0);
    closesocket(_ClientSocket);


    // ensure we are freed
    if(_szServerIP != NULL)
    {
        free(_szServerIP);
    }
}


void SendData(unsigned int nPacketID, unsigned int nDataSize, void* Data)
{
    printf_safe("[send][size: %d][type: %d]\n", nDataSize, nPacketID);

    //mutex_lock(pMutex);
    int nSendAmmount = RNSendData(_ClientSocket, nPacketID, nDataSize, Data);
    //mutex_unlock(pMutex);

    if(nSendAmmount < 0)
    {
        int error = SOC_GetErrno();
        printf_safe("Problem contacting server. Error code: 0X%X(%d)\n", error, error);
    }
}

int IsClientConnected()
{
    if(_Status == CONNECTED)
        return 1;
    return 0;
}


volatile int nPrevConnectError = 0;
void RunClient()
{
    switch(_Status)
    {
    case CONNECTED:
        PrintToScreen(GFX_TOP, 1, 1, "CONNECTED!");

        // send all data in the packet queue
        //FlushSendQueue();

        if(KeyReleased(BTN_SELECT))
        {
            SendData(PACKET_SCREENRETRIEVED, 0, NULL);
        }

        //RecievePacketData();

        break;
    case DISCONNECTED:
        if(_bHasServerInfo)
        {
            PrintToScreen(GFX_TOP, 1, 1, "Connecting To Server at %s:%d", _szServerIP, _sServerPort);
            // connect to the server
            int ret = connect(_ClientSocket, (struct sockaddr *)&_ServerAddr, sizeof(_ServerAddr));
            if(ret >= 0)
            {
                printf_safe("Connection to server successful!\n");
                //SendData(PACKET_SCREENRETRIEVED, 0, NULL);
                _Status = CONNECTED;
                StartThread();
            }
            else
            {
                #define PRINT_CRAP(CASE, REPORT) \
                case CASE:\
                PrintToScreen(GFX_TOP, 1, 5, "[ERR %d] %s", ret, REPORT);\
                if(nPrevConnectError != error){\
                printf_safe("[ERR %d] %s\n", ret, REPORT);\
                nPrevConnectError = error;\
                }\
                break;
                unsigned int error = SOC_GetErrno();
                switch(-error)
                {
                PRINT_CRAP(EADDRNOTAVAIL, "The specified address is not available from the local machine.")
                PRINT_CRAP(EAFNOSUPPORT, "The specified address is not a valid address for the address family of the specified socket.")
                PRINT_CRAP(EALREADY, "A connection request is already in progress for the specified socket.")
                PRINT_CRAP(EBADF, "The socket argument is not a valid file descriptor.")
                PRINT_CRAP(ECONNREFUSED, "The target address was not listening for connections or refused the connection request.")
                PRINT_CRAP(EINPROGRESS, "O_NONBLOCK is set for the file descriptor for the socket and the connection cannot be immediately established; the connection shall be established asynchronously.")
                PRINT_CRAP(EINTR, "The attempt to establish a connection was interrupted by delivery of a signal that was caught; the connection shall be established asynchronously.")
                //PRINT_CRAP(EISCONN, "The specified socket is connection-mode and is already connected.")
                PRINT_CRAP(ENETUNREACH, "No route to the network is present.")
                PRINT_CRAP(ENOTSOCK, "The socket argument does not refer to a socket.")
                PRINT_CRAP(EPROTOTYPE, "The specified address has a different type than the socket bound to the specified peer address.")
                PRINT_CRAP(ETIMEDOUT, "The attempt to connect timed out before a connection was made.")
                PRINT_CRAP(EACCES, "Search permission is denied for a component of the path prefix; or write access to the named socket is denied.")
                PRINT_CRAP(EADDRINUSE, "Attempt to establish a connection that uses addresses that are already in use.")
                PRINT_CRAP(ECONNRESET, "Remote host reset the connection request.")
                PRINT_CRAP(EHOSTUNREACH, "The destination host cannot be reached (probably because the host is down or a remote router cannot reach it).")
                PRINT_CRAP(EINVAL, "The address_len argument is not a valid length for the address family; or invalid address family in the sockaddr structure.")
                PRINT_CRAP(ELOOP, "More than {SYMLOOP_MAX} symbolic links were encountered during resolution of the pathname in address.")
                PRINT_CRAP(ENAMETOOLONG, "Pathname resolution of a symbolic link produced an intermediate result whose length exceeds {PATH_MAX}.")
                PRINT_CRAP(ENETDOWN, "The local network interface used to reach the destination is down.")
                PRINT_CRAP(ENOBUFS, "No buffer space is available.")
                PRINT_CRAP(EOPNOTSUPP, "The socket is listening and cannot be connected.")
                case EISCONN:
                    printf_safe("[ERR: EISCONN]Connection to server successful.\n");
                    _Status = CONNECTED;
                    StartThread();
                    break;
                default:
                    PrintToScreen(GFX_TOP, 1, 1, "connect() failed(ret %d) with code: 0X%X(%d)", ret, error, error);
                    break;
                }
                #undef PRINT_CRAP
                //printf_safe("Connection to server failed!\n");
                // TODO: Handle ewouldblock and other stuff needed for non-blocking sockets
            }

        }
        else
        {
            PrintToScreen(GFX_TOP, 1, 1, "Waiting for Server Info");
        }
        break;
        default:
            PrintToScreen(GFX_TOP, 1, 1, "What?");
        break;
    };
}




