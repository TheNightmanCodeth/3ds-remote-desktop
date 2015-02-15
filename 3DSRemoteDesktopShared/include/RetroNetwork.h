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

#include "Packet.h"

#ifndef _RETRONETWORK_H_
#define _RETRONETWORK_H_

#ifdef _3DS

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <3ds.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>

#define GETERRNO SOC_GetErrno

#else

#include <winsock2.h>
#include <windows.h>

#define GETERRNO WSAGetLastError

#endif // _3DS

#ifdef __cplusplus
extern "C" {
#endif

// in:
//      Socket to send to
//      packet ID of packet
//      packet data size
//      packet data
// ret:
//      -1 if send() error
//      else size of data sent(nDataSize + sizeof(Packet))
int RNSendData(int DestinationSocket, unsigned int nPacketID, unsigned int nDataSize, void* Data);


// in:
//      in-the socket to recv from
//      out-pRecvPacket - the packet we are requesting
//      out-nGetError - socket error code
// ret:
//       0 if the socket was closed cleanly
//      -1 if socket error
//          nGetError will hold the socket error code
//      -2 if unable to allocate data for the packet
//          nGetError will hold the size of the data we tried to allocate
//      >0 is the packet size retrieved(NOT the size of the packet data)
// NOTE: Dont get the errno if this function fails, it will be returned
int RNRecieveData(int RecvSocket, Packet** pRecvPacket, int* nGetErrno);


#ifdef __cplusplus
}
#endif


#endif // _RETRONETWORK_H_
