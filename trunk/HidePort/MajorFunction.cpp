#include "MajorFunction.h"


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
        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "Warning: MajorFunction: %d, IrpName: %s", 
                IrpStack->MajorFunction, FltGetIrpName(IrpStack->MajorFunction));
        break;
    }

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}


_Function_class_(DRIVER_DISPATCH)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
NTSTATUS GlobalMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ struct _IRP * Irp)
{
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    PDEVICE_EXTENSION DevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    NTSTATUS Status = STATUS_SUCCESS;

    PAGED_CODE();

    switch (DevExt->DeviceTag) {
    case MY_NSI_DEVICE_TAG:
        Status = NsiMajorFunction(IrpStack, Irp);
        break;
    case MY_CDO_DEVICE_TAG:
        return ControlDeviceObjectMajorFunction(DeviceObject, Irp);//直接返回，不能走下面。
        break;
    default:
        ASSERTMSG("非法的设备操作", FALSE);
        break;
    }

    //阻断响应处理。
    if (STATUS_ACCESS_DENIED == Status) {        
        Irp->IoStatus.Status = Status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return Status;
    }

    //放过继续。
    if (Irp->CurrentLocation <= 1) {
        IoSkipCurrentIrpStackLocation(Irp);
    } else {
        IoCopyCurrentIrpStackLocationToNext(Irp);
    }
    Status = IoCallDriver(DevExt->AttachedDevice, Irp);
    if (!NT_SUCCESS(Status)) {//这里失败是很正常的。
        //PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Warning: Status:%#x", Status);
    }

    return Status;
}
