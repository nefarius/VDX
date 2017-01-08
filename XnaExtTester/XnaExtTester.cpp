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

    XInputOverrideSetMask(0, XINPUT_GAMEPAD_OVERRIDE_A | XINPUT_GAMEPAD_OVERRIDE_Y);
    XInputOverrideSetState(0, &pad);

    return 0;
}

