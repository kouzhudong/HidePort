#include "DriverEntry.h"
#include "Attach.h"
#include "MajorFunction.h"
#include "Cdo.h"
#include "GetInfo.h"
#include "Rules.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


_Function_class_(DRIVER_UNLOAD)
_IRQL_requires_(PASSIVE_LEVEL)
_IRQL_requires_same_
VOID DriverUnload(_In_ struct _DRIVER_OBJECT * DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    DeleteControlDeviceObject();

    Detach(DriverObject);

    DeleteGenericTable();

    Sleep(3000);//等待一些挂起状态的IRP。注意：不是全部。
}


EXTERN_C DRIVER_INITIALIZE DriverEntry;
//#pragma INITCODE
//#pragma alloc_text(INIT, DriverEntry)
_Function_class_(DRIVER_INITIALIZE)
_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    NTSTATUS Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(RegistryPath);

    if (!KD_DEBUGGER_NOT_PRESENT) {
        KdBreakPoint();//__debugbreak();
    }

    PAGED_CODE();

    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    DriverObject->DriverUnload = DriverUnload;

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = GlobalMajorFunction;
    }

    //DriverObject->FastIoDispatch = &g_FastIoDispatch;

    InitializeGenericTable();

    __try {
        Status = CreateControlDeviceObject(DriverObject);
        if (!NT_SUCCESS(Status)) {
            __leave;
        }

        EnumTcpTable();
        EnumUdpTable();

        Status = AttachDevice(DriverObject, L"\\Device\\Nsi", L"\\Device\\MyNsi", MY_NSI_DEVICE_TAG);
        if (!NT_SUCCESS(Status)) {
            __leave;
        }

        EnumTcpTable();
        EnumUdpTable();
    } __finally {
        if (!NT_SUCCESS(Status)) {
            DriverUnload(DriverObject);
        }
    }

    return Status;
}
