// XnaExtTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../XInputExtensions/XInputExtensions.h"
#include "../Common/XInputOverrides.h"

int main()
{
    XINPUT_GAMEPAD pad = { 0 };
    auto counter = 0;
    
    XInputOverrideSetMask(0, 
        XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X | XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y
            | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y);

    while(TRUE)
    {
        XInputOverridePeekState(0, &pad);
        printf("%X\n", pad.wButtons);
        Sleep(100);

        counter += 1000;

        pad.sThumbLX = counter;
        pad.sThumbLY = counter;
        pad.sThumbRX = counter;
        pad.sThumbRY = counter;

        XInputOverrideSetState(0, &pad);
    }

    return 0;
}

