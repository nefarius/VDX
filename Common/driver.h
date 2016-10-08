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


// {A77BC4D5-6AF7-4E69-8DC4-6B88A6028CE6}
DEFINE_GUID(GUID_VIGEM_INTERFACE_STANDARD,
    0xA77BC4D5, 0x6AF7, 0x4E69, 0x8D, 0xC4, 0x6B, 0x88, 0xA6, 0x02, 0x8C, 0xE6);

#pragma once

DECLARE_GLOBAL_CONST_UNICODE_STRING(VigemNtDeviceName, L"\\Device\\ViGEmBus");
DECLARE_GLOBAL_CONST_UNICODE_STRING(VigemDosDeviceName, L"\\DosDevices\\ViGEmBus");

#include "public.h"

typedef VOID(*PINTERFACE_REFERENCE)(PVOID Context);
typedef VOID(*PINTERFACE_DEREFERENCE)(PVOID Context);

typedef
NTSTATUS
(*PVIGEM_FUNC_PLUGIN_TARGET)(
    IN PVOID Context,
    IN ULONG SerialNo,
    IN VIGEM_TARGET_TYPE TargetType,
    IN USHORT VendorId,
    IN USHORT ProductId
    );

typedef
NTSTATUS
(*PVIGEM_FUNC_UNPLUG_TARGET)(
    IN PVOID Context,
    IN ULONG SerialNo
    );

typedef
NTSTATUS
(*PVIGEM_FUNC_XUSB_SUBMIT_REPORT)(
    IN PVOID Context,
    IN ULONG SerialNo,
    IN PXUSB_SUBMIT_REPORT Report
    );

typedef
VOID
(*PVIGEM_FUNC_XUSB_NOTIFICATION_CALLBACK)(
    IN PVOID Context,
    IN UCHAR LargeMotor,
    IN UCHAR SmallMotor,
    IN UCHAR LedNumber
    );

typedef
NTSTATUS
(*PVIGEM_FUNC_XUSB_REQUEST_NOTIFICATION)(
    IN PVOID Context,
    IN ULONG SerialNo,
    IN PVIGEM_FUNC_XUSB_NOTIFICATION_CALLBACK Callback,
    IN PVOID CallbackContext
    );

typedef struct _VIGEM_INTERFACE_STANDARD
{
    INTERFACE Header;
    PVIGEM_FUNC_PLUGIN_TARGET PlugInTarget;
    PVIGEM_FUNC_UNPLUG_TARGET UnPlugTarget;
    PVIGEM_FUNC_XUSB_SUBMIT_REPORT XusbSubmitReport;
    PVIGEM_FUNC_XUSB_REQUEST_NOTIFICATION RegisterXusbRequestNotificationCallback;
} VIGEM_INTERFACE_STANDARD, *PVIGEM_INTERFACE_STANDARD;

