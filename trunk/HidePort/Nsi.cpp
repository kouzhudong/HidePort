#include "Nsi.h"


//http://safe.sh/security/?type=detail&id=1381
NPI_MODULEID NPI_MS_TCP_MODULEID = {
    sizeof(NPI_MODULEID),
    MIT_GUID,
    {0xEB004A03, 0x9B1A, 0x11D4,
    {0x91, 0x23, 0x00, 0x50, 0x04, 0x77, 0x59, 0xBC}}
};


//http://safe.sh/security/?type=detail&id=1381
NPI_MODULEID NPI_MS_UDP_MODULEID = {
    sizeof(NPI_MODULEID),
    MIT_GUID,
    {0xEB004A02, 0x9B1A, 0x11D4,
    {0x91, 0x23, 0x00, 0x50, 0x04, 0x77, 0x59, 0xBC}}
};


//http://safe.sh/security/?type=detail&id=1381
NPI_MODULEID NPI_MS_RAW_MODULEID = {
    sizeof(NPI_MODULEID),
    MIT_GUID,
    {0xEB004A07, 0x9B1A, 0x11D4,
    {0x91, 0x23, 0x00, 0x50, 0x04, 0x77, 0x59, 0xBC}}
};


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS DefaultMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp)
{
    PDEVICE_EXTENSION DevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    NTSTATUS Status = STATUS_SUCCESS;
    //PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    //UCHAR MajorFunction = IrpStack->MajorFunction;

    if (Irp->CurrentLocation <= 1) {
        IoSkipCurrentIrpStackLocation(Irp);
    } else {
        IoCopyCurrentIrpStackLocationToNext(Irp);
    }

    Status = IoCallDriver(DevExt->AttachedDevice, Irp);//这个函数之后禁止访问IrpStack等信息。
    if (!NT_SUCCESS(Status)) {//这里失败是很正常的。
        //PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Warning: IrpName: %s, Status:%#x",
        //        FltGetIrpName(MajorFunction), Status);
    }

    return Status;
}


void EnumUdpTable(_In_ PNsiParameters70 NsiParam)
{
    UNREFERENCED_PARAMETER(NsiParam);

}


void EnumTcpTable(_In_ PNsiParameters70 NsiParam)
{
    if (NsiParam->p1) {//这个是啥结构呢？可以分析GetTcp6Table2。
    //ASSERT(NsiParam->size1 == 0x38);//可以肯定这个结构的大小是0x38。

    /*
    这个结构里包含：
    LocalAddr
    dwLocalScopeId
    dwLocalPort
    RemoteAddr
    dwRemoteScopeId
    dwRemotePort
    等。
    */

        PTcpTable Table = (PTcpTable)NsiParam->p1;

        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "dwNumEntries: %d", NsiParam->Counter);

        for (ULONG i = 0; i < NsiParam->Counter; i++, Table++) {
            switch (Table->LocalFamily) {
            case AF_INET:
            {
                PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "LocalPort: %d, RemotePort:%d",
                        RtlUshortByteSwap(Table->LocalPort), RtlUshortByteSwap(Table->RemotePort));
                break;
            }
            case AF_INET6:
            {
                PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "LocalPort: %d, RemotePort:%d",
                        RtlUshortByteSwap(Table->LocalPort), RtlUshortByteSwap(Table->RemotePort));
                break;
            }
            default:
                PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "LocalFamily: %d", Table->LocalFamily);
                break;
            }
        }
    }

    if (NsiParam->p2) {//这个是啥结构呢？可以分析GetTcp6Table2。
        /*
        这个结构的指针大多为NULL。
        */

        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "size2: %d", NsiParam->size2);
    }

    if (NsiParam->p3) {//这个是啥结构呢？可以分析GetTcp6Table2。
        /*
        经测试，这个结构的大小是0x10.

        这个结构里包含：
        State
        dwOffloadState

        */

        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "size3: %d", NsiParam->size3);
    }

    if (NsiParam->p4) {//这个是啥结构呢？可以分析GetTcp6Table2。
        /*
        经测试，这个结构的大小是0x20.

        这个结构里包含：
        dwOwningPid

        */

        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "size4: %d", NsiParam->size4);
    }
}


