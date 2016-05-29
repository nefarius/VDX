#include "busenum.h"

//
// Dummy function to satisfy USB interface
// 
BOOLEAN USB_BUSIFFN UsbPdo_IsDeviceHighSpeed(IN PVOID BusContext)
{
    UNREFERENCED_PARAMETER(BusContext);

    KdPrint(("UsbPdo_IsDeviceHighSpeed: TRUE\n"));

    return TRUE;
}

//
// Dummy function to satisfy USB interface
// 
NTSTATUS USB_BUSIFFN UsbPdo_QueryBusInformation(IN PVOID BusContext, IN ULONG Level, IN OUT PVOID BusInformationBuffer, IN OUT PULONG BusInformationBufferLength, OUT PULONG BusInformationActualLength)
{
    UNREFERENCED_PARAMETER(BusContext);
    UNREFERENCED_PARAMETER(Level);
    UNREFERENCED_PARAMETER(BusInformationBuffer);
    UNREFERENCED_PARAMETER(BusInformationBufferLength);
    UNREFERENCED_PARAMETER(BusInformationActualLength);

    KdPrint(("UsbPdo_QueryBusInformation: STATUS_UNSUCCESSFUL\n"));
    return STATUS_UNSUCCESSFUL;
}

//
// Dummy function to satisfy USB interface
// 
NTSTATUS USB_BUSIFFN UsbPdo_SubmitIsoOutUrb(IN PVOID BusContext, IN PURB Urb)
{
    UNREFERENCED_PARAMETER(BusContext);
    UNREFERENCED_PARAMETER(Urb);

    KdPrint(("UsbPdo_SubmitIsoOutUrb: STATUS_UNSUCCESSFUL\n"));
    return STATUS_UNSUCCESSFUL;
}

//
// Dummy function to satisfy USB interface
// 
NTSTATUS USB_BUSIFFN UsbPdo_QueryBusTime(IN PVOID BusContext, IN OUT PULONG CurrentUsbFrame)
{
    UNREFERENCED_PARAMETER(BusContext);
    UNREFERENCED_PARAMETER(CurrentUsbFrame);

    KdPrint(("UsbPdo_QueryBusTime: STATUS_UNSUCCESSFUL\n"));
    return STATUS_UNSUCCESSFUL;
}

//
// Dummy function to satisfy USB interface
// 
VOID USB_BUSIFFN UsbPdo_GetUSBDIVersion(IN PVOID BusContext, IN OUT PUSBD_VERSION_INFORMATION VersionInformation, IN OUT PULONG HcdCapabilities)
{
    UNREFERENCED_PARAMETER(BusContext);

    KdPrint(("UsbPdo_GetUSBDIVersion: 0x500, 0x200\n"));

    if (VersionInformation != NULL)
    {
        VersionInformation->USBDI_Version = 0x500; /* Usbport */
        VersionInformation->Supported_USB_Version = 0x200; /* USB 2.0 */
    }

    if (HcdCapabilities != NULL)
    {
        *HcdCapabilities = 0;
    }
}

//
// Set device descriptor to identify as wired Microsoft Xbox 360 Controller.
// 
NTSTATUS UsbPdo_GetDeviceDescriptorType(PURB urb)
{
    PUSB_DEVICE_DESCRIPTOR pDescriptor = (PUSB_DEVICE_DESCRIPTOR)urb->UrbControlDescriptorRequest.TransferBuffer;

    pDescriptor->bLength = 0x12;
    pDescriptor->bDescriptorType = USB_DEVICE_DESCRIPTOR_TYPE;
    pDescriptor->bcdUSB = 0x0200; // USB v2.0
    pDescriptor->bDeviceClass = 0xFF;
    pDescriptor->bDeviceSubClass = 0xFF;
    pDescriptor->bDeviceProtocol = 0xFF;
    pDescriptor->bMaxPacketSize0 = 0x08;
    pDescriptor->idVendor = 0x045E; // Microsoft Corp.
    pDescriptor->idProduct = 0x028E; // Xbox360 Controller
    pDescriptor->bcdDevice = 0x0114;
    pDescriptor->iManufacturer = 0x01;
    pDescriptor->iProduct = 0x02;
    pDescriptor->iSerialNumber = 0x03;
    pDescriptor->bNumConfigurations = 0x01;

    return STATUS_SUCCESS;
}

