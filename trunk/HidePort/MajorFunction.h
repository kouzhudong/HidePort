#pragma once

#include "Pch.h"
#include "Attach.h"
#include "Nsi.h"

_Function_class_(DRIVER_DISPATCH)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
NTSTATUS GlobalMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ struct _IRP * Irp);

class MajorFunction
{

};

