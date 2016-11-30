// HidGuardianTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <hidsdi.h>

#pragma comment(lib, "hid.lib")

DEFINE_GUID(GUID_DEVINTERFACE_HIDGUARDIAN,
    0x2f6dcf26, 0xeb50, 0x44ea, 0x90, 0x93, 0x4d, 0x83, 0x5c, 0xdd, 0xef, 0x2f);
// {2f6dcf26-eb50-44ea-9093-4d835cddef2f}


int main()
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;

    GUID hidClass;
    HidD_GetHidGuid(&hidClass);
    
    auto deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HIDGUARDIAN, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &GUID_DEVINTERFACE_HIDGUARDIAN, memberIndex++, &deviceInterfaceData))
    {
        printf("Found one!\n");

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

        printf("%ls\n", detailDataBuffer->DevicePath);

        if (hDevice != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hDevice);
        }

        // device found, open it
        hDevice = CreateFile(detailDataBuffer->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            nullptr);

        printf("hDevice = 0x%p, error = %d\n", hDevice, GetLastError());

        free(detailDataBuffer);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    printf("Done\n");
    getchar();

    return 0;
}

