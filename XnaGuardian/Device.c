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
#include "Sideband.h"

XINPUT_PAD_STATE_INTERNAL   PadStates[XINPUT_MAX_DEVICES];
WDFWAITLOCK                 PadStatesLock;
XINPUT_GAMEPAD_STATE        PeekPadCache[XINPUT_MAX_DEVICES];
WDFWAITLOCK                 PeekPadCacheLock;

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

    //
    // Required for sideband clean-up
    // 
    deviceAttributes.EvtCleanupCallback = XnaGuardianCleanupCallback;

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status))
    {
        //
        // Initialize the I/O Package and any Queues
        //
        status = XnaGuardianQueueInitialize(device);

        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "XnaGuardianQueueInitialize failed with status 0x%X", status));
            return status;
        }

        //
        // Add this device to the FilterDevice collection.
        //
        WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);

        //
        // WdfCollectionAdd takes a reference on the item object and removes
        // it when you call WdfCollectionRemove.
        //
        status = WdfCollectionAdd(FilterDeviceCollection, device);
        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "WdfCollectionAdd failed with status code 0x%x\n", status));
        }
        WdfWaitLockRelease(FilterDeviceCollectionLock);

        //
        // Create a control device
        //
        status = FilterCreateControlDevice(device);
        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "FilterCreateControlDevice failed with status 0x%x\n",
                status));
        }
    }
    else
    {
        KdPrint((DRIVERNAME "WdfDeviceCreate failed with status 0x%X\n", status));
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
    ULONG   count;

    PAGED_CODE();

    KdPrint((DRIVERNAME "Entered XnaGuardianCleanupCallback\n"));

    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);

    count = WdfCollectionGetCount(FilterDeviceCollection);

    if (count == 1)
    {
        //
        // We are the last instance. So let us delete the control-device
        // so that driver can unload when the FilterDevice is deleted.
        // We absolutely have to do the deletion of control device with
        // the collection lock acquired because we implicitly use this
        // lock to protect ControlDevice global variable. We need to make
        // sure another thread doesn't attempt to create while we are
        // deleting the device.
        //
        FilterDeleteControlDevice((WDFDEVICE)Device);
    }

    WdfCollectionRemove(FilterDeviceCollection, Device);

    WdfWaitLockRelease(FilterDeviceCollectionLock);
}
#pragma warning(pop) // enable 28118 again

