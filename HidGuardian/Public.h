/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_HIDGUARDIAN,
    0x2f6dcf26,0xeb50,0x44ea,0x90,0x93,0x4d,0x83,0x5c,0xdd,0xef,0x2f);
// {2f6dcf26-eb50-44ea-9093-4d835cddef2f}
