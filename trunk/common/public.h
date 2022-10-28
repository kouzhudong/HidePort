#pragma once


#define DOS_DEVICE_NAME       L"\\DosDevices\\HideNsiInfo" 
#define DEVICE_NAME           L"\\Device\\HideNsiInfo" 
#define HIDE_NSI_INFO         L"\\\\.\\HideNsiInfo"


typedef struct _HIDE_SWITCH {
    bool IsHide; 
} HIDE_SWITCH, * PHIDE_SWITCH;


typedef struct _LOCAL_PORT {
    bool IsRemove;
    WORD LocalPort;//±¾µØÐò£¬·ÇÍøÂçÐò¡£
} LOCAL_PORT, * PLOCAL_PORT;


typedef struct _REMOTE_IP
{
    bool IsRemove;
    bool IsIPv6;
    union
    {
        IN_ADDR IPv4;//ÍøÂçÐò
        IN6_ADDR IPv6;//ÍøÂçÐò
    }IP;
} REMOTE_IP, * PREMOTE_IP;


#define IOCTL_HIDE                      CTL_CODE(90000, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_LOCALPORT             CTL_CODE(90000, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_REMOTEIP              CTL_CODE(90000, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
