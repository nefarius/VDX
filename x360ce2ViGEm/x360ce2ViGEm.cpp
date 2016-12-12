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


// x360ce2ViGEm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Xinput.h>
#include <ViGEmUM.h>
#include <chrono>
#include <thread>


typedef void (WINAPI* HidGuardianOpen_t)();
typedef void (WINAPI* HidGuardianClose_t)();

static HidGuardianOpen_t fpOpen;
static HidGuardianOpen_t fpClose;

BOOL WINAPI HandlerRoutine(
    _In_ DWORD dwCtrlType
);

int main()
{
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    SetConsoleTitle(L"x360ce to ViGEm demo application");
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    HMODULE cerberus = LoadLibrary(L"HidCerberus.Lib.dll");

    if (cerberus == INVALID_HANDLE_VALUE)
    {
        printf("Couldn't find HidCerberus.Lib.dll\n");
        return 1;
    }

    fpOpen = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianOpen"));
    fpClose = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianClose"));

    printf("Bypassing HidGuardian\n");
    fpOpen();

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
                vigem_target_set_vid(&targets[i], 0x1234);
                vigem_target_set_pid(&targets[i], 0x0001);

                if (VIGEM_SUCCESS(vigem_target_plugin(Xbox360Wired, &targets[i])))
                {
                    printf("Plugged in controller %d\t\t\t\t\n", targets[i].SerialNo);
                }

                vigem_xusb_submit_report(targets[i], *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
            }
            else
            {
                if (VIGEM_SUCCESS(vigem_target_unplug(&targets[i])))
                {
                    printf("Unplugged controller %d\t\t\t\t\n", targets[i].SerialNo);
                }
            }
        }

        auto end = high_resolution_clock::now();
        auto dur = end - begin;
        auto ns = duration_cast<nanoseconds>(dur);
        auto delay = milliseconds(5) - ns;

        sleep_for(delay);

        auto finished = high_resolution_clock::now();

        printf("Polling delay: %1lld ms (Frequency: %3.2f Hz)\r",
            duration_cast<milliseconds>(delay).count(),
            (1.0 / duration_cast<milliseconds>(finished - begin).count()) * 1000);
    }

    return 0;
}

BOOL WINAPI HandlerRoutine(
    _In_ DWORD dwCtrlType
)
{
    if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        fpClose();
        return TRUE;
    }

    return FALSE;
}

