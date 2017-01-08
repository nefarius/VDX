#pragma once

#include "driver.h"

#define NTDEVICE_NAME_STRING        L"\\Device\\XnaGuardian"
#define SYMBOLIC_NAME_STRING        L"\\DosDevices\\XnaGuardian"

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL XnaGuardianSidebandIoDeviceControl;

NTSTATUS
FilterCreateControlDevice(
    WDFDEVICE Device
);

VOID
FilterDeleteControlDevice(
    WDFDEVICE Device
);
