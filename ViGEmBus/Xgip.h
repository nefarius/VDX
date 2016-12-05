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

#define XGIP_DESCRIPTOR_SIZE	        0x0040
#define XGIP_CONFIGURATION_SIZE         0x88
#define XGIP_REPORT_SIZE                0x12
#define XGIP_SYS_INIT_PACKETS           0x0F
#define XGIP_SYS_INIT_PERIOD            0x32

typedef struct _XGIP_DEVICE_DATA
{
    UCHAR Report[XGIP_REPORT_SIZE];

    //
    // Queue for incoming interrupt transfer
    //
    WDFQUEUE PendingUsbInRequests;

    //
    // Queue for inverted calls
    //
    WDFQUEUE PendingNotificationRequests;

    WDFCOLLECTION XboxgipSysInitCollection;

    BOOLEAN XboxgipSysInitReady;

    WDFTIMER XboxgipSysInitTimer;
} XGIP_DEVICE_DATA, *PXGIP_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(XGIP_DEVICE_DATA, XgipGetData)


NTSTATUS
Bus_XgipSubmitInterrupt(
    WDFDEVICE Device,
    ULONG SerialNo,
    PXGIP_SUBMIT_INTERRUPT Report,
    _In_ BOOLEAN FromInterface
);

//
// XGIP-specific functions
// 
NTSTATUS Xgip_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription);
NTSTATUS Xgip_PrepareHardware(WDFDEVICE Device);
NTSTATUS Xgip_AssignPdoContext(WDFDEVICE Device);
VOID Xgip_GetConfigurationDescriptorType(PUCHAR Buffer, ULONG Length);

