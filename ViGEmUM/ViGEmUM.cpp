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
#include <vector>

HANDLE g_hViGEmBus = INVALID_HANDLE_VALUE;


VIGEM_API VIGEM_ERROR vigem_init()
{
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;
    VIGEM_ERROR error = VIGEM_ERROR_BUS_NOT_FOUND;

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

        if (g_hViGEmBus != INVALID_HANDLE_VALUE)
        {
            CloseHandle(g_hViGEmBus);
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

        error = VIGEM_ERROR_NONE;

        break;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return error;
}

VIGEM_API VOID vigem_shutdown()
{
    if (g_hViGEmBus != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hViGEmBus);
        g_hViGEmBus = INVALID_HANDLE_VALUE;
    }
}

VIGEM_API VIGEM_ERROR vigem_register_xusb_notification(
    PVIGEM_XUSB_NOTIFICATION Notification,
    VIGEM_TARGET Target)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target.SerialNo == 0 || Notification == nullptr)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    std::thread _async{ [](
        PVIGEM_XUSB_NOTIFICATION _Notification,
        VIGEM_TARGET _Target)
    {
        DWORD error = ERROR_SUCCESS;
        DWORD transfered = 0;
        OVERLAPPED lOverlapped = { 0 };
        lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

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

        CloseHandle(lOverlapped.hEvent);

    }, Notification, Target };

    _async.detach();

    return VIGEM_ERROR_NONE;
}

VIGEM_API VIGEM_ERROR vigem_target_plugin(
    VIGEM_TARGET_TYPE Type,
    PVIGEM_TARGET Target)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target->State == VigemTargetNew)
    {
        return VIGEM_ERROR_TARGET_UNINITIALIZED;
    }

    if (Target->State == VigemTargetConnected)
    {
        return VIGEM_ERROR_ALREADY_CONNECTED;
    }

    DWORD transfered = 0;
    VIGEM_PLUGIN_TARGET plugin;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    for (Target->SerialNo = 1; Target->SerialNo <= VIGEM_TARGETS_MAX; Target->SerialNo++)
    {
        VIGEM_PLUGIN_TARGET_INIT(&plugin, Target->SerialNo, Type);

        DeviceIoControl(g_hViGEmBus, IOCTL_VIGEM_PLUGIN_TARGET, &plugin, plugin.Size, nullptr, 0, &transfered, &lOverlapped);

        if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
        {
            Target->State = VigemTargetConnected;
            CloseHandle(lOverlapped.hEvent);
            return VIGEM_ERROR_NONE;
        }
    }

    CloseHandle(lOverlapped.hEvent);

    return VIGEM_ERROR_NO_FREE_SLOT;
}

VIGEM_API VIGEM_ERROR vigem_target_unplug(PVIGEM_TARGET Target)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target->State == VigemTargetNew)
    {
        return VIGEM_ERROR_TARGET_UNINITIALIZED;
    }

    if (Target->State != VigemTargetConnected)
    {
        return VIGEM_ERROR_TARGET_NOT_PLUGGED_IN;
    }

    DWORD transfered = 0;
    VIGEM_UNPLUG_TARGET unplug;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    VIGEM_UNPLUG_TARGET_INIT(&unplug, Target->SerialNo);

    DeviceIoControl(g_hViGEmBus, IOCTL_VIGEM_UNPLUG_TARGET, &unplug, unplug.Size, nullptr, 0, &transfered, &lOverlapped);

    if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
    {
        Target->State = VigemTargetDisconnected;
        CloseHandle(lOverlapped.hEvent);
        return VIGEM_ERROR_NONE;
    }

    CloseHandle(lOverlapped.hEvent);

    return VIGEM_ERROR_REMOVAL_FAILED;
}

