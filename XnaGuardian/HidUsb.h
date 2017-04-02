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

#define X360_HID_USB_INPUT_REPORT_BUFFER_LENGTH     0x0E
#define XBONE_HID_USB_INPUT_REPORT_BUFFER_LENGTH    0x11

typedef struct _XINPUT_HID_INPUT_REPORT
{
    SHORT   LeftThumbX;
    SHORT   LeftThumbY;
    SHORT   RightThumbX;
    SHORT   RightThumbY;
    SHORT   ZAxis;
    USHORT  Buttons;

} XINPUT_HID_INPUT_REPORT, *PXINPUT_HID_INPUT_REPORT;

typedef struct _X360_HID_USB_INPUT_REPORT
{
    SHORT   LeftThumbX;
    SHORT   LeftThumbY;
    SHORT   RightThumbX;
    SHORT   RightThumbY;
    UCHAR   ZAxisEngaged;   // 0x00 default, 0x80 engaged
    UCHAR   ZAxis;          // 0x80 default
    USHORT  Buttons;

} X360_HID_USB_INPUT_REPORT, *PX360_HID_USB_INPUT_REPORT;

typedef struct _XBONE_HID_USB_INPUT_REPORT
{
    SHORT   LeftThumbX;
    SHORT   LeftThumbY;
    SHORT   RightThumbX;
    SHORT   RightThumbY;
    UCHAR   LeftTriggerValue;
    UCHAR   LeftTriggerLevel;
    UCHAR   RightTriggerValue;
    UCHAR   RightTriggerLevel;
    USHORT  Buttons;
    UCHAR   Dpad;

} XBONE_HID_USB_INPUT_REPORT, *PXBONE_HID_USB_INPUT_REPORT;

typedef enum _XBONE_HID_USB_INPUT_REPORT_BUTTONS
{
    XBONE_HID_USB_INPUT_REPORT_BUTTON_A                 = 0x0100,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_B                 = 0x0200,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_X                 = 0x0400,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_Y                 = 0x0800,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_SHOULDER     = 0x1000,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_SHOULDER    = 0x2000,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_START             = 0x4000,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_BACK              = 0x8000,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_LEFT_THUMB        = 0x0001,
    XBONE_HID_USB_INPUT_REPORT_BUTTON_RIGHT_THUMB       = 0x0002

} XBONE_HID_USB_INPUT_REPORT_BUTTONS, *PXBONE_HID_USB_INPUT_REPORT_BUTTONS;

EVT_WDF_USB_READER_COMPLETION_ROUTINE XnaGuardianEvtUsbTargetPipeReadComplete;

BOOLEAN GetUpperUsbRequest(
    WDFDEVICE Device,
    WDFREQUEST *PendingRequest,
    PUCHAR *Buffer,
    PULONG BufferLength
);

