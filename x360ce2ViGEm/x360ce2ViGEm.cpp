// x360ce2ViGEm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Xinput.h>
#include <ViGEmUM.h>
#include <chrono>
#include <thread>


int main()
{
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    SetConsoleTitle(L"x360ce to ViGEm demo application");

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

    printf("Starting translation, close window to exit...\n");

    while (true)
    {
        auto begin = high_resolution_clock::now();

        for (int i = 0; i < XUSER_MAX_COUNT; i++)
        {
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            result = XInputGetState(i, &state);

            if (result == ERROR_SUCCESS)
            {
                if (VIGEM_SUCCESS(vigem_target_plugin(Xbox360Wired, &targets[i])))
                {
                    printf("Plugged in controller %d\n", targets[i].SerialNo);
                }

                RtlCopyMemory(&xReport, &state.Gamepad, sizeof(XUSB_REPORT));

                vigem_xusb_submit_report(targets[i], xReport);
            }
            else
            {
                if (VIGEM_SUCCESS(vigem_target_unplug(&targets[i])))
                {
                    printf("Unplugged in controller %d\n", targets[i].SerialNo);
                }
            }
        }

        auto end = high_resolution_clock::now();
        auto dur = end - begin;
        auto ns = duration_cast<nanoseconds>(dur);
        auto delay = milliseconds(7) - ns;

        sleep_for(delay);

        auto finished = high_resolution_clock::now();

        printf("Polling delay: %1d ms (Frequency: %3.2f Hz)\r",
            duration_cast<milliseconds>(delay).count(),
            (1.0 / duration_cast<milliseconds>(finished - begin).count()) * 1000);
    }

    return 0;
}

