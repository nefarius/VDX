// XnaExtTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../XInputExtensions/XInputExtensions.h"
#include "../Common/XInputOverrides.h"
#include <climits>

int main()
{
    XINPUT_GAMEPAD pad = { 0 };
    auto counter = 0;

    XInputOverrideSetMask(0,
        XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_X | XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB_Y
        | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_X | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB_Y
        | XINPUT_GAMEPAD_OVERRIDE_LEFT_SHOULDER | XINPUT_GAMEPAD_OVERRIDE_RIGHT_SHOULDER
        | XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB
        | XINPUT_GAMEPAD_OVERRIDE_LEFT_TRIGGER | XINPUT_GAMEPAD_OVERRIDE_RIGHT_TRIGGER);

    while (TRUE)
    {
        Sleep(100);

        pad.sThumbLX = SHRT_MAX;
        pad.sThumbLY = SHRT_MAX;
        pad.sThumbRX = SHRT_MAX;
        pad.sThumbRY = SHRT_MAX;

        pad.bLeftTrigger = 10;
        pad.bRightTrigger = 255;

        printf("%d\n", XInputOverrideSetState(0, &pad));
    }

    return 0;
}

