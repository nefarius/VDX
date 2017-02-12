// XnaExtTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../XInputExtensions/XInputExtensions.h"
#include "../Common/XInputOverrides.h"

int main()
{
    XINPUT_GAMEPAD pad = { 0 };
    pad.wButtons |= XINPUT_GAMEPAD_A;
    pad.wButtons |= XINPUT_GAMEPAD_Y;

    XInputOverrideSetMask(0, 1);
    getchar();

    //XInputOverrideSetState(0, &pad);

    while(TRUE)
    {
        XInputOverridePeekState(0, &pad);
        printf("%X\n", pad.wButtons);
        Sleep(100);
    }

    return 0;
}

