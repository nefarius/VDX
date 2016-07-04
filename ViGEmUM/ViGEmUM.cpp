// ViGEmUM.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ViGEmUM.h"
#include <SetupAPI.h>
#include <stdlib.h>
#include <winioctl.h>

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

VIGEM_API DWORD vigem_register_xusb_notification(
    IN vigem_xusb_notification notification, 
    IN VIGEM_TARGET Target)
{
    return 0;
}

VIGEM_API DWORD vigem_target_plugin(
    VIGEM_TARGET_TYPE Type, 
    PVIGEM_TARGET Target)
{
    DWORD transfered = 0;
    DWORD error = ERROR_SUCCESS;
    VIGEM_PLUGIN_TARGET plugin;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = hEvent;
    
    for (UINT serial = 1; serial <= VIGEM_TARGETS_MAX; serial++)
    {
        VIGEM_PLUGIN_TARGET_INIT(&plugin, serial, Type);

        DeviceIoControl(g_hViGEmBus, IOCTL_VIGEM_PLUGIN_TARGET, &plugin, plugin.Size, nullptr, 0, &transfered, &lOverlapped);

        if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
        {
            break;
        }

        error = ERROR_NO_MORE_DEVICES;
    }

    return error;
}

