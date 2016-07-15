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

    printf("Success!");
    getchar();

    XUSB_REPORT r = { 0 };

    while (getchar() != 'a')
    {
        r.bLeftTrigger++;

        vigem_xusb_submit_report(x360, r);
    }

    return 0;

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = {};
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;

    auto deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_BUSENUM_VIGEM, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &GUID_DEVINTERFACE_BUSENUM_VIGEM, memberIndex, &deviceInterfaceData))
    {
        // get required target buffer size
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);

        // allocate target buffer
        auto detailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
        detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // get detail buffer
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, requiredSize, &requiredSize, nullptr))
        {
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            free(detailDataBuffer);
            continue;
        }

        // bus found, open it
        bus = CreateFile(detailDataBuffer->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            nullptr);

        if (INVALID_HANDLE_VALUE != bus)
        {
            printf("bus opened! Enter serial:\n");

            serial = getchar() - 48;

            DWORD transfered = 0;
            VIGEM_PLUGIN_TARGET plugin;
            VIGEM_PLUGIN_TARGET_INIT(&plugin, serial, Xbox360Wired);

            auto retval = DeviceIoControl(bus, IOCTL_VIGEM_PLUGIN_TARGET, &plugin, plugin.Size, nullptr, 0, &transfered, nullptr);

            printf("IOCTL_BUSENUM_PLUGIN_HARDWARE retval: %d, trans: %d, error: %d\n", retval, transfered, GetLastError());

            getchar();

            XUSB_SUBMIT_REPORT report;
            XUSB_SUBMIT_REPORT_INIT(&report, serial);

            while (getchar() != 'a')
            {
                report.Report.bLeftTrigger++;

                retval = DeviceIoControl(bus, IOCTL_XUSB_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, nullptr);
                printf("IOCTL_XUSB_SUBMIT_REPORT retval: %d, trans: %d, report.Report.bLeftTrigger = %d\n", retval, transfered, report.Report.bLeftTrigger);
            }

            //DS4_SUBMIT_REPORT report;
            //DS4_SUBMIT_REPORT_INIT(&report, serial);
            //
            //int skip = 0;
            //srand(time(NULL));
            //
            //while (TRUE /*getchar() != 'a'*/)
            //{
            //    DS4_SET_DPAD(&report, Ds4DpadN);
            //
            //    retval = DeviceIoControl(bus, IOCTL_DS4_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, nullptr);
            //    printf("IOCTL_DS4_SUBMIT_REPORT retval: %d, trans: %d\n", retval, transfered);
            //    Sleep(25);
            //}

            getchar();
            printf("Enter unplug serial:\n");

            VIGEM_UNPLUG_TARGET unplug;
            VIGEM_UNPLUG_TARGET_INIT(&unplug, getchar() - 48);

            retval = DeviceIoControl(bus, IOCTL_VIGEM_UNPLUG_TARGET, &unplug, unplug.Size, nullptr, 0, &transfered, nullptr);

            printf("IOCTL_BUSENUM_UNPLUG_HARDWARE retval: %d, trans: %d\n", retval, transfered);

            getchar();
            getchar();

            CloseHandle(bus);
        }
        else
        {
            printf("Couldn't open bus: 0x%X", GetLastError());
        }

        free(detailDataBuffer);
        break;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    getchar();
    getchar();

    return 0;
}

