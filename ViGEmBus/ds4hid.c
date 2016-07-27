/*
MIT License

Copyright (c) 2016 Benjamin "Nefarius" Höglinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "busenum.h"
#include <hidclass.h>

NTSTATUS Ds4_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription)
{
    NTSTATUS status;
    UNICODE_STRING buffer;

    // prepare device description
    status = RtlUnicodeStringInit(DeviceDescription, L"Virtual DualShock 4 Controller");
    if (!NT_SUCCESS(status))
        return status;

    // Set hardware IDs
    RtlUnicodeStringInit(&buffer, L"USB\\VID_054C&PID_05C4&REV_0100");

    status = WdfPdoInitAddHardwareID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringCopy(DeviceId, &buffer);

    RtlUnicodeStringInit(&buffer, L"USB\\VID_054C&PID_05C4");

    status = WdfPdoInitAddHardwareID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    // Set compatible IDs
    RtlUnicodeStringInit(&buffer, L"USB\\Class_03&SubClass_00&Prot_00");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringInit(&buffer, L"USB\\Class_03&SubClass_00");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringInit(&buffer, L"USB\\Class_03");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    return STATUS_SUCCESS;
}

NTSTATUS Ds4_PrepareHardware(WDFDEVICE Device)
{
    NTSTATUS status;
    WDF_QUERY_INTERFACE_CONFIG ifaceCfg;
    INTERFACE devinterfaceHid;

    devinterfaceHid.Size = sizeof(INTERFACE);
    devinterfaceHid.Version = 1;
    devinterfaceHid.Context = (PVOID)Device;

    devinterfaceHid.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
    devinterfaceHid.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;

    // Expose GUID_DEVINTERFACE_HID so HIDUSB can initialize
    WDF_QUERY_INTERFACE_CONFIG_INIT(&ifaceCfg, (PINTERFACE)&devinterfaceHid, &GUID_DEVINTERFACE_HID, NULL);

    status = WdfDeviceAddQueryInterface(Device, &ifaceCfg);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfDeviceAddQueryInterface failed status 0x%x\n", status));
        return status;
    }

    PDS4_DEVICE_DATA ds4Data = Ds4GetData(Device);

    // Set default HID input report (everything zero`d)
    UCHAR DefaultHidReport[DS4_REPORT_SIZE] =
    {
        0x01, 0x82, 0x7F, 0x7E, 0x80, 0x08, 0x00, 0x58,
        0x00, 0x00, 0xFD, 0x63, 0x06, 0x03, 0x00, 0xFE,
        0xFF, 0xFC, 0xFF, 0x79, 0xFD, 0x1B, 0x14, 0xD1,
        0xE9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x00,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00
    };

    // Initialize HID reports to defaults
    RtlCopyBytes(ds4Data->Report, DefaultHidReport, DS4_REPORT_SIZE);
    RtlZeroMemory(&ds4Data->OutputReport, sizeof(DS4_OUTPUT_REPORT));

    // Start pending IRP queue flush timer
    WdfTimerStart(ds4Data->PendingUsbInRequestsTimer, DS4_QUEUE_FLUSH_PERIOD);

    return STATUS_SUCCESS;
}

NTSTATUS Ds4_AssignPdoContext(WDFDEVICE Device, PPDO_IDENTIFICATION_DESCRIPTION Description)
{
    NTSTATUS status;

    PDS4_DEVICE_DATA ds4 = Ds4GetData(Device);

    KdPrint(("Initializing DS4 context...\n"));

    // I/O Queue for pending IRPs
    WDF_IO_QUEUE_CONFIG pendingUsbQueueConfig, notificationsQueueConfig;

    // Create and assign queue for incoming interrupt transfer
    WDF_IO_QUEUE_CONFIG_INIT(&pendingUsbQueueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(Device, &pendingUsbQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &ds4->PendingUsbInRequests);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    // Initialize periodic timer
    WDF_TIMER_CONFIG timerConfig;
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, Ds4_PendingUsbRequestsTimerFunc, DS4_QUEUE_FLUSH_PERIOD);

    // Timer object attributes
    WDF_OBJECT_ATTRIBUTES timerAttribs;
    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttribs);

    // PDO is parent
    timerAttribs.ParentObject = Device;

    // Create timer
    status = WdfTimerCreate(&timerConfig, &timerAttribs, &ds4->PendingUsbInRequestsTimer);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfTimerCreate failed 0x%x\n", status));
        return status;
    }

    // Create and assign queue for user-land notification requests
    WDF_IO_QUEUE_CONFIG_INIT(&notificationsQueueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(Device, &notificationsQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &ds4->PendingNotificationRequests);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    // Load/generate MAC address

    // TODO: tidy up this region

    WDFKEY keyParams, keyTargets, keyDS, keySerial;
    UNICODE_STRING keyName, valueName;

    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(), STANDARD_RIGHTS_ALL, WDF_NO_OBJECT_ATTRIBUTES, &keyParams);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfDriverOpenParametersRegistryKey failed 0x%x\n", status));
        return status;
    }

    RtlUnicodeStringInit(&keyName, L"Targets");

    status = WdfRegistryCreateKey(keyParams, &keyName,
        KEY_ALL_ACCESS, REG_OPTION_NON_VOLATILE, NULL, WDF_NO_OBJECT_ATTRIBUTES, &keyTargets);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRegistryCreateKey failed 0x%x\n", status));
        return status;
    }

    RtlUnicodeStringInit(&keyName, L"DualShock");

    status = WdfRegistryCreateKey(keyTargets, &keyName,
        KEY_ALL_ACCESS, REG_OPTION_NON_VOLATILE, NULL, WDF_NO_OBJECT_ATTRIBUTES, &keyDS);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRegistryCreateKey failed 0x%x\n", status));
        return status;
    }

    DECLARE_UNICODE_STRING_SIZE(serialPath, 4);
    RtlUnicodeStringPrintf(&serialPath, L"%04d", Description->SerialNo);

    status = WdfRegistryCreateKey(keyDS, &serialPath,
        KEY_ALL_ACCESS, REG_OPTION_NON_VOLATILE, NULL, WDF_NO_OBJECT_ATTRIBUTES, &keySerial);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRegistryCreateKey failed 0x%x\n", status));
        return status;
    }

    RtlUnicodeStringInit(&valueName, L"TargetMacAddress");

    status = WdfRegistryQueryValue(keySerial, &valueName, sizeof(MAC_ADDRESS), &ds4->TargetMacAddress, NULL, NULL);

    KdPrint(("MAC-Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        ds4->TargetMacAddress.Vendor0,
        ds4->TargetMacAddress.Vendor1,
        ds4->TargetMacAddress.Vendor2,
        ds4->TargetMacAddress.Nic0,
        ds4->TargetMacAddress.Nic1,
        ds4->TargetMacAddress.Nic2));

    if (status == STATUS_OBJECT_NAME_NOT_FOUND)
    {
        GenerateRandomMacAddress(&ds4->TargetMacAddress);

        status = WdfRegistryAssignValue(keySerial, &valueName, REG_BINARY, sizeof(MAC_ADDRESS), (PVOID)&ds4->TargetMacAddress);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRegistryAssignValue failed 0x%x\n", status));
            return status;
        }
    }
    else if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRegistryQueryValue failed 0x%x\n", status));
        return status;
    }

    WdfRegistryClose(keySerial);
    WdfRegistryClose(keyDS);
    WdfRegistryClose(keyTargets);
    WdfRegistryClose(keyParams);

    return STATUS_SUCCESS;
}

//
// Completes pending I/O requests if feeder is too slow.
// 
VOID Ds4_PendingUsbRequestsTimerFunc(
    _In_ WDFTIMER Timer
)
{
    NTSTATUS status;
    WDFREQUEST usbRequest;
    WDFDEVICE hChild;
    PDS4_DEVICE_DATA ds4Data;
    PIRP pendingIrp;
    PIO_STACK_LOCATION irpStack;

    // KdPrint(("Ds4_PendingUsbRequestsTimerFunc: Timer elapsed\n"));

    hChild = WdfTimerGetParentObject(Timer);
    ds4Data = Ds4GetData(hChild);

    // Get pending USB request
    status = WdfIoQueueRetrieveNextRequest(ds4Data->PendingUsbInRequests, &usbRequest);

    if (NT_SUCCESS(status))
    {
        // KdPrint(("Ds4_PendingUsbRequestsTimerFunc: pending IRP found\n"));

        // Get pending IRP
        pendingIrp = WdfRequestWdmGetIrp(usbRequest);
        irpStack = IoGetCurrentIrpStackLocation(pendingIrp);

        // Get USB request block
        PURB urb = (PURB)irpStack->Parameters.Others.Argument1;

        // Get transfer buffer
        PUCHAR Buffer = (PUCHAR)urb->UrbBulkOrInterruptTransfer.TransferBuffer;
        // Set buffer length to report size
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength = DS4_REPORT_SIZE;

        // Copy cached report to transfer buffer 
        RtlCopyBytes(Buffer, ds4Data->Report, DS4_REPORT_SIZE);

        // Complete pending request
        WdfRequestComplete(usbRequest, status);
    }
}

