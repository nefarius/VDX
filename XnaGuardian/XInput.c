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
#include "XInputInternal.h"
#include "XInput.tmh"

//
// Filter GetDeviceInfoFromInterface(...) result
// 
void XInputGetInformationCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    NTSTATUS            status;
    PVOID               buffer;
    size_t              buflen;
    PDEVICE_CONTEXT     pDeviceContext;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "%!FUNC! Entry");

    status = WdfRequestGetStatus(Request);
    pDeviceContext = DeviceGetContext(Context);

    KdPrint((DRIVERNAME "IOCTL_XINPUT_GET_INFORMATION called with status 0x%x\n", status));

    status = WdfRequestRetrieveOutputBuffer(Request, IO_GET_INFORMATION_OUT_SIZE, &buffer, &buflen);

    if (NT_SUCCESS(status))
    {
        //
        // 3rd byte in buffer contains the maximum supported devices 
        // on the current handle (typically either 0x01 or 0x04).
        // 
        pDeviceContext->MaxDevices = MAX_DEVICES_FROM_BUFFER(buffer);
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE,
            "pDeviceContext->MaxDevices = 0x%X", pDeviceContext->MaxDevices);
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
            "WdfRequestRetrieveOutputBuffer failed with status %!STATUS!", status);
    }

    WdfRequestComplete(Request, status);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "%!FUNC! Exit");
}

//
// Filter GetLatestDeviceInfo(...) result
// 
void XInputGetGamepadStateCompleted(
    _In_ WDFREQUEST                     Request,
    _In_ WDFIOTARGET                    Target,
    _In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_ WDFCONTEXT                     Context
)
{
    NTSTATUS                        status;
    PVOID                           buffer;
    size_t                          buflen;
    PXINPUT_GAMEPAD_STATE           pGamepad;
    PDEVICE_CONTEXT                 pDeviceContext;
    PXINPUT_PAD_IDENTIFIER_CONTEXT  pRequestContext;
    PXINPUT_PAD_STATE_INTERNAL      pPad;
    LONG                            padIndex = 0;
    LONG                            nButtonOverrides;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Params);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "%!FUNC! Entry");

    status = WdfRequestGetStatus(Request);

    KdPrint((DRIVERNAME "IOCTL_XINPUT_GET_GAMEPAD_STATE called with status 0x%x\n", status));

    pDeviceContext = DeviceGetContext(Context);
    pRequestContext = GetPadIdentifier(Request);

    //
    // When MaxDevices equals 1, the first array item can 
    // either contain zero or the assigned LED state value.
    // 
    if (pDeviceContext->MaxDevices == 0x01)
    {
        padIndex = pDeviceContext->LedValues[0] - XINPUT_LED_OFFSET;
    }

    //
    // When MaxDevices is greater than 1, the request context
    // contains the pad index on the current device handle.
    // 
    if (pDeviceContext->MaxDevices > 0x01)
    {
        padIndex = pDeviceContext->LedValues[pRequestContext->Index] - XINPUT_LED_OFFSET;
    }

    //
    // Check bounds and just complete request on error
    // 
    if (!VALID_USER_INDEX(padIndex))
    {
        TraceEvents(TRACE_LEVEL_WARNING, TRACE_QUEUE, "%!FUNC! Exit - invalid user index");
        WdfRequestComplete(Request, status);
        return;
    }

    //
    // Get global pad override data
    // 

    pPad = &PadStates[padIndex];

    status = WdfRequestRetrieveOutputBuffer(Request, IO_GET_GAMEPAD_STATE_OUT_SIZE, &buffer, &buflen);

    if (NT_SUCCESS(status))
    {
        //
        // Extract XINPUT_GAMEPAD structure from buffer
        // 
        pGamepad = GAMEPAD_FROM_STATE_BUFFER(buffer);

        //
        // Cache the values of the physical pad for use in peek call
        // 
        RtlCopyBytes(&PeekPadCache[padIndex], pGamepad, sizeof(XINPUT_GAMEPAD_STATE));

        //
        // Override buttons
        // 

        nButtonOverrides = pPad->Overrides & 0xFFFF;
        pGamepad->wButtons = (pGamepad->wButtons&~nButtonOverrides) | (pPad->Gamepad.wButtons&nButtonOverrides);

        //
        // Override axes
        //

        // Triggers
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_TRIGGER)
            pGamepad->bLeftTrigger = pPad->Gamepad.bLeftTrigger;
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_TRIGGER)
            pGamepad->bRightTrigger = pPad->Gamepad.bRightTrigger;

        // Left Thumb
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X)
            pGamepad->sThumbLX = pPad->Gamepad.sThumbLX;
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y)
            pGamepad->sThumbLY = pPad->Gamepad.sThumbLY;

        // Right Thumb
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X)
            pGamepad->sThumbRX = pPad->Gamepad.sThumbRX;
        if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y)
            pGamepad->sThumbRY = pPad->Gamepad.sThumbRY;
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveOutputBuffer failed with status %!STATUS!", status);
    }

    //
    // Always complete
    // 
    WdfRequestComplete(Request, status);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "%!FUNC! Exit");
}
