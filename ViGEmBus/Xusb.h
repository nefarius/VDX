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


#pragma once

#if defined(_X86_)
#define XUSB_CONFIGURATION_SIZE         0x00E4
#else
#define XUSB_CONFIGURATION_SIZE         0x0130
#endif
#define XUSB_DESCRIPTOR_SIZE	        0x0099
#define XUSB_REPORT_SIZE                20
#define XUSB_RUMBLE_SIZE                8
#define XUSB_LEDSET_SIZE                3
#define XUSB_LEDNUM_SIZE                1

//
// XUSB-specific device context data.
// 
typedef struct _XUSB_DEVICE_DATA
{
    //
    // Rumble buffer
    //
    UCHAR Rumble[XUSB_RUMBLE_SIZE];

    //
    // LED number (represents XInput slot index)
    //
    UCHAR LedNumber;

    //
    // Report buffer
    //
    UCHAR Report[XUSB_REPORT_SIZE];

    //
    // Queue for incoming interrupt transfer
    //
    WDFQUEUE PendingUsbInRequests;

    //
    // Queue for inverted calls
    //
    WDFQUEUE PendingNotificationRequests;

} XUSB_DEVICE_DATA, *PXUSB_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(XUSB_DEVICE_DATA, XusbGetData)
