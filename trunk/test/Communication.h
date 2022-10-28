#pragma once

#include "pch.h"
#include "..\common\public.h"


DWORD StartHide();
DWORD StopHide();
DWORD HidePort(PWSTR LocalPortStr);
DWORD RemovePort(PWSTR LocalPortStr);
DWORD HideIPv4(PWSTR IpStr);
DWORD HideIPv6(PWSTR IpStr);
DWORD RemoveIPv4(PWSTR IpStr);
DWORD RemoveIPv6(PWSTR IpStr);


class Communication
{

};
