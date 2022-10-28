#include "Cdo.h"
#include "Attach.h"
#include "Hide.h"
#include "Rules.h"


UNICODE_STRING g_SymbolicLinkName = RTL_CONSTANT_STRING(DOS_DEVICE_NAME);
UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);

PDEVICE_OBJECT g_DeviceObject;//  一个常用的，普通的，控制设备对象。


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS SetGlobalSwitch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    //ULONG outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;  
    PHIDE_SWITCH inBuf = (PHIDE_SWITCH)Irp->AssociatedIrp.SystemBuffer;
    //PCHAR outBuf = (PCHAR)Irp->AssociatedIrp.SystemBuffer;

    UNREFERENCED_PARAMETER(DeviceObject);

    if (inBufLength != sizeof(HIDE_SWITCH)) {

        return STATUS_UNSUCCESSFUL;
    }

    if (nullptr == inBuf) {

        return STATUS_UNSUCCESSFUL;
    }

    g_IsHide = inBuf->IsHide;//建议原子操作加异常处理。

    return Status;
}


NTSTATUS SetLocalPort(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    //NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    //ULONG outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;  
    PLOCAL_PORT inBuf = (PLOCAL_PORT)Irp->AssociatedIrp.SystemBuffer;
    //PCHAR outBuf = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
    bool IsOk = false;

    UNREFERENCED_PARAMETER(DeviceObject);

    if (inBufLength != sizeof(HIDE_SWITCH)) {

        return STATUS_UNSUCCESSFUL;
    }

    if (nullptr == inBuf) {

        return STATUS_UNSUCCESSFUL;
    }

    if (inBuf->IsRemove) {
        IsOk = DeleteGenericTableElement(inBuf->LocalPort);
    } else {
        IsOk = InsertElementGenericTable(inBuf->LocalPort);
    }

    return IsOk ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}


NTSTATUS SetRemoteIp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    //ULONG outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;  
    PLOCAL_PORT inBuf = (PLOCAL_PORT)Irp->AssociatedIrp.SystemBuffer;
    //PCHAR outBuf = (PCHAR)Irp->AssociatedIrp.SystemBuffer;

    UNREFERENCED_PARAMETER(DeviceObject);

    if (inBufLength != sizeof(HIDE_SWITCH)) {

        return STATUS_UNSUCCESSFUL;
    }

    if (nullptr == inBuf) {

        return STATUS_UNSUCCESSFUL;
    }

    if (inBuf->IsRemove) {

    } else {

    }

    return Status;
}


NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_HIDE:
        Status = SetGlobalSwitch(DeviceObject, Irp);
        break;
    case IOCTL_SET_LOCALPORT:
        Status = SetLocalPort(DeviceObject, Irp);
        break;
    case IOCTL_SET_REMOTEIP:
        Status = SetRemoteIp(DeviceObject, Irp);
        break;
    default:
        break;
    }

    return Status;
}


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
    case IRP_MJ_CLEANUP:
        break;
    case IRP_MJ_CLOSE:
        break;
    case IRP_MJ_DEVICE_CONTROL:
        Status = DeviceControl(DeviceObject, Irp);
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