VIGEM_API VIGEM_ERROR vigem_register_ds4_notification(
    PVIGEM_DS4_NOTIFICATION Notification,
    VIGEM_TARGET Target)
{
    // TODO: de-duplicate this section

    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target.SerialNo == 0 || Notification == nullptr)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    std::thread _async{ [](
        PVIGEM_DS4_NOTIFICATION _Notification,
        VIGEM_TARGET _Target)
    {
        DWORD error = ERROR_SUCCESS;
        DWORD transfered = 0;
        OVERLAPPED lOverlapped = { 0 };
        lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        DS4_REQUEST_NOTIFICATION notify;
        DS4_REQUEST_NOTIFICATION_INIT(&notify, _Target.SerialNo);

        do
        {
            DeviceIoControl(g_hViGEmBus, IOCTL_DS4_REQUEST_NOTIFICATION, &notify, notify.Size, &notify, notify.Size, &transfered, &lOverlapped);

            if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) != 0)
            {
                _Notification(_Target, notify.Report.LargeMotor, notify.Report.SmallMotor, notify.Report.LightbarColor);
            }
            else
            {
                error = GetLastError();
            }
        } while (error != ERROR_OPERATION_ABORTED && error != ERROR_ACCESS_DENIED);

        CloseHandle(lOverlapped.hEvent);

    }, Notification, Target };

    _async.detach();

    return VIGEM_ERROR_NONE;
}

VIGEM_API VIGEM_ERROR vigem_xusb_submit_report(
    VIGEM_TARGET Target,
    XUSB_REPORT Report)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target.SerialNo == 0)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    DWORD transfered = 0;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    XUSB_SUBMIT_REPORT report;
    XUSB_SUBMIT_REPORT_INIT(&report, Target.SerialNo);

    report.Report = Report;

    DeviceIoControl(g_hViGEmBus, IOCTL_XUSB_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, &lOverlapped);

    if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) == 0)
    {
        CloseHandle(lOverlapped.hEvent);

        switch (GetLastError())
        {
        case ERROR_ACCESS_DENIED:
            return VIGEM_ERROR_INVALID_TARGET;
            break;
        default:
            break;
        }
    }

    CloseHandle(lOverlapped.hEvent);

    return VIGEM_ERROR_NONE;
}

VIGEM_API VIGEM_ERROR vigem_ds4_submit_report(VIGEM_TARGET Target, DS4_REPORT Report)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target.SerialNo == 0)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    DWORD transfered = 0;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    DS4_SUBMIT_REPORT report;
    DS4_SUBMIT_REPORT_INIT(&report, Target.SerialNo);

    report.Report = Report;

    DeviceIoControl(g_hViGEmBus, IOCTL_DS4_SUBMIT_REPORT, &report, report.Size, nullptr, 0, &transfered, &lOverlapped);

    if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) == 0)
    {
        CloseHandle(lOverlapped.hEvent);

        switch (GetLastError())
        {
        case ERROR_ACCESS_DENIED:
            return VIGEM_ERROR_INVALID_TARGET;
            break;
        default:
            break;
        }
    }

    CloseHandle(lOverlapped.hEvent);

    return VIGEM_ERROR_NONE;
}

