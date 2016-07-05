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


// ViGEmUM.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ViGEmUM.h"
#include <SetupAPI.h>
#include <stdlib.h>
#include <winioctl.h>
#include <thread>

HANDLE g_hViGEmBus = INVALID_HANDLE_VALUE;


VIGEM_API VIGEM_ERROR vigem_init()
{
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;
    VIGEM_ERROR error = VIGEM_ERROR_NONE;

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
            error = VIGEM_ERROR_BUS_NOT_FOUND;
            continue;
        }

        // bus found, open it
        g_hViGEmBus = CreateFile(detailDataBuffer->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            nullptr);

        free(detailDataBuffer);
        break;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return error;
}

VIGEM_API VOID vigem_shutdown()
{
    if (g_hViGEmBus != nullptr)
    {
        CloseHandle(g_hViGEmBus);
    }
}

VIGEM_API VOID vigem_register_xusb_notification(
    IN VIGEM_XUSB_NOTIFICATION Notification,
    IN VIGEM_TARGET Target)
{
    if (Target.SerialNo == 0)
    {
        return;
    }

    std::thread _async{ [](
        VIGEM_XUSB_NOTIFICATION _Notification,
        VIGEM_TARGET _Target)
    {
        DWORD error = ERROR_SUCCESS;
        DWORD transfered = 0;
        BOOLEAN retval;
        OVERLAPPED lOverlapped = { 0 };
        lOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        XUSB_REQUEST_NOTIFICATION notify;
        XUSB_REQUEST_NOTIFICATION_INIT(&notify, _Target.SerialNo);

        do
        {
            DeviceIoControl(g_hViGEmBus, IOCTL_XUSB_REQUEST_NOTIFICATION, &notify, notify.Size, &notify, notify.Size, &transfered, &lOverlapped);

            if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
            {
                _Notification(_Target, notify.LargeMotor, notify.SmallMotor, notify.LedNumber);
            }
            else
            {
                error = GetLastError();
            }
        } while (error != ERROR_OPERATION_ABORTED && error != ERROR_ACCESS_DENIED);

    }, Notification, Target };

    _async.detach();
}

VIGEM_API VIGEM_ERROR vigem_target_plugin(
    VIGEM_TARGET_TYPE Type,
    PVIGEM_TARGET Target)
{
    DWORD transfered = 0;
    VIGEM_ERROR error = VIGEM_ERROR_NONE;
    VIGEM_PLUGIN_TARGET plugin;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (Target->SerialNo = 1; Target->SerialNo <= VIGEM_TARGETS_MAX; Target->SerialNo++)
    {
        VIGEM_PLUGIN_TARGET_INIT(&plugin, Target->SerialNo, Type);

        DeviceIoControl(g_hViGEmBus, IOCTL_VIGEM_PLUGIN_TARGET, &plugin, plugin.Size, nullptr, 0, &transfered, &lOverlapped);

        if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
        {
            break;
        }

        error = VIGEM_ERROR_NO_FREE_SLOT;
    }

    return error;
}

VIGEM_API VIGEM_ERROR vigem_xusb_submit_report(
    VIGEM_TARGET Target, 
    XUSB_REPORT Report)
{
    DWORD transfered = 0;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (Target.SerialNo == 0)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    XUSB_SUBMIT_REPORT report;
    XUSB_SUBMIT_REPORT_INIT(&report, Target.SerialNo);

    report.Report = Report;

    DeviceIoControl(g_hViGEmBus, IOCTL_XUSB_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, &lOverlapped);

    if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) == 0)
    {
        switch (GetLastError())
        {
        case ERROR_ACCESS_DENIED:
            return VIGEM_ERROR_INVALID_TARGET;
            break;
        default:
            break;
        }
    }

    return VIGEM_ERROR_NONE;
}

