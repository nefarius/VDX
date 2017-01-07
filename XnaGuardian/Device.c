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


#include "driver.h"
#include "device.tmh"
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

XINPUT_PAD_STATE_INTERNAL   PadStates[XINPUT_MAX_DEVICES];
WDFWAITLOCK                 PadStatesLock;

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, XnaGuardianCreateDevice)
#pragma alloc_text (PAGE, XnaGuardianCleanupCallback)
#endif


NTSTATUS
XnaGuardianCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    WDFDEVICE               device;
    NTSTATUS                status;

    PAGED_CODE();

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    deviceAttributes.EvtCleanupCallback = XnaGuardianCleanupCallback;

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status))
    {
        // 
        // Create pad states lock
        // 
        status = WdfWaitLockCreate(WDF_NO_OBJECT_ATTRIBUTES,
            &PadStatesLock);
        if (!NT_SUCCESS(status))
        {
            KdPrint((DRIVERNAME "WdfWaitLockCreate failed with status 0x%x\n", status));
            return status;
        }

        //
        // Initialize the I/O Package and any Queues
        //
        status = XnaGuardianQueueInitialize(device);

        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "XnaGuardianQueueInitialize failed with status 0x%X", status));
            return status;
        }
    }

    return status;
}

//
// Called on filter unload.
// 
#pragma warning(push)
#pragma warning(disable:28118) // this callback will run at IRQL=PASSIVE_LEVEL
_Use_decl_annotations_
VOID XnaGuardianCleanupCallback(
    _In_ WDFOBJECT Device
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(Device);

    KdPrint((DRIVERNAME "XnaGuardianCleanupCallback called\n"));
}
#pragma warning(pop) // enable 28118 again