VIGEM_API VIGEM_ERROR vigem_xgip_submit_report(VIGEM_TARGET Target, XGIP_REPORT Report)
{
    if (g_hViGEmBus == nullptr)
    {
        return VIGEM_ERROR_BUS_NOT_FOUND;
    }

    if (Target.SerialNo == 0)
    {
        return VIGEM_ERROR_INVALID_TARGET;
    }

    using namespace std;

    vector<vector<unsigned char>> packets;

    packets.push_back(
    {
        0x02, 0x20, 0x01, 0x1C, 0xB9, 0x06, 0xCF, 0xCF,
        0x27, 0x97, 0x00, 0x00, 0x6F, 0x0E, 0x39, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00
    });

    packets.push_back(
    {
        0x04, 0xF0, 0x02, 0x3A, 0x94, 0x02, 0x10, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x14, 0x01, 0xA7, 0x00,
        0x16, 0x00, 0x1F, 0x00, 0x20, 0x00, 0x27, 0x00,
        0x2D, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x06, 0x01,
        0x02, 0x03, 0x04, 0x06, 0x07, 0x05, 0x01, 0x04
    });

    packets.push_back(
    {
        0x04, 0xA0, 0x02, 0xBA, 0x00, 0x3A, 0x05, 0x06,
        0x0A, 0x02, 0x1A, 0x00, 0x57, 0x69, 0x6E, 0x64,
        0x6F, 0x77, 0x73, 0x2E, 0x58, 0x62, 0x6F, 0x78,
        0x2E, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x2E, 0x47,
        0x61, 0x6D, 0x65, 0x70, 0x61, 0x64, 0x1A, 0x00,
        0x50, 0x44, 0x50, 0x2E, 0x58, 0x62, 0x6F, 0x78,
        0x2E, 0x47, 0x61, 0x6D, 0x65, 0x70, 0x61, 0x64,
        0x2E, 0x41, 0x66, 0x74, 0x65, 0x72, 0x67, 0x6C
    });

    packets.push_back(
    {
        0x04, 0xA0, 0x02, 0xBA, 0x00, 0x74, 0x6F, 0x77,
        0x04, 0x56, 0xFF, 0x76, 0x97, 0xFD, 0x9B, 0x81,
        0x45, 0xAD, 0x45, 0xB6, 0x45, 0xBB, 0xA5, 0x26,
        0xD6, 0x2C, 0x40, 0x2E, 0x08, 0xDF, 0x07, 0xE1,
        0x45, 0xA5, 0xAB, 0xA3, 0x12, 0x7A, 0xF1, 0x97,
        0xB5, 0xE7, 0x1F, 0xF3, 0xB8, 0x86, 0x73, 0xE9,
        0x40, 0xA9, 0xF8, 0x2F, 0x21, 0x26, 0x3A, 0xCF,
        0xB7, 0xA9, 0x0F, 0x7E, 0xDB, 0x5A, 0x96, 0xBC
    });

    packets.push_back(
    {
        0x04, 0xA0, 0x02, 0x3A, 0xAE, 0x01, 0x44, 0x9C,
        0x5B, 0x0E, 0x39, 0x5C, 0x50, 0xBD, 0x7D, 0x04,
        0x17, 0x00, 0x20, 0x0E, 0x00, 0x01, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17,
        0x00, 0x09, 0x3C, 0x00, 0x01, 0x00, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00
    });

    packets.push_back(
    {
        0x04, 0xB0, 0x02, 0x2C, 0xE8, 0x01, 0x21, 0x3C,
        0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x17, 0x00, 0x22, 0x3C, 0x00,
        0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    });

    packets.push_back(
    {
        0x04, 0xA0, 0x02, 0x00, 0x94, 0x02
    });

    packets.push_back(
    {
        0x01, 0x20, 0x02, 0x09, 0x02, 0x05, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x09, 0x00
    });

    packets.push_back(
    {
        0x03, 0x20, 0x03, 0x04, 0x80, 0x00, 0x00, 0x00
    });

    packets.push_back(
    {
        0x20, 0x00, 0x04, 0x0e, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x1a, 0xff, 0x0c, 0x02, 0x33, 0x01,
        0xcf, 0x00
    });

    packets.push_back(
    {
        0x20, 0x00, 0x05, 0x0e, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x6d, 0xfe, 0x57, 0x03, 0x09, 0x02,
        0x64, 0x01
    });

    packets.push_back(
    {
        0x20, 0x00, 0x06, 0x0e, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xfa, 0xfd, 0x33, 0x04, 0x84, 0x02,
        0xbd, 0x01
    });

    packets.push_back(
    {
        0x20, 0x00, 0x07, 0x0e, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xc0, 0xfd, 0xd9, 0x04, 0xe0, 0x02,
        0xf8, 0x01
    });

    packets.push_back(
    {
        0x01, 0x20, 0x01, 0x09, 0x00, 0x06, 0x30, 0x3a,
        0x00, 0x00, 0x00, 0x00, 0x00
    });

    packets.push_back(
    {
        0x06, 0x30, 0x02, 0x06, 0x00, 0xc1, 0x00, 0x01,
        0x00, 0x00
    });

    DWORD transfered = 0;
    OVERLAPPED lOverlapped = { 0 };
    lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    XGIP_SUBMIT_INTERRUPT interrupt;

    for (auto &packet : packets)
    {
        XGIP_SUBMIT_INTERRUPT_INIT(&interrupt, Target.SerialNo);

        interrupt.InterruptLength = packet.size();
        memcpy(interrupt.Interrupt, packet.data(), packet.size());

        DeviceIoControl(g_hViGEmBus, IOCTL_XGIP_SUBMIT_INTERRUPT, &interrupt, interrupt.Size, nullptr, 0, &transfered, &lOverlapped);

        if (GetOverlappedResult(g_hViGEmBus, &lOverlapped, &transfered, TRUE) == 0)
        {
            CloseHandle(lOverlapped.hEvent);

            switch (GetLastError())
            {
            case ERROR_ACCESS_DENIED:
                return VIGEM_ERROR_INVALID_TARGET;
                break;
            default:
                break;
            }
        }

        Sleep(5);
    }

    CloseHandle(lOverlapped.hEvent);

    return VIGEM_ERROR_NONE;
}

