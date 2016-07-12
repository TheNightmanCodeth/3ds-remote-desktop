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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "List.h"

typedef struct _PrivateListData_t
{
    char* pListData;
    struct _PrivateListData_t* pNext;
} _PrivateListData;

_PrivateListData* GetEndNode(LinkedList* pList)
{
    _PrivateListData* pCurNode = (_PrivateListData*)pList->pListData;
    _PrivateListData* pPrevNode = NULL;
    while(pCurNode != NULL)
    {
        pPrevNode = pCurNode;
        pCurNode = pCurNode->pNext;
    }
    return pPrevNode;
}

LinkedList* CreateList()
{
    LinkedList* pReturnList = (LinkedList*)malloc(sizeof(LinkedList));
    memset(pReturnList, 0, sizeof(LinkedList));
    return pReturnList;
}

int ListPush(LinkedList* pList, char* pData)
{
    if(pData == NULL)
    {
        return 0;
    }
    _PrivateListData* pNewNode = (_PrivateListData*)malloc(sizeof(_PrivateListData));
    ((_PrivateListData*)pNewNode)->pListData = pData;

    // push it to the back
    _PrivateListData* pEndNode = GetEndNode(pList);
    if(pEndNode == NULL)
    {
        // the list is empty
        (pList)->pListData = pNewNode;
    }
    else
    {
        pEndNode->pNext = pEndNode;
    }


    return 1;
}

char* ListPop(LinkedList* pList)
{
    // invalid or empty list
    if(pList == NULL || (_PrivateListData*)(pList->pListData) == NULL)
    {
        return NULL;
    }
    // save the data to return
    char* pDataReturn = ((_PrivateListData*)(pList->pListData))->pListData;

    // free and reset
    free(pList->pListData);
    pList->pListData = ((_PrivateListData*)(pList->pListData))->pNext;
    return pDataReturn;
}

void DestroyList(LinkedList* pList)
{
    _PrivateListData* pCurNode = (_PrivateListData*) ListPop(pList);
    while(pCurNode != NULL)
    {
        pCurNode = (_PrivateListData*) ListPop(pList);
    }

    free(pList);
}

#undef NULL