//
// Set configuration descriptor to identify as HID and exposed endpoints.
// 
NTSTATUS UsbPdo_GetConfigurationDescriptorType(PURB urb)
{
    /*
    0x09,        //   bLength
    0x02,        //   bDescriptorType (Configuration)
    0x99, 0x00,  //   wTotalLength 153
    0x04,        //   bNumInterfaces 4
    0x01,        //   bConfigurationValue
    0x00,        //   iConfiguration (String Index)
    0xA0,        //   bmAttributes Remote Wakeup
    0xFA,        //   bMaxPower 500mA

    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x00,        //   bInterfaceNumber 0
    0x00,        //   bAlternateSetting
    0x02,        //   bNumEndpoints 2
    0xFF,        //   bInterfaceClass
    0x5D,        //   bInterfaceSubClass
    0x01,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x11,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x00, 0x01,  //   bcdHID 1.00
    0x01,        //   bCountryCode
    0x25,        //   bNumDescriptors
    0x81,        //   bDescriptorType[0] (Unknown 0x81)
    0x14, 0x00,  //   wDescriptorLength[0] 20
    0x00,        //   bDescriptorType[1] (Unknown 0x00)
    0x00, 0x00,  //   wDescriptorLength[1] 0
    0x13,        //   bDescriptorType[2] (Unknown 0x13)
    0x01, 0x08,  //   wDescriptorLength[2] 2049
    0x00,        //   bDescriptorType[3] (Unknown 0x00)
    0x00,
    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x81,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x04,        //   bInterval 4 (unit depends on device speed)

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x01,        //   bEndpointAddress (OUT/H2D)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x08,        //   bInterval 8 (unit depends on device speed)

    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x01,        //   bInterfaceNumber 1
    0x00,        //   bAlternateSetting
    0x04,        //   bNumEndpoints 4
    0xFF,        //   bInterfaceClass
    0x5D,        //   bInterfaceSubClass
    0x03,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x1B,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x00, 0x01,  //   bcdHID 1.00
    0x01,        //   bCountryCode
    0x01,        //   bNumDescriptors
    0x82,        //   bDescriptorType[0] (Unknown 0x82)
    0x40, 0x01,  //   wDescriptorLength[0] 320
    0x02, 0x20, 0x16, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x82,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x02,        //   bInterval 2 (unit depends on device speed)

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x02,        //   bEndpointAddress (OUT/H2D)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x04,        //   bInterval 4 (unit depends on device speed)

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x83,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x40,        //   bInterval 64 (unit depends on device speed)

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x03,        //   bEndpointAddress (OUT/H2D)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x10,        //   bInterval 16 (unit depends on device speed)

    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x02,        //   bInterfaceNumber 2
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0xFF,        //   bInterfaceClass
    0x5D,        //   bInterfaceSubClass
    0x02,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x09,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x00, 0x01,  //   bcdHID 1.00
    0x01,        //   bCountryCode
    0x22,        //   bNumDescriptors
    0x84,        //   bDescriptorType[0] (Unknown 0x84)
    0x07, 0x00,  //   wDescriptorLength[0] 7

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x84,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x20, 0x00,  //   wMaxPacketSize 32
    0x10,        //   bInterval 16 (unit depends on device speed)

    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x03,        //   bInterfaceNumber 3
    0x00,        //   bAlternateSetting
    0x00,        //   bNumEndpoints 0
    0xFF,        //   bInterfaceClass
    0xFD,        //   bInterfaceSubClass
    0x13,        //   bInterfaceProtocol
    0x04,        //   iInterface (String Index)

    0x06,        //   bLength
    0x41,        //   bDescriptorType (Unknown)
    0x00, 0x01, 0x01, 0x03,
    // 153 bytes

    // best guess: USB Standard Descriptor
    */
    UCHAR DescriptorData[DESCRIPTOR_SIZE] =
    {
        0x09, 0x02, 0x99, 0x00, 0x04, 0x01, 0x00, 0xA0, 0xFA, 0x09,
        0x04, 0x00, 0x00, 0x02, 0xFF, 0x5D, 0x01, 0x00, 0x11, 0x21,
        0x00, 0x01, 0x01, 0x25, 0x81, 0x14, 0x00, 0x00, 0x00, 0x00,
        0x13, 0x01, 0x08, 0x00, 0x00, 0x07, 0x05, 0x81, 0x03, 0x20,
        0x00, 0x04, 0x07, 0x05, 0x01, 0x03, 0x20, 0x00, 0x08, 0x09,
        0x04, 0x01, 0x00, 0x04, 0xFF, 0x5D, 0x03, 0x00, 0x1B, 0x21,
        0x00, 0x01, 0x01, 0x01, 0x82, 0x40, 0x01, 0x02, 0x20, 0x16,
        0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x03, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x05, 0x82, 0x03, 0x20,
        0x00, 0x02, 0x07, 0x05, 0x02, 0x03, 0x20, 0x00, 0x04, 0x07,
        0x05, 0x83, 0x03, 0x20, 0x00, 0x40, 0x07, 0x05, 0x03, 0x03,
        0x20, 0x00, 0x10, 0x09, 0x04, 0x02, 0x00, 0x01, 0xFF, 0x5D,
        0x02, 0x00, 0x09, 0x21, 0x00, 0x01, 0x01, 0x22, 0x84, 0x07,
        0x00, 0x07, 0x05, 0x84, 0x03, 0x20, 0x00, 0x10, 0x09, 0x04,
        0x03, 0x00, 0x00, 0xFF, 0xFD, 0x13, 0x04, 0x06, 0x41, 0x00,
        0x01, 0x01, 0x03
    };

    // First request just gets required buffer size back
    if (urb->UrbControlDescriptorRequest.TransferBufferLength == sizeof(USB_CONFIGURATION_DESCRIPTOR))
    {
        PUSB_CONFIGURATION_DESCRIPTOR pDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)urb->UrbControlDescriptorRequest.TransferBuffer;

        pDescriptor->bLength = 0x09;
        pDescriptor->bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE;
        pDescriptor->wTotalLength = DESCRIPTOR_SIZE;
        pDescriptor->bNumInterfaces = 0x04;
        pDescriptor->bConfigurationValue = 0x01;
        pDescriptor->iConfiguration = 0x00;
        pDescriptor->bmAttributes = 0xA0;
        pDescriptor->MaxPower = 0xFA;
    }

    // Second request can store the whole descriptor
    if (urb->UrbControlDescriptorRequest.TransferBufferLength >= DESCRIPTOR_SIZE)
    {
        RtlCopyMemory(urb->UrbControlDescriptorRequest.TransferBuffer, DescriptorData, DESCRIPTOR_SIZE);
    }

    return STATUS_SUCCESS;
}

