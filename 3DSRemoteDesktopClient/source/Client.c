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

#include "Client.h"
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>

#define INVALID_SOCKET (int)(~0)
#include "RetroNetwork.h"

// other stuff
#include "ExtendedConsole.h"
#include "List.h"

#include "InputManager.h"

#include "lz4.h"
/*
#define PrintToScreen(...)
#define printf(...)
*/
u32 _IP;
struct sockaddr_in _ServerAddr;
int _ClientSocket;
bool _bHasServerInfo = false;
char* _szServerIP;
short _sServerPort;
bool _bConnected = false;

LinkedList* pPacketQueue = NULL;

typedef enum {CONNECTED, DISCONNECTED} ConnectionStatus_t;
ConnectionStatus_t _Status;


void Dispatch(Packet* pPacket)
{
    printf("[recv][size: %d][type: %d]\n", pPacket->_PacketSize, pPacket->_PacketID);

    switch(pPacket->_PacketID)
    {
    case PACKET_INPUT_PRESSED:
        break;
    case PACKET_INPUT_RELEASED:
        break;
    case PACKET_SCREEN:
        {
            // We recieved the screen
			// ScreenPacketData* pScreenShot = (ScreenPacketData*)pPacket->_PacketData;

            // Decompress the screen data
            // TODO: display image in the correct place... not in client!
            // NOTE: decompressing directly into the screen for speed and to avoid allocations
            /*u8* TopFrameBuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
            int nDecompressSize = LZ4_decompress_safe(pScreenShot->_ScreenData, TopFrameBuffer, pScreenShot->_CompressedSize, pScreenShot->_TotalSize);
            if(nDecompressSize < 0)
            {
                printf("Problem decompressing screen(%d)\n", nDecompressSize);
            }*/




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
            printf("Handshake Response Time: %f seconds\n", dDelta);
            */
        }
        break;
    case PACKET_MESSAGE:
            printf("Server: %s\n", pPacket->_PacketData);
        break;
    };

}

int nPrevRecvError = 0;
void RecievePacketData()
{
    int _errno;
    Packet* pRecvPacket;
    int ret = RNRecieveData(_ClientSocket, &pRecvPacket, &_errno);
    if(ret > 0)
    {
        Dispatch(pRecvPacket);
        free(pRecvPacket);
    }
    else if(ret == 0)
    {
        printf("We were disconnected from the server.\n");
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
        if(nPrevRecvError != _errno){\
        printf("[ERR %d] %s\n", ret, REPORT);\
        nPrevRecvError = _errno;\
        }\
        break;
        switch(-_errno)
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
            PrintToScreen(GFX_TOP, 0, 0, "recv() failed(%d) with code: 0X%X(%d)", ret, _errno, _errno);
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

        printf("[send][size: %d][type: %d]", nPacketLength, pToSend->_PacketID);
        int nSendAmmount = send(_ClientSocket, pToSend, nPacketLength, 0);
        if(nSendAmmount < 0)
        {
            error = errno;
            printf("Problem contacting server. Error code: 0X%X(%d)", error, error);
        }

        // release and refresh
        free(pToSend);
        pToSend = (Packet*)ListPop(pPacketQueue);
    }
}

static u32 *SOC_buffer = 0;

u32 soc_init(void) 
{
	Result ret;
	u32 result = 0;
	
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
	if (SOC_buffer != 0) 
	{
		ret = socInit(SOC_buffer, SOC_BUFFERSIZE);
		if (ret == 0) 
		{
			result = 1;
		} 
		else 
		{
			free(SOC_buffer);
		}
	}
	return result;
}

u32 soc_exit(void) 
{
	if (SOC_buffer) 
	{
		socExit();
		free(SOC_buffer);
		SOC_buffer = 0;
	}
	return 0;
}

void InitClient()
{
    _Status = DISCONNECTED;
    _bHasServerInfo = false;

    printf("Initializing SOC.");
    soc_init();

    // create the packet queue
    pPacketQueue = CreateList();

    // dont clsoe process when error occurs or something
    // signal(SIGPIPE, SIG_IGN);

/*
    _IP = (u32)gethostid();
    printf("IP: %d", _IP);
*/
}

void ShutdownClient()
{
    if(_ClientSocket != INVALID_SOCKET)
    {
        closesocket(_ClientSocket);
    }

	soc_exit();
}

bool SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) 
	   return false;

#ifdef WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags < 0) 
	   return false;
   flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
   
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

void ConnectClientToServer(char* szServerIP, short sPort)
{
    // close socket if exists
    if(_ClientSocket != INVALID_SOCKET || _bHasServerInfo)
    {
        closesocket(_ClientSocket);
    }

    // open a new socket
    printf("Opening Socket.\n");
    _ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(_ClientSocket == INVALID_SOCKET)
    {
        printf("Unable to create socket.\n");
        return;
    }

    // stop blocking
	if(!SetSocketBlockingEnabled(_ClientSocket, false))
    {
        printf("ioctl FIONBIO call failed.  Unable to make non-blocking.\n");
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
    printf("Set Server: %s:%d\n", _szServerIP, _sServerPort);

    // set the server info
    _ServerAddr.sin_family = AF_INET;
    _ServerAddr.sin_port = htons(sPort);
    _ServerAddr.sin_addr.s_addr = inet_addr(szServerIP);

    _bHasServerInfo = true;
}

int nPrevConnectError = 0;
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

        // recieve from server
        RecievePacketData();

        break;
    case DISCONNECTED:
        if(_bHasServerInfo)
        {
            PrintToScreen(GFX_TOP, 1, 1, "Connecting To Server at %s:%d", _szServerIP, _sServerPort);
            // connect to the server
            int ret = connect(_ClientSocket, (struct sockaddr *)&_ServerAddr, sizeof(_ServerAddr));
            if(ret >= 0)
            {
                printf("Connection to server successful!\n");
                //SendData(PACKET_SCREENRETRIEVED, 0, NULL);
                _Status = CONNECTED;
            }
            else
            {
                #define PRINT_CRAP(CASE, REPORT) \
                case CASE:\
                PrintToScreen(GFX_TOP, 1, 5, "[ERR %d] %s", ret, REPORT);\
                if(nPrevConnectError != error){\
                printf("[ERR %d] %s\n", ret, REPORT);\
                nPrevConnectError = error;\
                }\
                break;
                unsigned int error = errno;
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
                    printf("[ERR: EISCONN]Connection to server successful?\n");
                    _Status = CONNECTED;
                    break;
                default:
                    PrintToScreen(GFX_TOP, 1, 1, "connect() failed(ret %d) with code: 0X%X(%d)", ret, error, error);
                    break;
                }
                #undef PRINT_CRAP
                //printf("Connection to server failed!\n");
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
    printf("[send][size: %d][type: %d]\n", nDataSize, nPacketID);
    int nSendAmmount = RNSendData(_ClientSocket, nPacketID, nDataSize, Data);
    if(nSendAmmount < 0)
    {
        int error = errno;
        printf("Problem contacting server. Error code: 0X%X(%d)\n", error, error);
    }
}

int IsClientConnected()
{
    if(_Status == CONNECTED)
        return 1;
    return 0;
}