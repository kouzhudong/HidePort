#include "Cdo.h"
#include "Attach.h"


UNICODE_STRING g_SymbolicLinkName = RTL_CONSTANT_STRING(DOS_DEVICE_NAME);
UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);

PDEVICE_OBJECT g_DeviceObject;//  一个常用的，普通的，控制设备对象。


//////////////////////////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////////////////////////////


_Function_class_(DRIVER_DISPATCH)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
NTSTATUS ControlDeviceObjectMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ struct _IRP * Irp)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

    UNREFERENCED_PARAMETER(DeviceObject);

    switch (IrpStack->MajorFunction) {
    case IRP_MJ_CREATE:

        break;
    default:
        PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_WARNING_LEVEL, "Warning: MajorFunction: %d, IrpName: %s",
                IrpStack->MajorFunction, FltGetIrpName(IrpStack->MajorFunction));
        break;
    }

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}


NTSTATUS DeleteControlDeviceObject()
{
    NTSTATUS Status = STATUS_SUCCESS;    

    Status = IoDeleteSymbolicLink(&g_SymbolicLinkName);
    if (!NT_SUCCESS(Status)) {
        PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
    }

    IoDeleteDevice(g_DeviceObject);

    return Status;
}


NTSTATUS CreateControlDeviceObject(_In_ PDRIVER_OBJECT DriverObject)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PDEVICE_EXTENSION DevExt = NULL;    

    Status = IoCreateDevice(DriverObject,
                            sizeof(DEVICE_EXTENSION),
                            &g_DeviceName,
                            FILE_DEVICE_UNKNOWN,
                            FILE_DEVICE_SECURE_OPEN,
                            FALSE,
                            &g_DeviceObject);
    if (!NT_SUCCESS(Status)) {
        PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
        return Status;
    }

    DevExt = (PDEVICE_EXTENSION)g_DeviceObject->DeviceExtension;
    DevExt->DeviceTag = MY_CDO_DEVICE_TAG;//用于通讯的控制设备的标志。用于区别其他的AttachDevice。
    DevExt->AttachedDevice = NULL;//本设备不用于Attach。

    Status = IoCreateSymbolicLink(&g_SymbolicLinkName, &g_DeviceName);
    if (!NT_SUCCESS(Status)) {
        PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
        IoDeleteDevice(g_DeviceObject);
        return Status;
    }

    return Status;
}