//
// Fakes a successfully selected configuration.
// 
NTSTATUS UsbPdo_SelectConfiguration(PURB urb)
{
    PUSBD_INTERFACE_INFORMATION pInfo;

    pInfo = &urb->UrbSelectConfiguration.Interface;

    KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: TotalLength %d, Size %d\n", urb->UrbHeader.Length, CONFIGURATION_SIZE));

    if (urb->UrbHeader.Length == sizeof(struct _URB_SELECT_CONFIGURATION))
    {
        KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: NULL ConfigurationDescriptor\n"));
        return STATUS_SUCCESS;
    }

    if (urb->UrbHeader.Length < CONFIGURATION_SIZE)
    {
        KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: Invalid ConfigurationDescriptor\n"));
        return STATUS_INVALID_PARAMETER;
    }

    KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: Length %d, Interface %d, Alternate %d, Pipes %d\n",
        (int)pInfo->Length,
        (int)pInfo->InterfaceNumber,
        (int)pInfo->AlternateSetting,
        pInfo->NumberOfPipes));

    pInfo->Class = 0xFF;
    pInfo->SubClass = 0x5D;
    pInfo->Protocol = 0x01;

    pInfo->InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

    pInfo->Pipes[0].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[0].MaximumPacketSize = 0x20;
    pInfo->Pipes[0].EndpointAddress = 0x81;
    pInfo->Pipes[0].Interval = 0x04;
    pInfo->Pipes[0].PipeType = 0x03;
    pInfo->Pipes[0].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0081;
    pInfo->Pipes[0].PipeFlags = 0x00;

    pInfo->Pipes[1].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[1].MaximumPacketSize = 0x20;
    pInfo->Pipes[1].EndpointAddress = 0x01;
    pInfo->Pipes[1].Interval = 0x08;
    pInfo->Pipes[1].PipeType = 0x03;
    pInfo->Pipes[1].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0001;
    pInfo->Pipes[1].PipeFlags = 0x00;

    pInfo = (PUSBD_INTERFACE_INFORMATION)((PCHAR)pInfo + pInfo->Length);

    KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: Length %d, Interface %d, Alternate %d, Pipes %d\n",
        (int)pInfo->Length,
        (int)pInfo->InterfaceNumber,
        (int)pInfo->AlternateSetting,
        pInfo->NumberOfPipes));

    pInfo->Class = 0xFF;
    pInfo->SubClass = 0x5D;
    pInfo->Protocol = 0x03;

    pInfo->InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

    pInfo->Pipes[0].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[0].MaximumPacketSize = 0x20;
    pInfo->Pipes[0].EndpointAddress = 0x82;
    pInfo->Pipes[0].Interval = 0x04;
    pInfo->Pipes[0].PipeType = 0x03;
    pInfo->Pipes[0].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0082;
    pInfo->Pipes[0].PipeFlags = 0x00;

    pInfo->Pipes[1].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[1].MaximumPacketSize = 0x20;
    pInfo->Pipes[1].EndpointAddress = 0x02;
    pInfo->Pipes[1].Interval = 0x08;
    pInfo->Pipes[1].PipeType = 0x03;
    pInfo->Pipes[1].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0002;
    pInfo->Pipes[1].PipeFlags = 0x00;

    pInfo->Pipes[2].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[2].MaximumPacketSize = 0x20;
    pInfo->Pipes[2].EndpointAddress = 0x83;
    pInfo->Pipes[2].Interval = 0x08;
    pInfo->Pipes[2].PipeType = 0x03;
    pInfo->Pipes[2].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0083;
    pInfo->Pipes[2].PipeFlags = 0x00;

    pInfo->Pipes[3].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[3].MaximumPacketSize = 0x20;
    pInfo->Pipes[3].EndpointAddress = 0x03;
    pInfo->Pipes[3].Interval = 0x08;
    pInfo->Pipes[3].PipeType = 0x03;
    pInfo->Pipes[3].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0003;
    pInfo->Pipes[3].PipeFlags = 0x00;

    pInfo = (PUSBD_INTERFACE_INFORMATION)((PCHAR)pInfo + pInfo->Length);

    KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: Length %d, Interface %d, Alternate %d, Pipes %d\n",
        (int)pInfo->Length,
        (int)pInfo->InterfaceNumber,
        (int)pInfo->AlternateSetting,
        pInfo->NumberOfPipes));

    pInfo->Class = 0xFF;
    pInfo->SubClass = 0x5D;
    pInfo->Protocol = 0x02;

    pInfo->InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

    pInfo->Pipes[0].MaximumTransferSize = 0x00400000;
    pInfo->Pipes[0].MaximumPacketSize = 0x20;
    pInfo->Pipes[0].EndpointAddress = 0x84;
    pInfo->Pipes[0].Interval = 0x04;
    pInfo->Pipes[0].PipeType = 0x03;
    pInfo->Pipes[0].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0084;
    pInfo->Pipes[0].PipeFlags = 0x00;

    pInfo = (PUSBD_INTERFACE_INFORMATION)((PCHAR)pInfo + pInfo->Length);

    KdPrint((">> >> >> URB_FUNCTION_SELECT_CONFIGURATION: Length %d, Interface %d, Alternate %d, Pipes %d\n",
        (int)pInfo->Length,
        (int)pInfo->InterfaceNumber,
        (int)pInfo->AlternateSetting,
        pInfo->NumberOfPipes));

    pInfo->Class = 0xFF;
    pInfo->SubClass = 0xFD;
    pInfo->Protocol = 0x13;

    pInfo->InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

    return STATUS_SUCCESS;
}

