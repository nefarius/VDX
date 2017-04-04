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

EXTERN_C_START

#define GAMEPAD_FROM_STATE_BUFFER(_buffer_)     ((PXINPUT_GAMEPAD_STATE)&((PUCHAR)_buffer_)[11])
#define MAX_DEVICES_FROM_BUFFER(_buffer_)       ((ULONG)((PUCHAR)_buffer_)[2])
#define IS_INTERRUPT_IN(_urb_)                  ((_urb_->UrbBulkOrInterruptTransfer.TransferFlags & USBD_TRANSFER_DIRECTION_IN))


extern XINPUT_PAD_STATE_INTERNAL    PadStates[XINPUT_MAX_DEVICES];
extern XINPUT_GAMEPAD_STATE         PeekPadCache[XINPUT_MAX_DEVICES];

NTSTATUS
XnaGuardianQueueInitialize(
    _In_ WDFDEVICE hDevice
    );

NTSTATUS
UpperUsbInterruptRequestsQueueInitialize(
    _In_ WDFDEVICE Device
    );

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_DEFAULT XnaGuardianEvtIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL XnaGuardianEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL XnaGuardianEvtIoInternalDeviceControl;

EVT_WDF_REQUEST_COMPLETION_ROUTINE XInputGetInformationCompleted;
EVT_WDF_REQUEST_COMPLETION_ROUTINE XInputGetGamepadStateCompleted;

EXTERN_C_END
