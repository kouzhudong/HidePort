#pragma once

#include "Pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct _HIDE_LOCAL_PORT {
    WORD LocalPort;//本地序，非网络序。
} HIDE_LOCAL_PORT, * PHIDE_LOCAL_PORT;


#define MAX_LOCAL_PORTS 99 //隐藏端口支持的个数。


//////////////////////////////////////////////////////////////////////////////////////////////////


void InitializeGenericTable();
bool InsertElementGenericTable(WORD LocalPort);
bool IsHideLocalPort(WORD LocalPort);
bool DeleteGenericTableElement(WORD LocalPort);
void EnumerateGenericTable();
void DeleteGenericTable();


//////////////////////////////////////////////////////////////////////////////////////////////////


class Rules
{

};
