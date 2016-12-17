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

//
// Official I/O-control codes used by XInput1_3.dll
// 
#define IOCTL_XINPUT_GET_INFORMATION	        0x80006000
#define IOCTL_XINPUT_GET_CAPABILITIES	        0x8000E004
#define IOCTL_XINPUT_GET_LED_STATE	            0x8000E008
#define IOCTL_XINPUT_GET_GAMEPAD_STATE	        0x8000E00C
#define IOCTL_XINPUT_SET_GAMEPAD_STATE	        0x8000A010
#define IOCTL_XINPUT_WAIT_FOR_GUIDE_BUTTON	    0x8000E014
#define IOCTL_XINPUT_GET_BATTERY_INFORMATION	0x8000E018
#define IOCTL_XINPUT_POWER_DOWN_DEVICE	        0x8000A01C
#define IOCTL_XINPUT_GET_AUDIO_INFORMATION	    0x8000E020


//
// Custom extensions
// 
#define XINPUT_EXT_TYPE                         0x8001
#define XINPUT_EXT_CODE                         0x801

#define IOCTL_XINPUT_EXT_HIDE_GAMEPAD           CTL_CODE(XINPUT_EXT_TYPE, XINPUT_EXT_CODE, METHOD_BUFFERED, FILE_WRITE_DATA)

