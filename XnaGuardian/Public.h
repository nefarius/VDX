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
// Custom extensions
// 
#define XINPUT_EXT_TYPE                         0x8001
#define XINPUT_EXT_CODE                         0x801

#define IOCTL_XINPUT_EXT_HIDE_GAMEPAD           CTL_CODE(XINPUT_EXT_TYPE, XINPUT_EXT_CODE, METHOD_BUFFERED, FILE_WRITE_DATA)


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
