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
    printf("Serial: %d, LM: %d, SM: %d, LED: %d\n",
        Target.SerialNo,
        LargeMotor,
        SmallMotor,
        LedNumber);
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

    goto skip;

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

    printf("X360 Success!");
    getchar();

    skip:

    VIGEM_TARGET ds4;
    VIGEM_TARGET_INIT(&ds4);

    if (!VIGEM_SUCCESS(vigem_target_plugin(DualShock4Wired, &ds4)))
    {
        printf("Couldn't get target object\n");
        getchar();
        return 1;
    }

    printf("DS4 Success!");
    getchar();

    XUSB_REPORT r = { 0 };
    DS4_REPORT ds4r = { 0 };

    while (getchar() != 'a')
    {
        r.bLeftTrigger++;
        ds4r.bTriggerL++;

        //vigem_xusb_submit_report(x360, r);
        vigem_ds4_submit_report(ds4, ds4r);
    }

    getchar();
    getchar();

    return 0;
}

