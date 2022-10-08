#include "Pch.h"


_IRQL_requires_min_(PASSIVE_LEVEL)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
_Success_(return == STATUS_SUCCESS)
NTSTATUS Sleep(_In_ UINT32 numMS)
/**
Purpose:  Delay the thread for the specified time.
MSDN_Ref: HTTP://MSDN.Microsoft.com/En-US/Library/FF551986.aspx
*/
{
    ASSERT(numMS);

    // (numMS[milli] * (-1[relative] * 1000[milli to micro] * 1000[micro to nano]) / 100[ns]
    INT64 interval = numMS * -10000i64;

    return KeDelayExecutionThread(KernelMode, FALSE, (PLARGE_INTEGER)&interval);
}
