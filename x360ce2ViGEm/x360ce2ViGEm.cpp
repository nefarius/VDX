// x360ce2ViGEm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Xinput.h>
#include <ViGEmUM.h>


int main()
{
    printf("Initializing emulation driver\n");

    if (!VIGEM_SUCCESS(vigem_init())) {
        printf("Couldn't initialize emulation driver\n");
        return 1;
    }

    printf("Enabling XInput\n");

    XInputEnable(TRUE);

    VIGEM_TARGET targets[XUSER_MAX_COUNT];

    for (int i = 0; i < XUSER_MAX_COUNT; i++)
    {
        VIGEM_TARGET_INIT(&targets[i]);
    }

    DWORD result;
    XINPUT_STATE state;
    XUSB_REPORT xReport;

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;

    QueryPerformanceFrequency(&frequency);

    printf("Starting translation, close window to exit...\n");

    while (true)
    {
        QueryPerformanceCounter(&t1);

        for (int i = 0; i < XUSER_MAX_COUNT; i++)
        {
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            result = XInputGetState(i, &state);

            if (result == ERROR_SUCCESS)
            {
                vigem_target_plugin(Xbox360Wired, &targets[i]);

                RtlCopyMemory(&xReport, &state.Gamepad, sizeof(XUSB_REPORT));

                vigem_xusb_submit_report(targets[i], xReport);
            }
            else
            {
                vigem_target_unplug(&targets[i]);
            }
        }

        Sleep(8);

        QueryPerformanceCounter(&t2);

        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

        printf("Polling frequency: %3.2f Hz\r", (1.0 / elapsedTime) * 1000);
    }

    return 0;
}

