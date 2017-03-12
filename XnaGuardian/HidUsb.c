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
#include <usb.h>
#include <usbioctl.h>
#include <usbdlib.h>


NTSTATUS SendUrbBulkOrInterruptInRequest(WDFDEVICE Device, WDFREQUEST Request, PVOID Buffer, ULONG BufferLength)
{
    WDF_OBJECT_ATTRIBUTES   woa;
    NTSTATUS                status;
    WDFMEMORY               memory;
    PURB                    pUrb;

    KdPrint((DRIVERNAME "SendUrbBulkOrInterruptInRequest called\n"));

    WDF_OBJECT_ATTRIBUTES_INIT(&woa);

    woa.ParentObject = Request;

    status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES,
        NonPagedPool,
        0,
        sizeof(URB),
        &memory,
        (PVOID*)&pUrb);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //[…Format the XRB…]
    UsbBuildInterruptOrBulkTransferRequest(
        pUrb,
        sizeof(URB),
        (USBD_PIPE_HANDLE)0x01,
        Buffer,
        NULL,
        BufferLength,
        USBD_TRANSFER_DIRECTION_IN,
        NULL);
    
    // Parameters.Others.Argument3 is used by the IOCTL value
    status = WdfIoTargetFormatRequestForInternalIoctlOthers(
        WdfDeviceGetIoTarget(Device),
        Request,
        IOCTL_INTERNAL_USB_SUBMIT_URB,
        memory, NULL,        // Parameters.Others.Argument1
        WDF_NO_HANDLE, NULL, // Parameters.Others.Argument2
        WDF_NO_HANDLE, NULL   // Parameters.Others.Argument4
    );

    if (NT_SUCCESS(status))
    {
        WdfRequestSetCompletionRoutine(Request, LowerUsbBulkOrInterruptTransferCompleted, Device);

        // send async
        if (WdfRequestSend(Request, WdfDeviceGetIoTarget(Device), WDF_NO_SEND_OPTIONS) == FALSE)
        {
            // send failed
            WdfObjectDelete(memory);

            status = WdfRequestGetStatus(Request);

            KdPrint((DRIVERNAME "WdfRequestSend failed with status 0x%X\n", status));
        }
        else {
            status = STATUS_PENDING;
        }
    }

    if (!NT_SUCCESS(status)) {
        WdfObjectDelete(memory);
    }

    return status;
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
    PUCHAR                          pTransferBuffer;
    ULONG                           transferBufferLength;
    PXINPUT_PAD_STATE_INTERNAL      pPad;
    PDEVICE_CONTEXT                 pDeviceContext;
    ULONG                           index;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);

    status = WdfRequestGetStatus(Request);
    pUrb = URB_FROM_IRP(WdfRequestWdmGetIrp(Request));
    pTransferBuffer = (PUCHAR)pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
    transferBufferLength = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;
    pDeviceContext = DeviceGetContext(Context);

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

    // Left Thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X)
        RtlCopyBytes(&pTransferBuffer[0], &pPad->Gamepad.sThumbLX, 2);
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y)
        RtlCopyBytes(&pTransferBuffer[2], &pPad->Gamepad.sThumbLY, 2);

    // Right Thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X)
        RtlCopyBytes(&pTransferBuffer[4], &pPad->Gamepad.sThumbRX, 2);
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y)
        RtlCopyBytes(&pTransferBuffer[6], &pPad->Gamepad.sThumbRY, 2);

#ifdef DBG
    KdPrint((DRIVERNAME "BUFFER: "));
    for (ULONG i = 0; i < transferBufferLength; i++)
    {
        KdPrint(("%02X ", pTransferBuffer[i]));
    }
    KdPrint(("\n"));
#endif

    WdfRequestComplete(Request, status);
}

void LowerUsbBulkOrInterruptTransferCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    WDF_REQUEST_REUSE_PARAMS    params;
    NTSTATUS                    status;
    PURB                        pUrb;
    PUCHAR                      pTransferBuffer;
    ULONG                       transferBufferLength;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);
    UNREFERENCED_PARAMETER(Context);

    status = WdfRequestGetStatus(Request);

    KdPrint((DRIVERNAME "LowerUsbBulkOrInterruptTransferCompleted called with status 0x%X\n", status));
    if (TRUE)goto blargh;

    if (NT_SUCCESS(status))
    {
        pUrb = URB_FROM_IRP(WdfRequestWdmGetIrp(Request));
        pTransferBuffer = (PUCHAR)pUrb->UrbBulkOrInterruptTransfer.TransferBuffer;
        transferBufferLength = pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;

        KdPrint((DRIVERNAME "transferBufferLength = %d\n", transferBufferLength));

#ifdef DBG
        KdPrint((DRIVERNAME "BUFFER: "));
        for (ULONG i = 0; i < transferBufferLength; i++)
        {
            KdPrint(("%02X ", pTransferBuffer[i]));
        }
        KdPrint(("\n"));
#endif
    }

    WDF_REQUEST_REUSE_PARAMS_INIT(
        &params,
        WDF_REQUEST_REUSE_NO_FLAGS,
        STATUS_SUCCESS
    );

    status = WdfRequestReuse(Request, &params);

    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfRequestReuse failed with status 0x%X\n", status));
    }

    blargh:
    return;
}

