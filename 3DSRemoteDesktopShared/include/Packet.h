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

#ifndef _PACKET_H_
#define _PACKET_H_

#define PACKET_INPUT_PRESSED        0x0
#define PACKET_INPUT_RELEASED       0x1
#define PACKET_SCREEN               0x2
#define PACKET_SCREENRETRIEVED      0x3
#define PACKET_HANDSHAKEREQUEST     0x4
#define PACKET_HANDSHAKERESPONSE    0x5
#define PACKET_MESSAGE              0x6
#define PACKET_INPUT_CIRCLEPAD      0x7

typedef struct
{
    unsigned int _PacketID;
    unsigned int _PacketSize;
    char _PacketData[];
} Packet;

typedef struct
{
  unsigned int _TotalSize; // size when decompressed
  unsigned int _CompressedSize; // size when compressed
  char _ScreenData[];
} ScreenPacketData;

#endif // _PACKET_H_
