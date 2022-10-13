#include "GetInfo.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS GetTcpNumbers(_In_ PSIZE_T Counter)
{
    UNICODE_STRING UnicodeString{};
    RtlInitUnicodeString(&UnicodeString, L"\\Device\\Nsi");

    OBJECT_ATTRIBUTES ObjectAttributes{};
    InitializeObjectAttributes(&ObjectAttributes, 
                               &UnicodeString, 
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               NULL, 
                               NULL);

    HANDLE FileHandle = nullptr;
    IO_STATUS_BLOCK IoStatus{};
    NTSTATUS Status = ZwCreateFile(&FileHandle,
                                   FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE,
                                   &ObjectAttributes,
                                   &IoStatus,
                                   (PLARGE_INTEGER)NULL,
                                   0L,
                                   0L,
                                   FILE_OPEN_IF,
                                   FILE_SYNCHRONOUS_IO_ALERT,
                                   (PVOID)NULL,
                                   0L);
    if (Status != STATUS_SUCCESS) {

        return Status;
    }

    if (IoStatus.Status != STATUS_SUCCESS) {
        return Status;
    }

    if (IoStatus.Information != 0L) {
        return Status;
    }

    NsiParameters NsiParam{};
    NsiParam.ModuleId = &NPI_MS_TCP_MODULEID;
    NsiParam.Flag1 = 3;
    NsiParam.Flag2.QuadPart = 1 | 0x100000000i64;

    Status = ZwDeviceIoControlFile(FileHandle,
                                   (HANDLE)NULL,
                                   (PIO_APC_ROUTINE)NULL,
                                   (PVOID)NULL,
                                   &IoStatus,
                                   0x12001Bu,
                                   &NsiParam,
                                   sizeof(NsiParameters),
                                   &NsiParam,
                                   sizeof(NsiParameters));
    if (Status != STATUS_SUCCESS) {

    } else if (IoStatus.Status != STATUS_SUCCESS) {

    }

    if (IoStatus.Information != 0) {

    }

    if (Counter) {
        *Counter = NsiParam.Counter;
    }

    Status = ZwClose(FileHandle);
    if (Status != STATUS_SUCCESS) {

        return Status;
    }

    return Status;
}


NTSTATUS GetTcpInfoByApi(_In_ PNsiParameters NsiParam)
{
    UNICODE_STRING UnicodeString{};
    RtlInitUnicodeString(&UnicodeString, L"\\Device\\Nsi");

    OBJECT_ATTRIBUTES ObjectAttributes{};
    InitializeObjectAttributes(&ObjectAttributes,
                               &UnicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL);

    HANDLE FileHandle = nullptr;
    IO_STATUS_BLOCK IoStatus{};
    NTSTATUS Status = ZwCreateFile(&FileHandle,
                                   FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE,
                                   &ObjectAttributes,
                                   &IoStatus,
                                   (PLARGE_INTEGER)NULL,
                                   0L,
                                   0L,
                                   FILE_OPEN_IF,
                                   FILE_SYNCHRONOUS_IO_ALERT,
                                   (PVOID)NULL,
                                   0L);
    if (Status != STATUS_SUCCESS) {

        return Status;
    }

    if (IoStatus.Status != STATUS_SUCCESS) {
        return Status;
    }

    if (IoStatus.Information != 0L) {
        return Status;
    }

    Status = ZwDeviceIoControlFile(FileHandle,
                                   (HANDLE)NULL,
                                   (PIO_APC_ROUTINE)NULL,
                                   (PVOID)NULL,
                                   &IoStatus,
                                   0x12001Bu,
                                   &NsiParam,
                                   sizeof(NsiParameters),
                                   &NsiParam,
                                   sizeof(NsiParameters));
    if (Status != STATUS_SUCCESS) {//STATUS_ACCESS_VIOLATION

    } else if (IoStatus.Status != STATUS_SUCCESS) {

    }

    if (IoStatus.Information != 0) {

    }

    Status = ZwClose(FileHandle);
    if (Status != STATUS_SUCCESS) {

        return Status;
    }

    return Status;
}


