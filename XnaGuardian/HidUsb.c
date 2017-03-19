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


#include "Driver.h"


VOID XnaGuardianEvtUsbTargetPipeReadComplete(
    _In_ WDFUSBPIPE Pipe,
    _In_ WDFMEMORY  Buffer,
    _In_ size_t     NumBytesTransferred,
    _In_ WDFCONTEXT Context
)
{
    UNREFERENCED_PARAMETER(Pipe);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(NumBytesTransferred);
    UNREFERENCED_PARAMETER(Context);

    KdPrint((DRIVERNAME "NumBytesTransferred = %d\n", NumBytesTransferred));
}

//
// URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER completion routine.
// 
void UpperUsbBulkOrInterruptTransferCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    NTSTATUS                        status;
    PURB                            pUrb;
    PXINPUT_PAD_STATE_INTERNAL      pPad;
    ULONG                           index;
    PXINPUT_HID_INPUT_REPORT        pHidReport;
    LONG                            nButtonOverrides;
#ifdef DBG
    PUCHAR                          pTransferBuffer;
    ULONG                           transferBufferLength;
#endif

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);

    status = WdfRequestGetStatus(Request);
    pUrb = URB_FROM_IRP(WdfRequestWdmGetIrp(Request));
    pHidReport = (PXINPUT_HID_INPUT_REPORT)pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
#ifdef DBG
    pTransferBuffer = (PUCHAR)pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
    transferBufferLength = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;
#endif

    KdPrint((DRIVERNAME "UpperUsbBulkOrInterruptTransferCompleted called with status 0x%X\n", status));

    //
    // Map XInput user index to HID USB device by using device arrival order
    // 
    for (index = 0; index < XINPUT_MAX_DEVICES && index < WdfCollectionGetCount(HidUsbDeviceCollection); index++)
    {
        if (WdfCollectionGetItem(HidUsbDeviceCollection, index) == Context)
        {
            KdPrint((DRIVERNAME "HID USB Device 0x%X found at index %d\n", Context, index));
            break;
        }
    }

    //
    // Validate range
    // 
    if (index >= XINPUT_MAX_DEVICES)
    {
        WdfRequestComplete(Request, status);
        return;
    }

    pPad = &PadStates[index];

    //
    // Buttons
    // 
    nButtonOverrides = pPad->Overrides & 0xFFFF;
    pHidReport->Buttons = (pHidReport->Buttons&~nButtonOverrides) | (pPad->Gamepad.wButtons&nButtonOverrides);

    // Left Thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X)
        pHidReport->LeftThumbX = pPad->Gamepad.sThumbLX;
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y)
        pHidReport->LeftThumbY = pPad->Gamepad.sThumbLY;

    // Right Thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X)
        pHidReport->RightThumbX = pPad->Gamepad.sThumbRX;
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y)
        pHidReport->RightThumbY = pPad->Gamepad.sThumbRY;

#ifdef DBG
    KdPrint((DRIVERNAME "BUFFER: "));
    for (ULONG i = 0; i < transferBufferLength; i++)
    {
        KdPrint(("%02X ", pTransferBuffer[i]));
    }
    KdPrint(("\n"));
#endif

    status = (status == STATUS_CANCELLED) ? STATUS_SUCCESS : status;

    WdfRequestComplete(Request, status);
}

