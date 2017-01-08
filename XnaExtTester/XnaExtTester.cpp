// XnaExtTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../XInputExtensions/XInputExtensions.h"
#include "../Common/XnaGuardianShared.h"

int main()
{
    XINPUT_GAMEPAD pad = { 0 };
    pad.wButtons |= XINPUT_GAMEPAD_A;
    pad.wButtons |= XINPUT_GAMEPAD_Y;

    printf("%X\n", XInputOverrideSetMask(0, XINPUT_GAMEPAD_OVERRIDE_A | XINPUT_GAMEPAD_OVERRIDE_Y));
    printf("%X\n", XInputOverrideSetState(0, &pad));

    return 0;
}

