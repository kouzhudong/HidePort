#include "Hide.h"

bool g_IsHide;//隐藏的总开关。


//////////////////////////////////////////////////////////////////////////////////////////////////


bool IsHideUdp(_In_ PUdpTable Entry)
/*
功能：识别是不是要隐藏的信息。

后期还可以考虑加参数，如：进程等。
*/
{
    bool ret = false;

    UNREFERENCED_PARAMETER(Entry);


    return ret;
}


bool IsHideTcp(_In_ PTcpTable Entry)
/*
功能：识别是不是要隐藏的信息。

后期还可以考虑加参数，如：进程等。
*/
{
    bool ret = false;

    UNREFERENCED_PARAMETER(Entry);


    return ret;
}


void HideUdpInfo(_In_ PNsiParameters70 NsiParam)
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

    for (ULONG i = 0; i < NsiParam->Counter; i++) {
        if (NsiParam->p1) {
            if (IsHideUdp(Table)) {
                //移动这个元素后面的数据到这个元素的位置。
                memmove(Table, Table + 1, NsiParam->size1 * (NsiParam->Counter - i));
                NsiParam->Counter--;
            }

            Table++;
        }

        if (NsiParam->p2) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }

        if (NsiParam->StateInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }

        if (NsiParam->ProcessInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }
    }

    //更新Counter的值。如果已经更新，这里忽略。
    //NsiParam->Counter = XXX。
}


void HideTcpInfo(_In_ PNsiParameters70 NsiParam)
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

    for (ULONG i = 0; i < NsiParam->Counter; i++) {
        if (NsiParam->p1) {
            if (IsHideTcp(Table)) {
                //移动这个元素后面的数据到这个元素的位置。
                memmove(Table, Table + 1, NsiParam->size1 * (NsiParam->Counter - i));
                NsiParam->Counter--;
            }

            Table++;
        }

        if (NsiParam->p2) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }

        if (NsiParam->StateInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }

        if (NsiParam->ProcessInfo) {
            //如果上面的匹配了，这里也应移除下。
            //这里也可以匹配下。
        }
    }

    //更新Counter的值。如果已经更新，这里忽略。
    //NsiParam->Counter = XXX。
}
