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

#ifdef XINPUTEXTENSIONS_EXPORTS
#define XINPUTEXTENSIONS_API __declspec(dllexport)
#else
#define XINPUTEXTENSIONS_API __declspec(dllimport)
#endif
#include <Xinput.h>


#ifdef __cplusplus
extern "C"
{ // only need to export C interface if
  // used by C++ source code
#endif

    XINPUTEXTENSIONS_API DWORD XInputOverrideSetMask(DWORD dwUserIndex, DWORD dwMask);

    XINPUTEXTENSIONS_API DWORD XInputOverrideSetState(DWORD dwUserIndex, PXINPUT_GAMEPAD pGamepad);

    XINPUTEXTENSIONS_API DWORD XInputOverridePeekState(DWORD dwUserIndex, PXINPUT_GAMEPAD pGamepad);

#ifdef __cplusplus
}
#endif
