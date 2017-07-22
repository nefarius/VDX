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
        | XINPUT_GAMEPAD_OVERRIDE_LEFT_THUMB | XINPUT_GAMEPAD_OVERRIDE_RIGHT_THUMB);

    while (TRUE)
    {
        Sleep(100);

        pad.sThumbLX = SHRT_MIN;
        pad.sThumbLY = SHRT_MIN;
        pad.sThumbRX = SHRT_MIN;
        pad.sThumbRY = SHRT_MIN;

        printf("%d\n", XInputOverrideSetState(0, &pad));
    }

    return 0;
}

