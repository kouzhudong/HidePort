#include "Communication.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
//基础函数。


DWORD DeviceIoControlCode(_In_ DWORD dwIoControlCode,
                          _In_reads_bytes_opt_(nInBufferSize) LPVOID lpInBuffer,
                          _In_ DWORD nInBufferSize,
                          _Out_writes_bytes_opt_(nOutBufferSize) LPVOID lpOutBuffer,
                          _In_ DWORD nOutBufferSize
)
/*

*/
{
    HANDLE HDevice = CreateFileW(HIDE_NSI_INFO,
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 NULL,
                                 OPEN_EXISTING,
                                 0,
                                 NULL);
    if (HDevice == INVALID_HANDLE_VALUE) {

        return GetLastError();
    }

    DWORD bytesReturned;
    BOOL ret = DeviceIoControl(HDevice,
                               dwIoControlCode,
                               lpInBuffer,
                               nInBufferSize,
                               lpOutBuffer,
                               nOutBufferSize,
                               &bytesReturned,
                               NULL);
    DWORD LastError = GetLastError();
    if (!ret) {

    }

    CloseHandle(HDevice);

    return LastError;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//二次封装的函数。


DWORD HideSwitch(bool Open)
{
    DWORD LastError = ERROR_SUCCESS;

    HIDE_SWITCH Temp{};
    Temp.IsHide = Open;

    LastError = DeviceIoControlCode(IOCTL_HIDE, &Temp, sizeof(HIDE_SWITCH), nullptr, 0);

    return LastError;
}


DWORD SetLocalPort(bool IsRemove, PWSTR LocalPortStr)
{
    DWORD LastError = ERROR_SUCCESS;
    int LocalPort = _wtoi(LocalPortStr);

    if (LocalPort <= 0) {
        return ERROR_INVALID_PARAMETER;
    }

    if (LocalPort > MAXWORD) {
        return ERROR_INVALID_PARAMETER;
    }

    LOCAL_PORT Temp{};
    Temp.IsRemove = IsRemove;
    Temp.LocalPort = (WORD)LocalPort;

    LastError = DeviceIoControlCode(IOCTL_HIDE, &Temp, sizeof(LOCAL_PORT), nullptr, 0);

    return LastError;
}


DWORD SetRemoteIp(bool IsRemove, bool IsIPv6, PWSTR IpStr)
{
    DWORD LastError = ERROR_SUCCESS;

    REMOTE_IP Temp{};
    Temp.IsRemove = IsRemove;
    Temp.IsIPv6 = IsIPv6;

    IN_ADDR IPv4{};
    IN6_ADDR IPv6{};

    if (IsIPv6) {
        if (InetPton(AF_INET6, IpStr, &IPv6) != 1) {
            return WSAGetLastError();
        }
        RtlCopyMemory(&Temp.IP.IPv6, &IPv6, sizeof(IN6_ADDR));
    } else {
        if (InetPton(AF_INET, IpStr, &IPv4) != 1) {
            return WSAGetLastError();
        }
        Temp.IP.IPv4.S_un.S_addr = IPv4.S_un.S_addr;
    }

    LastError = DeviceIoControlCode(IOCTL_HIDE, &Temp, sizeof(REMOTE_IP), nullptr, 0);

    return LastError;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//导出的公共的函数。


DWORD StartHide()
{
    return HideSwitch(true);
}


DWORD StopHide()
{
    return HideSwitch(false);
}


DWORD HidePort(PWSTR LocalPortStr)
{
    return SetLocalPort(false, LocalPortStr);
}


DWORD RemovePort(PWSTR LocalPortStr)
{
    return SetLocalPort(true, LocalPortStr);
}


DWORD HideIPv4(PWSTR IpStr)
{
    return SetRemoteIp(false, false, IpStr);
}


DWORD HideIPv6(PWSTR IpStr)
{
    return SetRemoteIp(false, true, IpStr);
}


DWORD RemoveIPv4(PWSTR IpStr)
{
    return SetRemoteIp(true, false, IpStr);
}


DWORD RemoveIPv6(PWSTR IpStr)
{
    return SetRemoteIp(true, true, IpStr);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
