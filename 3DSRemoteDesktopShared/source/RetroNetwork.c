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

#include "RetroNetwork.h"

#ifdef _3DS
#include "ExtendedConsole.h"
#endif // _3DS

int RNSendData(int DestinationSocket, unsigned int nPacketID, unsigned int nDataSize, void* Data)
{
    // create the packet to send
    int nPacketLength = sizeof(Packet) + (nDataSize);
    Packet* packet = (Packet*)malloc(nPacketLength);
    packet->_PacketID = nPacketID;
    packet->_PacketSize = nDataSize;
    memcpy(packet->_PacketData, Data, nDataSize);

    int nTotalSent = 0;
    int nSendAmmount = 0;
    do
    {
        nSendAmmount = send(DestinationSocket, (char*)packet + nTotalSent, nPacketLength, 0);
        if(nSendAmmount < 0)
        {
            return -1;
        }
        nTotalSent += nSendAmmount;
    } while(nTotalSent < nPacketLength);

    free(packet);

    return nTotalSent;
}

int RNRecieveData(int RecvSocket, Packet** pRecvPacket, int* nGetErrno)
{
    int nPacketSize = sizeof(Packet);
    Packet tempPacket;
    int ret = recv(RecvSocket, (char*)&tempPacket, nPacketSize, 0);
    if(ret > 0)
    {
        // determine hte full size of the data coming in
        int nTotalSize = tempPacket._PacketSize + nPacketSize;
        *pRecvPacket = (Packet*)malloc(nTotalSize);
        if(*pRecvPacket == NULL)
        {
            if(nGetErrno)
            {
                *nGetErrno = nTotalSize;
            }
            return -2;
        }

        // put the temp into the final
        memcpy(*pRecvPacket, (char*)&tempPacket, nPacketSize);

        // recieve everything if we need to
        int nTotalRetrieved = nPacketSize;
        if(tempPacket._PacketSize > 0)
        {
            do
            {
                /*
                #ifdef _3DS
                PrintToScreen(GFX_TOP, 5, 5, "%d/%d = %d%%", nTotalRetrieved, nTotalSize, (int)(((float)nTotalRetrieved / (float)nTotalSize) * 100.0f));
                printf("%d ", nTotalRetrieved);
                #endif // _3DS
*/
                //printf("\n0.");
                //printf("1.");
                ret = recv(RecvSocket, (char*)*pRecvPacket + nTotalRetrieved, nTotalSize - nTotalRetrieved, 0 );
                //printf("2.");
                if(ret > 0)
                {
                //printf("3.");
                    nTotalRetrieved += ret;
                //printf("4.");
                }
                else
                {
                //printf("5.");
                    // if not EWOULDBLOCK, leave while setting the error
                    unsigned int error = GETERRNO();
                //printf("6(%d).", error);
                    #ifdef _3DS
                    if(-error != EWOULDBLOCK && -error != EAGAIN)
                    #else
                    if(error != WSAEWOULDBLOCK)
                    #endif // _3DS
                    {
                //printf("7.");
                        if(nGetErrno)
                        {
                //printf("8.");
                            free(*pRecvPacket);
                //printf("9.");
                            *pRecvPacket = NULL;
                //printf("10.");
                            *nGetErrno = error;
                //printf("11.");
                            return -1;
                        }
                //printf("12.");
                    }
                //printf("13.");
                }
                //printf("14.");
/*
                #ifdef _3DS
                PrintToScreen(GFX_TOP, 5, 5, "%d/%d = %d%%", nTotalRetrieved, nTotalSize, (int)(((float)nTotalRetrieved / (float)nTotalSize) * 100.0f));
                printf("%d \n", ret);
                #endif // _3DS
                */
            }while(nTotalRetrieved < nTotalSize);
        }
        ret = nTotalRetrieved;
    }
    else
    {
        if(ret < 0)
        {
            if(nGetErrno)
            {
                *nGetErrno = GETERRNO();
                return -1;
            }
        }
        return -1;
    }
    return ret;
}
