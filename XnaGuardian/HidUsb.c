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


//
// Gets the next available upper USB request - if any - and
// returns the associated request, a pointer to the transfer 
// buffer and a pointer to the transfer buffer length.
// 
BOOLEAN GetUpperUsbRequest(
    WDFDEVICE Device,
    WDFREQUEST *PendingRequest,
    PUCHAR *Buffer,
    PULONG BufferLength
)
{
    NTSTATUS            status;
    PDEVICE_CONTEXT     pDeviceContext;
    PURB                pUrb;

    pDeviceContext = DeviceGetContext(Device);

    status = WdfIoQueueRetrieveNextRequest(pDeviceContext->UpperUsbInterruptRequests, PendingRequest);

    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfIoQueueRetrieveNextRequest failed with status 0x%X\n", status));
        return FALSE;
    }

    pUrb = URB_FROM_IRP(WdfRequestWdmGetIrp(*PendingRequest));
    if (Buffer) *Buffer = (PUCHAR)pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
    if (BufferLength) *BufferLength = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;

    return TRUE;
}

VOID XnaGuardianEvtUsbTargetPipeReadComplete(
    _In_ WDFUSBPIPE Pipe,
    _In_ WDFMEMORY  Buffer,
    _In_ size_t     NumBytesTransferred,
    _In_ WDFCONTEXT Context
)
{
    PUCHAR                          pLowerBuffer;
    ULONG                           index;
    PXINPUT_PAD_STATE_INTERNAL      pPad;
    LONG                            nButtonOverrides;
    PXINPUT_HID_INPUT_REPORT        pHidReport;
    WDFREQUEST                      Request;
    PUCHAR                          pUpperBuffer;
    ULONG                           upperBufferLength;
    size_t                          lowerBufferLength;

    UNREFERENCED_PARAMETER(Pipe);

    if (!GetUpperUsbRequest(Context, &Request, &pUpperBuffer, &upperBufferLength)) return;

    pLowerBuffer = WdfMemoryGetBuffer(Buffer, NULL);
    lowerBufferLength = NumBytesTransferred;
    pHidReport = (PXINPUT_HID_INPUT_REPORT)pUpperBuffer;

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

    RtlCopyBytes(pUpperBuffer, pLowerBuffer, upperBufferLength);

    //
    // Validate range
    // 
    if (index >= XINPUT_MAX_DEVICES)
    {
        KdPrint((DRIVERNAME "Device index out of range: %d\n", index));
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return;
    }

    KdPrint((DRIVERNAME "Pad index %d\n", index));

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
    KdPrint((DRIVERNAME "BUFFER_UP: "));
    for (ULONG i = 0; i < upperBufferLength; i++)
    {
        KdPrint(("%02X ", pUpperBuffer[i]));
    }
    KdPrint(("\n"));

    KdPrint((DRIVERNAME "BUFFER_LO: "));
    for (ULONG i = 0; i < lowerBufferLength; i++)
    {
        KdPrint(("%02X ", pLowerBuffer[i]));
    }
    KdPrint(("\n"));
#endif

    WdfRequestComplete(Request, STATUS_SUCCESS);
}

