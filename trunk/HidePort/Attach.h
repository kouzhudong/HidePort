#pragma once

#include "Pch.h"

#define MY_CDO_DEVICE_TAG   'CTO' //自己的控制设备的扩展里的标志。
#define MY_NSI_DEVICE_TAG   'NSI' //自己的过滤NSI的设备的扩展里标志。


typedef struct _DEVICE_EXTENSION
{
    ULONG DeviceTag; //可以填写的值有NPFS,MSFS,CD0
    PDEVICE_OBJECT AttachedDevice;
    IO_REMOVE_LOCK RemoveLock;//经测试验证：无用的安全卸载机制。
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS Detach(_In_ PDRIVER_OBJECT DriverObject);

NTSTATUS AttachDevice(_In_ PDRIVER_OBJECT DriverObject,
                      _In_ PCWSTR DeviceNameString,
                      _In_ PCWSTR FilterDeviceName,
                      _In_ ULONG Flag
);


//////////////////////////////////////////////////////////////////////////////////////////////////


class Attach
{

};

