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


// ViGEmTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <initguid.h>
#include <public.h>
#include <SetupAPI.h>
#include <Xinput.h>
#include <time.h>
#include <ViGEmUM.h>

HANDLE bus;
int serial = 0;

VOID my_xusb_notification(
    VIGEM_TARGET Target,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    UCHAR LedNumber)
{
    printf("X360 Response - Serial: %d, LM: %d, SM: %d, LED: %d\n",
        Target.SerialNo,
        LargeMotor,
        SmallMotor,
        LedNumber);
}

VOID my_ds4_notification(
    VIGEM_TARGET Target,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    DS4_LIGHTBAR_COLOR LightbarColor)
{
    printf("DS4 Response - Serial: %d, LM: %d, SM: %d, R: %d, G: %d, B: %d\n",
        Target.SerialNo,
        LargeMotor,
        SmallMotor,
        LightbarColor.Red,
        LightbarColor.Green,
        LightbarColor.Blue);
}


int main()
{
    printf("Starting...\n");

    if (!VIGEM_SUCCESS(vigem_init()))
    {
        printf("Couldn't open bus\n");
        getchar();
        return 1;
    }

    //VIGEM_TARGET xbone;
    //VIGEM_TARGET_INIT(&xbone);
    //
    //if (!VIGEM_SUCCESS(vigem_target_plugin(XboxOneWired, &xbone)))
    //{
    //    printf("Couldn't get target object\n");
    //    getchar();
    //    return 1;
    //}
    //
    //printf("XBONE Success!");
    //getchar();

    VIGEM_TARGET x360;
    VIGEM_TARGET_INIT(&x360);

    if (!VIGEM_SUCCESS(vigem_target_plugin(Xbox360Wired, &x360)))
    {
        printf("Couldn't get target object\n");
        getchar();
        return 1;
    }

    vigem_register_xusb_notification(
        (VIGEM_XUSB_NOTIFICATION)my_xusb_notification,
        x360);

    printf("X360 Success!\n\n");
    getchar();

    VIGEM_TARGET ds4;
    VIGEM_TARGET_INIT(&ds4);

    if (!VIGEM_SUCCESS(vigem_target_plugin(DualShock4Wired, &ds4)))
    {
        printf("Couldn't get target object\n");
        getchar();
        return 1;
    }

    vigem_register_ds4_notification(
        (VIGEM_DS4_NOTIFICATION)my_ds4_notification,
        ds4);

    printf("DS4 Success!\n\n");
    getchar();

    XUSB_REPORT r = { 0 };
    DS4_REPORT ds4r = { 0 };

    while (getchar() != 'a')
    {
        r.bLeftTrigger++;
        ds4r.bTriggerL++;

        vigem_xusb_submit_report(x360, r);
        vigem_ds4_submit_report(ds4, ds4r);
    }

    getchar();
    getchar();

    return 0;
}

