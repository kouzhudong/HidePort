// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "Communication.h"


void banner()
{
    printf("made by correy\r\n");
    printf("https://correy.webs.com\r\n");
    printf("\r\n");
}


VOID Usage(TCHAR * exe)
/*++
Routine Description
    Prints usage
--*/
{
    printf("本程序的用法如下：\r\n");
    printf("\r\n");

    printf("Install Driver：\"%ls\" InstallDriver\r\n", exe);
    printf("Start Driver：\"%ls\" StartDriver\r\n", exe);

    printf("Start Hide：\"%ls\" StartHide\r\n", exe);

    printf("Hide Local Port(Tcp and Udp)：\"%ls\" HidePort Port\r\n", exe);
    printf("Hide Remote IPv4(Tcp Only)：\"%ls\" HideIPv4 IPv4\r\n", exe);
    printf("Hide Remote IPv6(Tcp Only)：\"%ls\" HideIPv6 IPv6\r\n", exe);

    printf("Remove Local Port(Tcp and Udp)：\"%ls\" RemovePort Port\r\n", exe);
    printf("Remove Remote IPv4(Tcp Only)：\"%ls\" RemoveIPv4 IPv4\r\n", exe);
    printf("Remove Remote IPv6(Tcp Only)：\"%ls\" RemoveIPv6 IPv6\r\n", exe);

    printf("Stop Hide：\"%ls\" StopHide\r\n", exe);

    printf("Stop Driver：\"%ls\" StopDriver\r\n", exe);
    printf("UnInstall Driver：\"%ls\" UnInstallDriver\r\n", exe);
}


int _cdecl wmain(_In_ int argc, _In_reads_(argc) TCHAR * argv[])
{
    int ret = ERROR_SUCCESS;

    setlocale(LC_CTYPE, ".936");

    banner();

    switch (argc) {
    case 1:
        Usage(argv[0]);
        break;
    case 2:
        if (lstrcmpi(argv[1], TEXT("InstallDriver")) == 0) {
            //ret = InstallDriver();
        } else if (lstrcmpi(argv[1], TEXT("StartDriver")) == 0) {
            //ret = StartDriver();
        } else if (lstrcmpi(argv[1], TEXT("StartHide")) == 0) {
            ret = StartHide();
        } else if (lstrcmpi(argv[1], TEXT("StopHide")) == 0) {
            ret = StopHide();
        } else if (lstrcmpi(argv[1], TEXT("StopDriver")) == 0) {
            //ret = StopDriver();
        } else if (lstrcmpi(argv[1], TEXT("UnInstallDriver")) == 0) {
            //ret = UnInstallDriver();
        } else {
            Usage(argv[0]);
        }

        break;
    case 3:
    {
        if (lstrcmpi(argv[1], TEXT("HidePort")) == 0) {
            ret = HidePort(argv[2]);
        } else if (lstrcmpi(argv[1], TEXT("HideIPv4")) == 0) {
            ret = HideIPv4(argv[2]);
        } else if (lstrcmpi(argv[1], TEXT("HideIPv6")) == 0) {
            ret = HideIPv6(argv[2]);
        } else if (lstrcmpi(argv[1], TEXT("RemovePort")) == 0) {
            ret = RemovePort(argv[2]);
        } else if (lstrcmpi(argv[1], TEXT("RemoveIPv4")) == 0) {
            ret = RemoveIPv4(argv[2]);
        } else if (lstrcmpi(argv[1], TEXT("RemoveIPv6")) == 0) {
            ret = RemoveIPv6(argv[2]);
        } else {
            Usage(argv[0]);
        }

        break;
    }
    default:
        Usage(argv[0]);
        break;
    }

    return ret;
}
