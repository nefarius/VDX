// ViGEmUM.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ViGEmUM.h"
#include <SetupAPI.h>
#include <stdlib.h>
#include <winioctl.h>
#include <thread>

HANDLE g_hViGEmBus = INVALID_HANDLE_VALUE;



VIGEM_API DWORD vigem_init()
{
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;
    DWORD error = ERROR_SUCCESS;

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
            error = ERROR_NOT_FOUND;
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

        if (_Target.SerialNo == 0)
        {
            return;
        }

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
        } while (error != ERROR_OPERATION_ABORTED);

    }, Notification, Target };

    _async.detach();
}

VIGEM_API DWORD vigem_target_plugin(
    VIGEM_TARGET_TYPE Type,
    PVIGEM_TARGET Target)
{
    DWORD transfered = 0;
    DWORD error = ERROR_SUCCESS;
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

        error = ERROR_NO_MORE_DEVICES;
    }

    return error;
}

VIGEM_API DWORD vigem_xusb_submit_report(
    VIGEM_TARGET Target, 
    XUSB_REPORT Report)
{
    DWORD transfered = 0;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (Target.SerialNo == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    XUSB_SUBMIT_REPORT report;
    XUSB_SUBMIT_REPORT_INIT(&report, Target.SerialNo);

    report.Report = Report;

    DeviceIoControl(g_hViGEmBus, IOCTL_XUSB_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, &lOverlapped);

    // TODO: add error checking
    GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE);

    return ERROR_SUCCESS;
}

