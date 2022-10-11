#include "Attach.h"


NTSTATUS Detach(_In_ PDRIVER_OBJECT DriverObject)
/*
功能：对本驱动的所有的AttachDevice进行DetachDevice操作。
*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    PDEVICE_OBJECT PreDeviceObject, CurrentDeviceObject;
    PDEVICE_EXTENSION DevExt;

    PAGED_CODE();

    PreDeviceObject = DriverObject->DeviceObject;
    while (PreDeviceObject != NULL) {
        DevExt = (PDEVICE_EXTENSION)PreDeviceObject->DeviceExtension;
        if (DevExt->AttachedDevice) { //确保本驱动创建的设备都包含PFILTER_DEVICE_EXTENSION,没有挂载的把此值设置为0.
            IoDetachDevice(DevExt->AttachedDevice);
        }

        CurrentDeviceObject = PreDeviceObject;
        PreDeviceObject = CurrentDeviceObject->NextDevice;

        if (DevExt->AttachedDevice) {
            IoDeleteDevice(CurrentDeviceObject);
        }
    }

    return Status;
}


NTSTATUS AttachDevice(_In_ PDRIVER_OBJECT DriverObject, 
                      _In_ PCWSTR DeviceNameString, 
                      _In_ PCWSTR FilterDeviceName, 
                      _In_ ULONG Flag
)
/*
功能:创建一个(过滤)设备并挂载到指定的设备上.

参数说明:
DeviceNameString 被挂载的设备的名字.
FilterDeviceName 创建的自己过滤设备.
Flag 创建的过滤设备的标志,在设备对象的扩展结构里面.
*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING DeviceName = {0};
    PDEVICE_OBJECT FilterDeviceObject = NULL;
    UNICODE_STRING ObjectName = {0};
    PFILE_OBJECT FileObject = NULL;
    PDEVICE_OBJECT DeviceObject = NULL;
    PDEVICE_EXTENSION DevExt = NULL;
    PDEVICE_OBJECT  AttachedDevice = NULL;

    RtlInitUnicodeString(&ObjectName, DeviceNameString);
    RtlInitUnicodeString(&DeviceName, FilterDeviceName);

    do {
        //Status = SetFastIoDispatch(&ObjectName);
        //if (Status != STATUS_SUCCESS) {
        //    break;
        //}

        Status = IoGetDeviceObjectPointer(&ObjectName, FILE_ALL_ACCESS, &FileObject, &DeviceObject);
        if (Status != STATUS_SUCCESS) {//INF里必须设置依赖项，否则这几个设备没创建（STATUS_OBJECT_NAME_NOT_FOUND）。
            PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
            break;
        }

        ObDereferenceObject(FileObject);

        Status = IoCreateDevice(DriverObject, 
                                sizeof(DEVICE_EXTENSION), 
                                &DeviceName, 
                                DeviceObject->DeviceType, 
                                0, 
                                FALSE, 
                                &FilterDeviceObject);//可以考虑保存这个值到全局变量。
        if (!NT_SUCCESS(Status)) {
            PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
            break;
        }

        ClearFlag(FilterDeviceObject->Flags, DO_DEVICE_INITIALIZING); //filterDeviceObject->Flags &= ~0x00000080;

        Status = IoAttachDeviceToDeviceStackSafe(FilterDeviceObject, DeviceObject, &AttachedDevice);//返回附加前的顶层设备。
        if (!NT_SUCCESS(Status)) {
            PrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "Error: Status:%#x", Status);
            IoDeleteDevice(FilterDeviceObject);
            break;
        }

        DevExt = (PDEVICE_EXTENSION)FilterDeviceObject->DeviceExtension;
        DevExt->AttachedDevice = AttachedDevice;//可以考虑保存这个值到全局变量，用于DetachDevice。
        DevExt->DeviceTag = Flag;
    } while (FALSE);

    return Status;
}
