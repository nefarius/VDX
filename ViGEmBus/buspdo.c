#include "busenum.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Bus_CreatePdo)
#pragma alloc_text(PAGE, Bus_EvtDeviceListCreatePdo)
#endif

NTSTATUS Bus_EvtDeviceListCreatePdo(
    WDFCHILDLIST DeviceList,
    PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription,
    PWDFDEVICE_INIT ChildInit)
{
    PPDO_IDENTIFICATION_DESCRIPTION pDesc;

    PAGED_CODE();

    pDesc = CONTAINING_RECORD(IdentificationDescription, PDO_IDENTIFICATION_DESCRIPTION, Header);

    return Bus_CreatePdo(WdfChildListGetDevice(DeviceList), ChildInit, pDesc->SerialNo, pDesc->TargetType);
}

NTSTATUS Bus_CreatePdo(
    _In_ WDFDEVICE Device,
         _In_ PWDFDEVICE_INIT DeviceInit,
         _In_ ULONG SerialNo,
         _In_ VIGEM_TARGET_TYPE TargetType)
{
    NTSTATUS status;
    PPDO_DEVICE_DATA pdoData = NULL;
    WDFDEVICE hChild = NULL;
    WDF_DEVICE_PNP_CAPABILITIES pnpCaps;
    WDF_DEVICE_POWER_CAPABILITIES powerCaps;
    WDF_OBJECT_ATTRIBUTES pdoAttributes;
    DECLARE_CONST_UNICODE_STRING(deviceLocation, L"Virtual Gamepad Emulation Bus");
    DECLARE_UNICODE_STRING_SIZE(buffer, MAX_INSTANCE_ID_LEN);
    DECLARE_UNICODE_STRING_SIZE(deviceId, MAX_INSTANCE_ID_LEN);
    DECLARE_UNICODE_STRING_SIZE(compatId, MAX_COMPATIBLE_IDS_LEN);

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Device);

    KdPrint(("Entered Bus_CreatePdo\n"));

    // set parameters matching desired target device
    switch (TargetType)
    {
        //
        // A Xbox 360 device was requested
        // 
    case Xbox360Wired:
        RtlInitUnicodeString(&deviceId, X360WIRED_HARDWARE_IDS);
        RtlInitUnicodeString(&compatId, X360WIRED_COMPATIBLE_IDS);
        break;
        //
        // A Sony DualShock 4 device was requested
        // 
    case DualShock4Wired:
        RtlInitUnicodeString(&deviceId, DS4WIRED_HARDWARE_IDS);
        RtlInitUnicodeString(&compatId, DS4WIRED_COMPATIBLE_IDS);
        break;
    }

    // set device type
    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_BUS_EXTENDER);

    // set device id
    status = WdfPdoInitAssignDeviceID(DeviceInit, &deviceId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // set hardware ids
    status = WdfPdoInitAddHardwareID(DeviceInit, &deviceId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // set compatible ids (if any)
    status = WdfPdoInitAddCompatibleID(DeviceInit, &compatId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // prepare instance id
    status = RtlUnicodeStringPrintf(&buffer, L"%02d", SerialNo);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // set instance id
    status = WdfPdoInitAssignInstanceID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // prepare device description
    status = RtlUnicodeStringPrintf(&buffer, L"Virtual Gamepad Emulation Device #%02d", SerialNo);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // set device description (for English operating systems)
    status = WdfPdoInitAddDeviceText(DeviceInit, &buffer, &deviceLocation, 0x409);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // default locale is English
    WdfPdoInitSetDefaultLocale(DeviceInit, 0x409);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&pdoAttributes, PDO_DEVICE_DATA);

    status = WdfDeviceCreate(&DeviceInit, &pdoAttributes, &hChild);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Get the device context.
    //
    pdoData = PdoGetData(hChild);

    pdoData->SerialNo = SerialNo;
    pdoData->CallingProcessId = CURRENT_PROCESS_ID();

    //
    // Set some properties for the child device.
    //
    WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);
    pnpCaps.Removable = WdfTrue;
    pnpCaps.EjectSupported = WdfTrue;
    pnpCaps.SurpriseRemovalOK = WdfTrue;

    pnpCaps.Address = SerialNo;
    pnpCaps.UINumber = SerialNo;

    WdfDeviceSetPnpCapabilities(hChild, &pnpCaps);

    WDF_DEVICE_POWER_CAPABILITIES_INIT(&powerCaps);

    powerCaps.DeviceD1 = WdfTrue;
    powerCaps.WakeFromD1 = WdfTrue;
    powerCaps.DeviceWake = PowerDeviceD1;

    powerCaps.DeviceState[PowerSystemWorking] = PowerDeviceD1;
    powerCaps.DeviceState[PowerSystemSleeping1] = PowerDeviceD1;
    powerCaps.DeviceState[PowerSystemSleeping2] = PowerDeviceD2;
    powerCaps.DeviceState[PowerSystemSleeping3] = PowerDeviceD2;
    powerCaps.DeviceState[PowerSystemHibernate] = PowerDeviceD3;
    powerCaps.DeviceState[PowerSystemShutdown] = PowerDeviceD3;

    WdfDeviceSetPowerCapabilities(hChild, &powerCaps);

    /*
    //
    // Create a custom interface so that other drivers can
    // query (IRP_MN_QUERY_INTERFACE) and use our callbacks directly.
    //
    RtlZeroMemory(&ToasterInterface, sizeof(ToasterInterface));

    ToasterInterface.InterfaceHeader.Size = sizeof(ToasterInterface);
    ToasterInterface.InterfaceHeader.Version = 1;
    ToasterInterface.InterfaceHeader.Context = (PVOID)hChild;

    //
    // Let the framework handle reference counting.
    //
    ToasterInterface.InterfaceHeader.InterfaceReference =
        WdfDeviceInterfaceReferenceNoOp;
    ToasterInterface.InterfaceHeader.InterfaceDereference =
        WdfDeviceInterfaceDereferenceNoOp;

    ToasterInterface.GetCrispinessLevel = Bus_GetCrispinessLevel;
    ToasterInterface.SetCrispinessLevel = Bus_SetCrispinessLevel;
    ToasterInterface.IsSafetyLockEnabled = Bus_IsSafetyLockEnabled;

    WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig,
        (PINTERFACE)&ToasterInterface,
        &GUID_TOASTER_INTERFACE_STANDARD,
        NULL);
    //
    // If you have multiple interfaces, you can call WdfDeviceAddQueryInterface
    // multiple times to add additional interfaces.
    //
    status = WdfDeviceAddQueryInterface(hChild, &qiConfig);

    if (!NT_SUCCESS(status)) {
        return status;
    }
    */

    return status;
}

