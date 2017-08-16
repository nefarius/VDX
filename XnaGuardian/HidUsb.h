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

#include <limits.h>

#define X360_HID_USB_INPUT_REPORT_BUFFER_LENGTH     0x0E
#define XBONE_HID_USB_INPUT_REPORT_BUFFER_LENGTH    0x11
#define XBONE_HID_USB_THUMB_AXIS_OFFSET             ((USHRT_MAX / 2) + 1)


typedef struct _X360_HID_USB_INPUT_REPORT
{
    SHORT LeftThumbX;
    SHORT LeftThumbY;
    SHORT RightThumbX;
    SHORT RightThumbY;
    UCHAR ZAxisEngaged; // 0x00 default, 0x80 engaged
    UCHAR ZAxis; // 0x80 default
    USHORT Buttons;
} X360_HID_USB_INPUT_REPORT, *PX360_HID_USB_INPUT_REPORT;

typedef struct _XBONE_HID_USB_INPUT_REPORT
{
    USHORT LeftThumbX;
    USHORT LeftThumbY;
    USHORT RightThumbX;
    USHORT RightThumbY;
    USHORT LeftTrigger;
    USHORT RightTrigger;
    USHORT Buttons;
    UCHAR  Dpad;
} XBONE_HID_USB_INPUT_REPORT, *PXBONE_HID_USB_INPUT_REPORT;

typedef enum _XBONE_HID_USB_INPUT_REPORT_BUTTONS
{
    XBONE_HID_USB_INPUT_REPORT_BUTTON_A = 0x0001,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_B = 0x0002,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_X = 0x0004,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_Y = 0x0008,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_SHOULDER = 0x0010,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_SHOULDER = 0x0020,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_START = 0x0040,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_BACK = 0x0080,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_THUMB = 0x0100,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_THUMB = 0x0200
} XBONE_HID_USB_INPUT_REPORT_BUTTONS, *PXBONE_HID_USB_INPUT_REPORT_BUTTONS;

typedef enum _XBONE_HID_USB_INPUT_REPORT_DPAD
{
    XBONE_HID_USB_INPUT_REPORT_DPAD_NONE = 0x00,
    XBONE_HID_USB_INPUT_REPORT_DPAD_N = 0x01,
    XBONE_HID_USB_INPUT_REPORT_DPAD_NE = 0x02,
    XBONE_HID_USB_INPUT_REPORT_DPAD_E = 0x03,
    XBONE_HID_USB_INPUT_REPORT_DPAD_SE = 0x04,
    XBONE_HID_USB_INPUT_REPORT_DPAD_S = 0x05,
    XBONE_HID_USB_INPUT_REPORT_DPAD_SW = 0x06,
    XBONE_HID_USB_INPUT_REPORT_DPAD_W = 0x07,
    XBONE_HID_USB_INPUT_REPORT_DPAD_NW = 0x08
} XBONE_HID_USB_INPUT_REPORT_DPAD, *PXBONE_HID_USB_INPUT_REPORT_DPAD;

EVT_WDF_USB_READER_COMPLETION_ROUTINE XnaGuardianEvtUsbTargetPipeReadComplete;

BOOLEAN GetUpperUsbRequest(
    WDFDEVICE Device,
    WDFREQUEST* PendingRequest,
    PUCHAR* Buffer,
    PULONG BufferLength
);

VOID FORCEINLINE XINPUT_GAMEPAD_TO_XBONE_HID_USB_INPUT_REPORT(
    PXINPUT_PAD_STATE_INTERNAL pPad,
    PXBONE_HID_USB_INPUT_REPORT pXboneReport
)
{
    // Left Thumb Axes
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X)
        pXboneReport->LeftThumbX = pPad->Gamepad.sThumbLX + XBONE_HID_USB_THUMB_AXIS_OFFSET;
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y)
        pXboneReport->LeftThumbY = pPad->Gamepad.sThumbLY + XBONE_HID_USB_THUMB_AXIS_OFFSET;

    // Right Thumb Axes
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X)
        pXboneReport->RightThumbX = pPad->Gamepad.sThumbRX + XBONE_HID_USB_THUMB_AXIS_OFFSET;
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y)
        pXboneReport->RightThumbY = pPad->Gamepad.sThumbRY + XBONE_HID_USB_THUMB_AXIS_OFFSET;

    // Left Trigger
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_TRIGGER)
    {
        pXboneReport->LeftTrigger = pPad->Gamepad.bLeftTrigger * 4;
    }

    // Right Trigger
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_TRIGGER)
    {
        pXboneReport->RightTrigger = pPad->Gamepad.bRightTrigger * 4;
    }

    // A
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_A)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_A;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_A;

    // B
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_B)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_B;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_B;

    // X
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_X)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_X;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_X;

    // Y
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_Y)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_Y;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_Y;

    // Left shoulder
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_SHOULDER)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_SHOULDER;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_SHOULDER;

    // Right shoulder
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_SHOULDER)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_SHOULDER;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_SHOULDER;

    // Start
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_START)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_START;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_START;

    // Back
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_BACK)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_BACK;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_BACK;

    // Left thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_THUMB;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_THUMB;

    // Right thumb
    if (pPad->Overrides & XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB)
        if ((pPad->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
            pXboneReport->Buttons |= XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_THUMB;
        else
            pXboneReport->Buttons &= ~XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_THUMB;
}
