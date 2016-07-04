// ViGEmTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <initguid.h>
#include <public.h>
#include <SetupAPI.h>
#include <Xinput.h>
#include <time.h>

HANDLE bus;
int serial = 0;

DWORD WINAPI notify(LPVOID param)
{
    DWORD error = ERROR_SUCCESS;
    DWORD transfered = 0;
    BOOLEAN retval;
    XUSB_REQUEST_NOTIFICATION notify;
    XUSB_REQUEST_NOTIFICATION_INIT(&notify, serial);

    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    OVERLAPPED  lOverlapped = { 0 };
    lOverlapped.hEvent = hEvent;

    do
    {
        printf("Sending IOCTL_XUSB_REQUEST_NOTIFICATION request...\n");
        retval = DeviceIoControl(bus, IOCTL_XUSB_REQUEST_NOTIFICATION, &notify, notify.Size, &notify, notify.Size, &transfered, &lOverlapped);
        printf("IOCTL_XUSB_REQUEST_NOTIFICATION retval: %d, trans: %d\n", retval, transfered);

        if (GetLastError() == ERROR_IO_PENDING)
        {
            SetLastError(ERROR_SUCCESS);
        }

        GetOverlappedResult(bus, &lOverlapped, &transfered, TRUE);

        error = GetLastError();

        printf("IOCTL_XUSB_REQUEST_NOTIFICATION completed, LED: %d, Large: %d, Small: %d, error: %d\n", 
            notify.LedNumber, notify.LargeMotor, notify.SmallMotor, error);
    } while (error != ERROR_OPERATION_ABORTED);

    printf("Thread aborted...\n");

    return 0;
}

int main()
{
    printf("XUSB_SUBMIT_REPORT = %llu, XINPUT_GAMEPAD = %llu, XUSB_REPORT = %llu\n\n\n", sizeof(XUSB_SUBMIT_REPORT), sizeof(XINPUT_GAMEPAD), sizeof(XUSB_REPORT));

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

            DWORD myThreadID;
            HANDLE myHandle = CreateThread(0, 0, notify, NULL, 0, &myThreadID);

            //Sleep(2000);
            //
            //CloseHandle(bus);

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

