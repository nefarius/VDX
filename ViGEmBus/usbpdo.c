#include "busenum.h"

BOOLEAN USB_BUSIFFN UsbPdo_IsDeviceHighSpeed(IN PVOID BusContext)
{
    UNREFERENCED_PARAMETER(BusContext);

    KdPrint(("UsbPdo_IsDeviceHighSpeed: TRUE\n"));

    return TRUE;
}