//
// Fakes a successfully selected interface.
// 
NTSTATUS UsbPdo_SelectInterface(PURB urb)
{
    PUSBD_INTERFACE_INFORMATION pInfo = &urb->UrbSelectInterface.Interface;

    KdPrint((">> >> >> URB_FUNCTION_SELECT_INTERFACE: Length %d, Interface %d, Alternate %d, Pipes %d\n",
        (int)pInfo->Length,
        (int)pInfo->InterfaceNumber,
        (int)pInfo->AlternateSetting,
        pInfo->NumberOfPipes));

    KdPrint((">> >> >> URB_FUNCTION_SELECT_INTERFACE: Class %d, SubClass %d, Protocol %d\n",
        (int)pInfo->Class,
        (int)pInfo->SubClass,
        (int)pInfo->Protocol));

    if (pInfo->InterfaceNumber == 1)
    {
        pInfo[0].Class = 0xFF;
        pInfo[0].SubClass = 0x5D;
        pInfo[0].Protocol = 0x03;
        pInfo[0].NumberOfPipes = 0x04;

        pInfo[0].InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

        pInfo[0].Pipes[0].MaximumTransferSize = 0x00400000;
        pInfo[0].Pipes[0].MaximumPacketSize = 0x20;
        pInfo[0].Pipes[0].EndpointAddress = 0x82;
        pInfo[0].Pipes[0].Interval = 0x04;
        pInfo[0].Pipes[0].PipeType = 0x03;
        pInfo[0].Pipes[0].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0082;
        pInfo[0].Pipes[0].PipeFlags = 0x00;

        pInfo[0].Pipes[1].MaximumTransferSize = 0x00400000;
        pInfo[0].Pipes[1].MaximumPacketSize = 0x20;
        pInfo[0].Pipes[1].EndpointAddress = 0x02;
        pInfo[0].Pipes[1].Interval = 0x08;
        pInfo[0].Pipes[1].PipeType = 0x03;
        pInfo[0].Pipes[1].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0002;
        pInfo[0].Pipes[1].PipeFlags = 0x00;

        pInfo[0].Pipes[2].MaximumTransferSize = 0x00400000;
        pInfo[0].Pipes[2].MaximumPacketSize = 0x20;
        pInfo[0].Pipes[2].EndpointAddress = 0x83;
        pInfo[0].Pipes[2].Interval = 0x08;
        pInfo[0].Pipes[2].PipeType = 0x03;
        pInfo[0].Pipes[2].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0083;
        pInfo[0].Pipes[2].PipeFlags = 0x00;

        pInfo[0].Pipes[3].MaximumTransferSize = 0x00400000;
        pInfo[0].Pipes[3].MaximumPacketSize = 0x20;
        pInfo[0].Pipes[3].EndpointAddress = 0x03;
        pInfo[0].Pipes[3].Interval = 0x08;
        pInfo[0].Pipes[3].PipeType = 0x03;
        pInfo[0].Pipes[3].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0003;
        pInfo[0].Pipes[3].PipeFlags = 0x00;

        return STATUS_SUCCESS;
    }

    if (pInfo->InterfaceNumber == 2)
    {
        pInfo[0].Class = 0xFF;
        pInfo[0].SubClass = 0x5D;
        pInfo[0].Protocol = 0x02;
        pInfo[0].NumberOfPipes = 0x01;

        pInfo[0].InterfaceHandle = (USBD_INTERFACE_HANDLE)0xFFFF0000;

        pInfo[0].Pipes[0].MaximumTransferSize = 0x00400000;
        pInfo[0].Pipes[0].MaximumPacketSize = 0x20;
        pInfo[0].Pipes[0].EndpointAddress = 0x84;
        pInfo[0].Pipes[0].Interval = 0x04;
        pInfo[0].Pipes[0].PipeType = 0x03;
        pInfo[0].Pipes[0].PipeHandle = (USBD_PIPE_HANDLE)0xFFFF0084;
        pInfo[0].Pipes[0].PipeFlags = 0x00;

        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}

NTSTATUS UsbPdo_BulkOrInterruptTransfer(PURB urb, WDFDEVICE Device)
{
    struct _URB_BULK_OR_INTERRUPT_TRANSFER* pTransfer = &urb->UrbBulkOrInterruptTransfer;

    UNREFERENCED_PARAMETER(pTransfer);

    PXUSB_DEVICE_DATA xusb = XusbGetData(Device);

    // Check context
    if (xusb == NULL)
    {
        KdPrint(("No XUSB context found on device %p\n", Device));

        return STATUS_UNSUCCESSFUL;
    }

    // Data coming FROM us TO higher driver
    if (pTransfer->TransferFlags & USBD_TRANSFER_DIRECTION_IN)
    {
        
    }

    // Data coming FROM the higher driver TO us
    if (pTransfer->TransferFlags & USBD_TRANSFER_DIRECTION_OUT)
    {
        KdPrint(("<< URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER : Handle %p, Flags %X, Length %d\n",
            pTransfer->PipeHandle,
            pTransfer->TransferFlags,
            pTransfer->TransferBufferLength));

        if (pTransfer->TransferBufferLength == XUSB_LEDSET_SIZE) // Led
        {
            UCHAR* Buffer = pTransfer->TransferBuffer;

            KdPrint(("-- LED Buffer: %02X %02X %02X", Buffer[0], Buffer[1], Buffer[2]));

            // extract LED byte to get controller slot
            if (Buffer[0] == 0x01 && Buffer[1] == 0x03 && Buffer[2] >= 0x02)
            {
                if (Buffer[2] == 0x02) xusb->LedNumber = 0;
                if (Buffer[2] == 0x03) xusb->LedNumber = 1;
                if (Buffer[2] == 0x04) xusb->LedNumber = 2;
                if (Buffer[2] == 0x05) xusb->LedNumber = 3;

                KdPrint(("-- LED Number: %d", xusb->LedNumber));
            }
        }

        if (pTransfer->TransferBufferLength == XUSB_RUMBLE_SIZE) // Rumble
        {
            UCHAR* Buffer = pTransfer->TransferBuffer;

            KdPrint(("-- Rumble Buffer: %02X %02X %02X %02X %02X %02X %02X %02X",
                Buffer[0],
                Buffer[1],
                Buffer[2],
                Buffer[3],
                Buffer[4],
                Buffer[5],
                Buffer[6],
                Buffer[7]));

            RtlCopyBytes(xusb->Rumble, Buffer, pTransfer->TransferBufferLength);
        }
    }
    
    return STATUS_SUCCESS;
}

