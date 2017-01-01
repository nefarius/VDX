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


#include "driver.h"
#include "queue.tmh"
#include "XInputInternal.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, XnaGuardianQueueInitialize)
#endif

NTSTATUS
XnaGuardianQueueInitialize(
    _In_ WDFDEVICE Device
)
{
    WDFQUEUE                queue;
    NTSTATUS                status;
    WDF_IO_QUEUE_CONFIG     queueConfig;

    PAGED_CODE();

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &queueConfig,
        WdfIoQueueDispatchParallel
    );

    queueConfig.EvtIoDefault = XnaGuardianEvtIoDefault;
    queueConfig.EvtIoDeviceControl = XnaGuardianEvtIoDeviceControl;

    status = WdfIoQueueCreate(
        Device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &queue
    );

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

//
// Forward everything we're not interested in.
// 
VOID XnaGuardianEvtIoDefault(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request
)
{
    WDF_REQUEST_SEND_OPTIONS    options;
    NTSTATUS                    status;
    BOOLEAN                     ret;

    KdPrint((DRIVERNAME "XnaGuardianEvtIoDefault called\n"));

    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)), &options);

    if (ret == FALSE)
    {
        status = WdfRequestGetStatus(Request);
        KdPrint((DRIVERNAME "WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }
}

VOID XnaGuardianEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
)
{
    WDF_REQUEST_SEND_OPTIONS        options;
    NTSTATUS                        status;
    BOOLEAN                         ret;
    size_t                          buflen;
    PDEVICE_CONTEXT                 pDeviceContext;
    PVOID                           pBuffer;
    PXINPUT_EXT_HIDE_GAMEPAD        pHidePad;
    PXINPUT_EXT_OVERRIDE_GAMEPAD    pOverride;
    WDFDEVICE                       Device;

    KdPrint((DRIVERNAME "XnaGuardianEvtIoDeviceControl called with code 0x%X\n", IoControlCode));

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    Device = WdfIoQueueGetDevice(Queue);
    pDeviceContext = DeviceGetContext(Device);

    KdPrint((DRIVERNAME "Device = 0x%X\n", Device));

    //
    // Filter desired I/O-control codes
    // 
    switch (IoControlCode)
    {
    case IOCTL_XINPUT_GET_INFORMATION:

        KdPrint((DRIVERNAME ">> IOCTL_XINPUT_GET_INFORMATION\n"));

        KdPrint((DRIVERNAME "InputBufferLength = 0x%X\n", InputBufferLength));

        WdfRequestFormatRequestUsingCurrentType(Request);
        WdfRequestSetCompletionRoutine(Request, XInputGetInformationCompleted, Device);

        ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(Device), WDF_NO_SEND_OPTIONS);

        if (!ret) {
            status = WdfRequestGetStatus(Request);
            KdPrint((DRIVERNAME "WdfRequestSend failed: 0x%x\n", status));
        }

        return;

        //
        // Filter XInputGetState(...) call
        // 
    case IOCTL_XINPUT_GET_GAMEPAD_STATE:

        KdPrint((DRIVERNAME ">> IOCTL_XINPUT_GET_GAMEPAD_STATE\n"));

        WdfRequestFormatRequestUsingCurrentType(Request);
        WdfRequestSetCompletionRoutine(Request, XInputGetGamepadStateCompleted, Device);

        ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(Device), WDF_NO_SEND_OPTIONS);

        if (!ret) {
            status = WdfRequestGetStatus(Request);
            KdPrint((DRIVERNAME "WdfRequestSend failed: 0x%x\n", status));
        }

        return;

    case IOCTL_XINPUT_EXT_HIDE_GAMEPAD:

        KdPrint((DRIVERNAME ">> IOCTL_XINPUT_EXT_HIDE_GAMEPAD\n"));

        // 
        // Retrieve input buffer
        // 
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(XINPUT_EXT_HIDE_GAMEPAD), &pBuffer, &buflen);
        if (!NT_SUCCESS(status) || buflen < sizeof(XINPUT_EXT_HIDE_GAMEPAD))
        {
            KdPrint((DRIVERNAME "WdfRequestRetrieveInputBuffer failed with status 0x%X\n", status));
            WdfRequestComplete(Request, status);
            return;
        }

        pHidePad = (PXINPUT_EXT_HIDE_GAMEPAD)pBuffer;

        //
        // Validate padding
        // 
        if (pHidePad->Size != sizeof(XINPUT_EXT_HIDE_GAMEPAD))
        {
            WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
            return;
        }

        //
        // Set pad state
        // 
        pDeviceContext->PadStates[pHidePad->UserIndex].IsGetStateForbidden = pHidePad->Hidden;

        //
        // Complete request
        // 
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return;

    case IOCTL_XINPUT_EXT_OVERRIDE_GAMEPAD_STATE:

        KdPrint((DRIVERNAME ">> IOCTL_XINPUT_EXT_OVERRIDE_GAMEPAD_STATE\n"));

        // 
        // Retrieve input buffer
        // 
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(XINPUT_EXT_OVERRIDE_GAMEPAD), &pBuffer, &buflen);
        if (!NT_SUCCESS(status) || buflen < sizeof(XINPUT_EXT_OVERRIDE_GAMEPAD))
        {
            KdPrint((DRIVERNAME "WdfRequestRetrieveInputBuffer failed with status 0x%X\n", status));
            WdfRequestComplete(Request, status);
            return;
        }

        pOverride = (PXINPUT_EXT_OVERRIDE_GAMEPAD)pBuffer;

        //
        // Validate padding
        // 
        if (pOverride->Size != sizeof(XINPUT_EXT_OVERRIDE_GAMEPAD))
        {
            WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
            return;
        }

        //
        // Set pad overrides
        // 
        pDeviceContext->PadStates[pOverride->UserIndex].Overrides = pOverride->Overrides;
        pDeviceContext->PadStates[pOverride->UserIndex].Gamepad = pOverride->Gamepad;

        //
        // Complete request
        // 
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return;

    default:
        break;
    }

    //
    // Not our business, forward
    // 
    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(Device), &options);

    if (ret == FALSE)
    {
        status = WdfRequestGetStatus(Request);
        KdPrint((DRIVERNAME "WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }
}

void XInputGetGamepadStateCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    NTSTATUS                    status;
    PVOID                       buffer;
    size_t                      buflen;
    PXINPUT_GAMEPAD             pGamepad;
    PDEVICE_CONTEXT             pDeviceContext;
    //PXINPUT_GAMEPAD_OVERRIDES   pOverrides;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);

    status = WdfRequestGetStatus(Request);

    KdPrint((DRIVERNAME "IOCTL_XINPUT_GET_GAMEPAD_STATE called with status 0x%x\n", status));

    pDeviceContext = DeviceGetContext(Context);

    status = WdfRequestRetrieveInputBuffer(Request, IO_GET_GAMEPAD_STATE_IN_SIZE, &buffer, &buflen);

    if (NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "[IOCTL] [0x%X] [I] ", Context));

        for (size_t i = 0; i < buflen; i++)
        {
            KdPrint(("%02X ", ((PUCHAR)buffer)[i]));
        }
    }
    else
    {
        KdPrint((DRIVERNAME "WdfRequestRetrieveInputBuffer failed with status 0x%X", status));
    }
    
    status = WdfRequestRetrieveOutputBuffer(Request, IO_GET_GAMEPAD_STATE_OUT_SIZE, &buffer, &buflen);

    if (NT_SUCCESS(status))
    {
        pGamepad = GAMEPAD_FROM_BUFFER(buffer);
        pGamepad->bLeftTrigger = 0xFF;

        KdPrint(("[O] "));

        for (size_t i = 0; i < buflen; i++)
        {
            KdPrint(("%02X ", ((PUCHAR)buffer)[i]));
        }

        KdPrint(("\n"));
    }
    else
    {
        KdPrint((DRIVERNAME "WdfRequestRetrieveOutputBuffer failed with status 0x%x\n", status));
    }

    WdfRequestComplete(Request, status);
}

void XInputGetInformationCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    NTSTATUS                    status;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(Params);

    status = WdfRequestGetStatus(Request);

    KdPrint((DRIVERNAME "IOCTL_XINPUT_GET_INFORMATION called with status 0x%x\n", status));

    WdfRequestComplete(Request, status);
}

