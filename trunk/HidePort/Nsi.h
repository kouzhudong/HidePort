#pragma once

#include "Pch.h"
#include "Attach.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct _TcpTable
{
    //这四个成员应该是SOCKADDR。
    WORD LocalFamily;
    WORD LocalPort;
    IN_ADDR LocalAddrV4;
    IN6_ADDR LocalAddrV6;

    DWORD LocalScopeId;

    WORD RemoteFamily;//这个是猜测的，经观察和LocalFamily一样。
    WORD RemotePort;
    IN_ADDR RemoteAddrV4;
    IN6_ADDR RemoteAddrV6;

    DWORD RemoteScopeId;
}TcpTable, * PTcpTable;

static_assert(sizeof(TcpTable) == 0x38);


typedef struct _NsiParameters70
{
    GUID Guid;
    PNPI_MODULEID ModuleId;
    SIZE_T field_18;
    SIZE_T Flag;

    PTcpTable p1;
    SIZE_T size1;

    PVOID p2;
    SIZE_T size2;

    PVOID p3;
    SIZE_T size3;

    PVOID p4;
    ULONG size4;

    ULONG field_64;
    ULONG Counter;
    ULONG field_6C;
}NsiParameters70, * PNsiParameters70;

static_assert(sizeof(NsiParameters70) == 0x70); //语言功能 "简要静态断言" 需要编译器标志 "/std:c++17"


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS NsiMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp);


//////////////////////////////////////////////////////////////////////////////////////////////////


class Nsi
{

};