NTSTATUS MyNsippEnumerateObjectsAllParameters(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp)
//NTSTATUS MyNsippEnumerateObjectsAllParameters(PVOID InputBuffer,
//                                              SIZE_T Length,
//                                              KPROCESSOR_MODE RequestorMode,
//                                              PULONG_PTR Information
//)
/*
功能：隐藏端口。

做法：摘除需要指定摘除的信息。

重点：InputBuffer的结构类型。

篡改时机：IoCallDriver之后。

InputBufferLength：不小于0x3C，也不小于0x70。经观察都是0x70。

参考：https://github.com/claudiouzelac/rootkit.com/blob/c8869de5a947273c9c151b44aa39643a7fea531c/cardmagic/PortHidDemo_Vista.c
*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    PVOID Type3InputBuffer = IrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
    KPROCESSOR_MODE RequestorMode = Irp->RequestorMode;
    ULONG InputBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outBufLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    DBG_UNREFERENCED_LOCAL_VARIABLE(InputBufferLength);
    DBG_UNREFERENCED_LOCAL_VARIABLE(outBufLength);
    DBG_UNREFERENCED_LOCAL_VARIABLE(RequestorMode);

    Status = DefaultMajorFunction(DeviceObject, Irp);
    if (!NT_SUCCESS(Status)) {
        return Status;//大多是STATUS_MORE_ENTRIES，而不是挂起状态：STATUS_PENDING。
    }

    //if (STATUS_SUCCESS != Status) {
    //    return Status;
    //}

    __try {
        PNsiParameters70 NsiParam = (PNsiParameters70)Type3InputBuffer;
        PNPI_MODULEID ModuleId = NsiParam->ModuleId;

        if (NmrIsEqualNpiModuleId(ModuleId, &NPI_MS_TCP_MODULEID)) {
            EnumTcpTable(NsiParam);
        }

        if (NmrIsEqualNpiModuleId(ModuleId, &NPI_MS_UDP_MODULEID)) {
            EnumUdpTable(NsiParam);
        }


    } __except (EXCEPTION_EXECUTE_HANDLER) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "ExceptionCode:%#X", GetExceptionCode());
    }

    return Status;
}


NTSTATUS NsiDeviceControl(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp)
/*
做法参考：nsiproxy!NsippDispatch.

还有个函数叫：NsippDispatchDeviceControl。
*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  IrpStack = IoGetCurrentIrpStackLocation(Irp);// Pointer to current stack location   
    ULONG IoControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;

    switch (IoControlCode) {
    case 0x12001Bu://NsippEnumerateObjectsAllParameters
        //MyNsippEnumerateObjectsAllParameters(Type3InputBuffer,
        //                                     (SIZE_T)InputBufferLength,
        //                                     RequestorMode,
        //                                     &Irp->IoStatus.Information);
        Status = MyNsippEnumerateObjectsAllParameters(DeviceObject, Irp);
        break;
    case 0x120007u://NsippGetParameter
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12000Fu://NsippGetAllParameters
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12000Bu://NsippSetParameter
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x120013u://NsippSetAllParameters
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12001Fu://NsippIsCallerNsiService + NsippRegisterChangeNotification
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x120023u://NsippIsCallerNsiService + NsippDeregisterChangeNotification
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12003Fu://NsippRequestChangeNotification
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x120040u://NsippCancelChangeNotification
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x120047u://NsippEnumerateObjectsAllPersistentParametersWithMask
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12004Bu://NsippGetAllPersistentParametersWithMask
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case 0x12004Fu://NsippSetAllPersistentParametersWithMask
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    default:
        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL,
                "Warning: MajorFunction: %d, IrpName: %s, IoControlCode:%d",
                IrpStack->MajorFunction, FltGetIrpName(IrpStack->MajorFunction), IoControlCode);
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    }

    return Status;
}


NTSTATUS NsiMajorFunction(_In_ struct _DEVICE_OBJECT * DeviceObject, _Inout_ PIRP Irp)
/*
做法参考：nsiproxy!NsippDispatch.
*/
{
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS Status = STATUS_SUCCESS;

    switch (IrpStack->MajorFunction) {
    case IRP_MJ_CREATE:
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case IRP_MJ_DEVICE_CONTROL:
        Status = NsiDeviceControl(DeviceObject, Irp);
        break;
    case IRP_MJ_INTERNAL_DEVICE_CONTROL:
        Status = NsiDeviceControl(DeviceObject, Irp);
        break;
    case IRP_MJ_CLEANUP:
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    case IRP_MJ_CLOSE:
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    default:
        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "Warning: MajorFunction: %d, IrpName: %s",
                IrpStack->MajorFunction, FltGetIrpName(IrpStack->MajorFunction));
        Status = DefaultMajorFunction(DeviceObject, Irp);
        break;
    }

    //if (STATUS_ACCESS_DENIED == Status) {//阻断响应处理。        
    //    Irp->IoStatus.Status = Status;
    //    Irp->IoStatus.Information = 0;
    //    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    //    return Status;
    //}

    return Status;
}
