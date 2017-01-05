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


// ReSharper disable once CppMissingIncludeGuard
DEFINE_GUID(XUSB_INTERFACE_CLASS_GUID,
    0xEC87F1E3, 0xC13B, 0x4100, 0xB5, 0xF7, 0x8B, 0x84, 0xD5, 0x42, 0x60, 0xCB);
// {EC87F1E3-C13B-4100-B5F7-8B84D54260CB}

#pragma once


//
// Constants for gamepad buttons
//
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

//
// Custom extensions
// 
#define XINPUT_EXT_TYPE                         0x8001
#define XINPUT_EXT_CODE                         0x801

#define IOCTL_XINPUT_EXT_HIDE_GAMEPAD           CTL_CODE(XINPUT_EXT_TYPE, XINPUT_EXT_CODE + 0x00, METHOD_BUFFERED, FILE_WRITE_DATA)
#define IOCTL_XINPUT_EXT_OVERRIDE_GAMEPAD_STATE CTL_CODE(XINPUT_EXT_TYPE, XINPUT_EXT_CODE + 0x01, METHOD_BUFFERED, FILE_WRITE_DATA)


typedef struct _XINPUT_EXT_HIDE_GAMEPAD
{
    IN ULONG Size;

    IN UCHAR UserIndex;

    IN BOOLEAN Hidden;

} XINPUT_EXT_HIDE_GAMEPAD, *PXINPUT_EXT_HIDE_GAMEPAD;

VOID FORCEINLINE XINPUT_EXT_HIDE_GAMEPAD_INIT(
    _Out_ PXINPUT_EXT_HIDE_GAMEPAD HideGamepad,
    _In_ UCHAR UserIndex,
    _In_ BOOLEAN Hidden
)
{
    RtlZeroMemory(HideGamepad, sizeof(XINPUT_EXT_HIDE_GAMEPAD));

    HideGamepad->Size = sizeof(XINPUT_EXT_HIDE_GAMEPAD);
    HideGamepad->UserIndex = UserIndex;
    HideGamepad->Hidden = Hidden;
}

typedef enum _XINPUT_GAMEPAD_OVERRIDES
{
    XINPUT_GAMEPAD_OVERRIDE_DPAD_UP = 1 << 0,
    XINPUT_GAMEPAD_OVERRIDE_DPAD_DOWN = 1 << 1,
    XINPUT_GAMEPAD_OVERRIDE_DPAD_LEFT = 1 << 2,
    XINPUT_GAMEPAD_OVERRIDE_DPAD_RIGHT = 1 << 3,
    XINPUT_GAMEPAD_OVERRIDE_START = 1 << 4,
    XINPUT_GAMEPAD_OVERRIDE_BACK = 1 << 5,
    XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB = 1 << 6,
    XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB = 1 << 7,
    XINPUT_GAMEPAD_OVERRIDE_LEFT_SHOULDER = 1 << 8,
    XINPUT_GAMEPAD_OVERRIDE_RIGHT_SHOULDER = 1 << 9,
    XINPUT_GAMEPAD_OVERRIDE_A = 1 << 10,
    XINPUT_GAMEPAD_OVERRIDE_B = 1 << 11,
    XINPUT_GAMEPAD_OVERRIDE_X = 1 << 12,
    XINPUT_GAMEPAD_OVERRIDE_Y = 1 << 13,
    XINPUT_GAMEPAD_OVERRIDE_LEFT_TRIGGER = 1 << 14,
    XINPUT_GAMEPAD_OVERRIDE_RIGHT_TRIGGER = 1 << 15,
    XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X = 1 << 16,
    XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y = 1 << 17,
    XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X = 1 << 18,
    XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y = 1 << 19
} XINPUT_GAMEPAD_OVERRIDES, *PXINPUT_GAMEPAD_OVERRIDES;

//
// State of the gamepad (compatible to XINPUT_GAMEPAD)
// 
typedef struct _XINPUT_GAMEPAD {
    USHORT wButtons;
    BYTE   bLeftTrigger;
    BYTE   bRightTrigger;
    SHORT  sThumbLX;
    SHORT  sThumbLY;
    SHORT  sThumbRX;
    SHORT  sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

//
// Context data for IOCTL_XINPUT_EXT_OVERRIDE_GAMEPAD_STATE I/O control code
// 
typedef struct _XINPUT_EXT_OVERRIDE_GAMEPAD
{
    IN ULONG Size;

    IN UCHAR UserIndex;

    IN ULONG Overrides;

    IN XINPUT_GAMEPAD Gamepad;

} XINPUT_EXT_OVERRIDE_GAMEPAD, *PXINPUT_EXT_OVERRIDE_GAMEPAD;

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_INIT(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ UCHAR UserIndex
)
{
    RtlZeroMemory(OverrideGamepad, sizeof(XINPUT_EXT_OVERRIDE_GAMEPAD));

    OverrideGamepad->Size = sizeof(XINPUT_EXT_OVERRIDE_GAMEPAD);
    OverrideGamepad->UserIndex = UserIndex;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_DPAD_UP(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_UP;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_DPAD_UP;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_DPAD_DOWN(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_DOWN;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_DPAD_DOWN;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_DPAD_LEFT(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_LEFT;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_DPAD_LEFT;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_DPAD_RIGHT(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_RIGHT;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_DPAD_RIGHT;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_START(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_START;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_START;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_START;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_BACK(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_BACK;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_BACK;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_LEFT_THUMB(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_LEFT_THUMB;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_RIGHT_THUMB(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_RIGHT_THUMB;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_LEFT_SHOULDER(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_LEFT_SHOULDER;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_LEFT_SHOULDER;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_RIGHT_SHOULDER(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_RIGHT_SHOULDER;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_RIGHT_SHOULDER;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_A(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_A;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_A;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_A;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_B(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_B;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_B;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_B;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_X(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_X;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_X;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_X;
}

VOID FORCEINLINE XINPUT_EXT_OVERRIDE_GAMEPAD_Y(
    _Out_ PXINPUT_EXT_OVERRIDE_GAMEPAD OverrideGamepad,
    _In_ BOOLEAN Set
)
{
    if (Set) OverrideGamepad->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
    else OverrideGamepad->Gamepad.wButtons &= ~XINPUT_GAMEPAD_Y;

    OverrideGamepad->Overrides |= XINPUT_GAMEPAD_OVERRIDE_Y;
}
