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

NTSTATUS Xusb_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription)
{
    NTSTATUS status;
    UNICODE_STRING buffer;

    // prepare device description
    status = RtlUnicodeStringInit(DeviceDescription, L"Virtual Xbox 360 Controller");
    if (!NT_SUCCESS(status))
        return status;

    // Set hardware IDs
    RtlUnicodeStringInit(&buffer, L"USB\\VID_045E&PID_028E&REV_0114");

    status = WdfPdoInitAddHardwareID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringCopy(DeviceId, &buffer);

    RtlUnicodeStringInit(&buffer, L"USB\\VID_045E&PID_028E");

    status = WdfPdoInitAddHardwareID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    // Set compatible IDs
    RtlUnicodeStringInit(&buffer, L"USB\\MS_COMP_XUSB10");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringInit(&buffer, L"USB\\Class_FF&SubClass_5D&Prot_01");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringInit(&buffer, L"USB\\Class_FF&SubClass_5D");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    RtlUnicodeStringInit(&buffer, L"USB\\Class_FF");

    status = WdfPdoInitAddCompatibleID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
        return status;

    return STATUS_SUCCESS;
}

NTSTATUS Xusb_PrepareHardware(WDFDEVICE Device)
{
    NTSTATUS status;
    WDF_QUERY_INTERFACE_CONFIG ifaceCfg;

    INTERFACE dummyIface;

    dummyIface.Size = sizeof(INTERFACE);
    dummyIface.Version = 1;
    dummyIface.Context = (PVOID)Device;

    dummyIface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
    dummyIface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;

    /* XUSB.sys will query for the following three (unknown) interfaces
    * BUT WONT USE IT so we just expose them to satisfy initialization. */

    // Dummy 0

    WDF_QUERY_INTERFACE_CONFIG_INIT(&ifaceCfg, (PINTERFACE)&dummyIface, &GUID_DEVINTERFACE_XUSB_UNKNOWN_0, NULL);

    status = WdfDeviceAddQueryInterface(Device, &ifaceCfg);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("Couldn't register unknown interface GUID: %08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X (status 0x%x)\n",
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data1,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data2,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data3,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[0],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[1],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[2],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[3],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[4],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[5],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[6],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_0.Data4[7],
            status));

        return status;
    }

    // Dummy 1

    WDF_QUERY_INTERFACE_CONFIG_INIT(&ifaceCfg, (PINTERFACE)&dummyIface, &GUID_DEVINTERFACE_XUSB_UNKNOWN_1, NULL);

    status = WdfDeviceAddQueryInterface(Device, &ifaceCfg);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("Couldn't register unknown interface GUID: %08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X (status 0x%x)\n",
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data1,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data2,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data3,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[0],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[1],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[2],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[3],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[4],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[5],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[6],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_1.Data4[7],
            status));

        return status;
    }

    // Dummy 2

    WDF_QUERY_INTERFACE_CONFIG_INIT(&ifaceCfg, (PINTERFACE)&dummyIface, &GUID_DEVINTERFACE_XUSB_UNKNOWN_2, NULL);

    status = WdfDeviceAddQueryInterface(Device, &ifaceCfg);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("Couldn't register unknown interface GUID: %08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X (status 0x%x)\n",
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data1,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data2,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data3,
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[0],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[1],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[2],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[3],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[4],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[5],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[6],
            GUID_DEVINTERFACE_XUSB_UNKNOWN_2.Data4[7],
            status));

        return status;
    }

    // Expose USB_BUS_INTERFACE_USBDI_GUID

    // This interface actually IS used
    USB_BUS_INTERFACE_USBDI_V1 xusbInterface;

    xusbInterface.Size = sizeof(USB_BUS_INTERFACE_USBDI_V1);
    xusbInterface.Version = USB_BUSIF_USBDI_VERSION_1;
    xusbInterface.BusContext = (PVOID)Device;

    xusbInterface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
    xusbInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;

    xusbInterface.SubmitIsoOutUrb = UsbPdo_SubmitIsoOutUrb;
    xusbInterface.GetUSBDIVersion = UsbPdo_GetUSBDIVersion;
    xusbInterface.QueryBusTime = UsbPdo_QueryBusTime;
    xusbInterface.QueryBusInformation = UsbPdo_QueryBusInformation;
    xusbInterface.IsDeviceHighSpeed = UsbPdo_IsDeviceHighSpeed;

    WDF_QUERY_INTERFACE_CONFIG_INIT(&ifaceCfg, (PINTERFACE)&xusbInterface, &USB_BUS_INTERFACE_USBDI_GUID, NULL);

    status = WdfDeviceAddQueryInterface(Device, &ifaceCfg);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfDeviceAddQueryInterface failed status 0x%x\n", status));
        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS Xusb_AssignPdoContext(WDFDEVICE Device, PPDO_IDENTIFICATION_DESCRIPTION Description)
{
    NTSTATUS status;

    KdPrint(("Initializing XUSB context...\n"));

    PXUSB_DEVICE_DATA xusb = XusbGetData(Device);

    RtlZeroMemory(xusb, sizeof(XUSB_DEVICE_DATA));

    // Is later overwritten by actual XInput slot
    xusb->LedNumber = (UCHAR)Description->SerialNo;
    // This value never changes
    xusb->Report[1] = 0x14;

    // I/O Queue for pending IRPs
    WDF_IO_QUEUE_CONFIG usbInQueueConfig, notificationsQueueConfig;

    // Create and assign queue for incoming interrupt transfer
    WDF_IO_QUEUE_CONFIG_INIT(&usbInQueueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(Device, &usbInQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &xusb->PendingUsbInRequests);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    // Create and assign queue for user-land notification requests
    WDF_IO_QUEUE_CONFIG_INIT(&notificationsQueueConfig, WdfIoQueueDispatchManual);

    status = WdfIoQueueCreate(Device, &notificationsQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &xusb->PendingNotificationRequests);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    return STATUS_SUCCESS;
}

