#include "busenum.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Bus_CreatePdo)
#pragma alloc_text(PAGE, Bus_EvtDeviceListCreatePdo)
#endif

NTSTATUS Bus_EvtDeviceListCreatePdo(WDFCHILDLIST DeviceList, PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription, PWDFDEVICE_INIT ChildInit)
{
    PPDO_IDENTIFICATION_DESCRIPTION pDesc;

    PAGED_CODE();

    pDesc = CONTAINING_RECORD(IdentificationDescription, PDO_IDENTIFICATION_DESCRIPTION, Header);

    return Bus_CreatePdo(WdfChildListGetDevice(DeviceList), ChildInit, pDesc->HardwareIds, pDesc->SerialNo);
}

NTSTATUS Bus_CreatePdo(_In_ WDFDEVICE Device, _In_ PWDFDEVICE_INIT DeviceInit, _In_reads_(MAX_INSTANCE_ID_LEN) PCWSTR HardwareIds, _In_ ULONG SerialNo)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(DeviceInit);
    UNREFERENCED_PARAMETER(HardwareIds);
    UNREFERENCED_PARAMETER(SerialNo);

    return STATUS_SUCCESS;
}

