#include "busenum.h"

BOOLEAN USB_BUSIFFN UsbPdo_IsDeviceHighSpeed(IN PVOID BusContext)
{
    UNREFERENCED_PARAMETER(BusContext);

    KdPrint(("UsbPdo_IsDeviceHighSpeed: TRUE\n"));

    return TRUE;
}

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

NTSTATUS USB_BUSIFFN UsbPdo_SubmitIsoOutUrb(IN PVOID BusContext, IN PURB Urb)
{
    UNREFERENCED_PARAMETER(BusContext);
    UNREFERENCED_PARAMETER(Urb);

    KdPrint(("UsbPdo_SubmitIsoOutUrb: STATUS_UNSUCCESSFUL\n"));
    return STATUS_UNSUCCESSFUL;
}

NTSTATUS USB_BUSIFFN UsbPdo_QueryBusTime(IN PVOID BusContext, IN OUT PULONG CurrentUsbFrame)
{
    UNREFERENCED_PARAMETER(BusContext);
    UNREFERENCED_PARAMETER(CurrentUsbFrame);

    KdPrint(("UsbPdo_QueryBusTime: STATUS_UNSUCCESSFUL\n"));
    return STATUS_UNSUCCESSFUL;
}

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

NTSTATUS UsbPdo_SetDeviceDescriptorType(PURB urb)
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

NTSTATUS UsbPdo_SetConfigurationDescriptorType(PURB urb)
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

    KdPrint(("UrbControlDescriptorRequest.TransferBufferLength = %d\n", urb->UrbControlDescriptorRequest.TransferBufferLength));

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

    if (urb->UrbControlDescriptorRequest.TransferBufferLength >= DESCRIPTOR_SIZE)
    {
        UCHAR* Buffer = urb->UrbControlDescriptorRequest.TransferBuffer;
        int    Index;
        
        for (Index = 0; Index < DESCRIPTOR_SIZE; Index++)
        {
            Buffer[Index] = DescriptorData[Index];
        }
    }

    return STATUS_SUCCESS;
}

