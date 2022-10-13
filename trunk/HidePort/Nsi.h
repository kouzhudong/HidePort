#pragma once

#include "Pch.h"
#include "Attach.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma pack(1)
typedef struct _StateTable //Tcp特有，Udp没有。
{
    MIB_TCP_STATE State;
    int field_4;
    TCP_CONNECTION_OFFLOAD_STATE dwOffloadState;
    int field_C;
}StateTable, * PStateTable;
#pragma pack()

static_assert(sizeof(StateTable) == 0x10);


typedef __declspec(align(32)) struct _ProcessTable
//这里应该还有路径信息。
{
    SIZE_T field_0;
    int field_8;
    int dwOwningPid;
    SIZE_T field_10;
    SIZE_T field_18;
}ProcessTable, * PProcessTable;

static_assert(sizeof(ProcessTable) == 0x20);


#pragma pack(1)
typedef struct _UdpTable
{
    //这四个成员应该是SOCKADDR。
    WORD LocalFamily;
    WORD LocalPort;
    IN_ADDR LocalAddrV4;
    IN6_ADDR LocalAddrV6;

    DWORD LocalScopeId;
}UdpTable, * PUdpTable;
#pragma pack()

static_assert(sizeof(UdpTable) == 0x1C);


#pragma pack(1)
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
#pragma pack()

static_assert(sizeof(TcpTable) == 0x38);


#pragma pack(1)
typedef struct _NsiParameters //__declspec(align(16))
{
    SIZE_T Unknown1;
    SIZE_T Unknown2;

    PNPI_MODULEID ModuleId;

    SIZE_T Flag1;
    LARGE_INTEGER Flag2;

    PVOID p1;
    SIZE_T size1;//不是内存的大小，应该是数组的元素的大小。

    PVOID p2;
    SIZE_T size2;//不是内存的大小，应该是数组的元素的大小。

    PVOID StateInfo;
    SIZE_T size3;//不是内存的大小，应该是数组的元素的大小。

    PVOID ProcessInfo;
    SIZE_T size4;//不是内存的大小，应该是数组的元素的大小。

    SIZE_T Counter;
}NsiParameters, * PNsiParameters;
#pragma pack()


#if defined(_WIN64)
static_assert(sizeof(NsiParameters) == 0x70);//语言功能 "简要静态断言" 需要编译器标志 "/std:c++17"
#else 
static_assert(sizeof(NsiParameters) == 0x3C);
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////


extern NPI_MODULEID NPI_MS_TCP_MODULEID;
extern NPI_MODULEID NPI_MS_UDP_MODULEID;
extern NPI_MODULEID NPI_MS_RAW_MODULEID;


NTSTATUS NsiMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp);
void EnumUdpTable(_In_ PNsiParameters NsiParam);
void EnumTcpTable(_In_ PNsiParameters NsiParam);


//////////////////////////////////////////////////////////////////////////////////////////////////


class Nsi
{

};