NTSTATUS GetTcpInfoByIrp(_In_ PNsiParameters NsiParam)
/*

*/
{
    KEVENT EventObject = {};
    KeInitializeEvent(&EventObject, NotificationEvent, FALSE);

    UNICODE_STRING ObjectName;
    RtlInitUnicodeString(&ObjectName, L"\\Device\\Nsi");

    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;
    NTSTATUS Status = IoGetDeviceObjectPointer(&ObjectName, FILE_ALL_ACCESS, &FileObject, &DeviceObject);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    IO_STATUS_BLOCK IoStatusBlock;
    PIRP Irp = IoBuildDeviceIoControlRequest(0x12001Bu,
                                             DeviceObject,
                                             &NsiParam,
                                             sizeof(NsiParameters),
                                             &NsiParam,
                                             sizeof(NsiParameters),
                                             FALSE,
                                             &EventObject,
                                             &IoStatusBlock);
    if (!Irp) {
        ObDereferenceObject(FileObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //PIO_STACK_LOCATION StackLocation = IoGetNextIrpStackLocation(Irp);
    //StackLocation->FileObject = FileObject;
    //StackLocation->DeviceObject = DeviceObject;
    //Irp->RequestorMode = KernelMode;
    
    Status = IoCallDriver(DeviceObject, Irp);//依旧STATUS_ACCESS_VIOLATION。
    if (Status == STATUS_PENDING) {
        KeWaitForSingleObject(&EventObject, Executive, KernelMode, FALSE, NULL);
        Status = IoStatusBlock.Status;
    }
    //ASSERT(NT_SUCCESS(Status));

    ObDereferenceObject(FileObject);

    return Status;
}


void FreeNsiParam(_In_ PNsiParameters NsiParam)
{
    if (!NsiParam) {
        return;
    }

    if (NsiParam->p1) {
        ExFreePoolWithTag(NsiParam->p1, TAG);
        NsiParam->p1 = nullptr;
        NsiParam->size1 = 0;
    }

    if (NsiParam->p2) {
        ExFreePoolWithTag(NsiParam->p2, TAG);
        NsiParam->p2 = nullptr;
        NsiParam->size2 = 0;
    }

    if (NsiParam->StateInfo) {
        ExFreePoolWithTag(NsiParam->StateInfo, TAG);
        NsiParam->StateInfo = nullptr;
        NsiParam->size3 = 0;
    }

    if (NsiParam->ProcessInfo) {
        ExFreePoolWithTag(NsiParam->ProcessInfo, TAG);
        NsiParam->ProcessInfo = nullptr;
        NsiParam->size4 = 0;
    }
}


NTSTATUS InitTcpNsiParam(_In_ PNsiParameters NsiParam)
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (nullptr == NsiParam || 0 == NsiParam->Counter) {

        return Status;
    }

    NsiParam->Counter += 2;
    NsiParam->ModuleId = &NPI_MS_TCP_MODULEID;
    NsiParam->Flag1 = 3;
    NsiParam->Flag2.QuadPart = 1 | 0x100000000i64;

    __try {        
        NsiParam->p1 = ExAllocatePoolWithTag(NonPagedPool, sizeof(TcpTable) * NsiParam->Counter, TAG);
        if (nullptr == NsiParam->p1) {

            __leave;
        }

        RtlZeroMemory(NsiParam->p1, sizeof(TcpTable) * NsiParam->Counter);
        NsiParam->size1 = sizeof(TcpTable);

        NsiParam->StateInfo = ExAllocatePoolWithTag(NonPagedPool, sizeof(StateTable) * NsiParam->Counter, TAG);
        if (nullptr == NsiParam->StateInfo) {

            __leave;
        }

        RtlZeroMemory(NsiParam->StateInfo, sizeof(StateTable) * NsiParam->Counter);
        NsiParam->size3 = sizeof(StateTable);

        NsiParam->ProcessInfo = ExAllocatePoolWithTag(NonPagedPool, sizeof(ProcessTable) * NsiParam->Counter, TAG);
        if (nullptr == NsiParam->ProcessInfo) {

            __leave;
        }

        RtlZeroMemory(NsiParam->ProcessInfo, sizeof(ProcessTable) * NsiParam->Counter);
        NsiParam->size4 = sizeof(ProcessTable);

        NsiParam->Counter -= 2;

        Status = STATUS_SUCCESS;
    } __finally {
        if (!NT_SUCCESS(Status)) {
            FreeNsiParam(NsiParam);
        }
    }

    return Status;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS EnumTcpTable()
/*
功能：枚举TCP信息。


*/
{
    NsiParameters NsiParam{};
    NTSTATUS Status = GetTcpNumbers(&NsiParam.Counter);
    if (!NT_SUCCESS(Status)) {

        return Status;
    }

    Status = InitTcpNsiParam(&NsiParam);
    if (!NT_SUCCESS(Status)) {

        return Status;
    }

    //Status = GetTcpInfoByApi(&NsiParam);
    Status = GetTcpInfoByIrp(&NsiParam);
    if (!NT_SUCCESS(Status)) {
        FreeNsiParam(&NsiParam);
        return Status;
    }

    //可以考虑把NsiParameters转换为PMIB_TCPTABLE。

    EnumTcpTable(&NsiParam);

    FreeNsiParam(&NsiParam);

    return Status;
}


NTSTATUS EnumUdpTable()
/*
功能：枚举UDP信息。


*/
{
    NTSTATUS Status = STATUS_SUCCESS;



    return Status;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
