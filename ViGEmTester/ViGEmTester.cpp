// ViGEmTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <initguid.h>
#include <public.h>
#include <SetupAPI.h>
#include <Xinput.h>


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
        auto bus = CreateFile(detailDataBuffer->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            nullptr);

        if (INVALID_HANDLE_VALUE != bus)
        {
            printf("bus opened!\n");

            DWORD transfered = 0;
            BUSENUM_PLUGIN_HARDWARE plugin = { 0 };
            plugin.Size = sizeof(BUSENUM_PLUGIN_HARDWARE);
            plugin.SerialNo = 1;
            plugin.TargetType = Xbox360Wired;

            auto retval = DeviceIoControl(bus, IOCTL_BUSENUM_PLUGIN_HARDWARE, &plugin, plugin.Size, nullptr, 0, &transfered, nullptr);

            printf("IOCTL_BUSENUM_PLUGIN_HARDWARE retval: %d, trans: %d\n", retval, transfered);

#ifdef OUTPUT
            getchar();

            XUSB_SUBMIT_REPORT report = { 0 };
            report.Size = sizeof(XUSB_SUBMIT_REPORT);
            report.SerialNo = 1;

            while (getchar() != 'a')
            {
                report.Report.bLeftTrigger++;

                retval = DeviceIoControl(bus, IOCTL_XUSB_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, nullptr);
                printf("IOCTL_XUSB_SUBMIT_REPORT retval: %d, trans: %d\n", retval, transfered);
            }
#endif

            getchar();

            HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            OVERLAPPED  lOverlapped = { 0 };
            lOverlapped.hEvent = hEvent;

            XUSB_REQUEST_NOTIFICATION notify = { 0 };
            notify.Size = sizeof(XUSB_REQUEST_NOTIFICATION);
            notify.SerialNo = 1;

            retval = DeviceIoControl(bus, IOCTL_XUSB_REQUEST_NOTIFICATION, &notify, notify.Size, &notify, notify.Size, &transfered, nullptr);
            printf("IOCTL_XUSB_REQUEST_NOTIFICATION retval: %d, trans: %d\n", retval, transfered);

            WaitForSingleObject(lOverlapped.hEvent, INFINITE);

            printf("IOCTL_XUSB_REQUEST_NOTIFICATION completed, LED: %d\n", notify.LedNumber);

            getchar();

            BUSENUM_UNPLUG_HARDWARE unplug = { 0 };
            unplug.Size = sizeof(BUSENUM_UNPLUG_HARDWARE);
            unplug.SerialNo = 0;

            retval = DeviceIoControl(bus, IOCTL_BUSENUM_UNPLUG_HARDWARE, &unplug, unplug.Size, nullptr, 0, &transfered, nullptr);

            printf("IOCTL_BUSENUM_UNPLUG_HARDWARE retval: %d, trans: %d\n", retval, transfered);

            CloseHandle(bus);
        }

        free(detailDataBuffer);
        break;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    getchar();

    return 0;
}

