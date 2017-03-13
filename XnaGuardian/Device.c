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
XINPUT_GAMEPAD_STATE        PeekPadCache[XINPUT_MAX_DEVICES];
WDFCOLLECTION               HidUsbDeviceCollection;
WDFWAITLOCK                 HidUsbDeviceCollectionLock;

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
    PDEVICE_CONTEXT         pDeviceContext;
    WDF_TIMER_CONFIG        timerCfg;

    PAGED_CODE();

#pragma region Set up filter device

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    //
    // Required for sideband clean-up
    // 
    deviceAttributes.EvtCleanupCallback = XnaGuardianCleanupCallback;

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    //
    // Opt out early on failure
    // 
    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfDeviceCreate failed with status 0x%X\n", status));
        return status;
    }

#pragma endregion

#pragma region Query device properties

    pDeviceContext = DeviceGetContext(device);

    WDF_OBJECT_ATTRIBUTES_INIT(&deviceAttributes);
    deviceAttributes.ParentObject = device;

    //
    // Query for current device's Hardware ID
    // 
    status = WdfDeviceAllocAndQueryProperty(device,
        DevicePropertyHardwareID,
        NonPagedPool,
        &deviceAttributes,
        &pDeviceContext->MemoryHardwareId
    );

    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "WdfDeviceAllocAndQueryProperty failed with status 0x%X", status));
        return status;
    }

    pDeviceContext->HardwareId = WdfMemoryGetBuffer(pDeviceContext->MemoryHardwareId, NULL);
    KdPrint((DRIVERNAME "HardwareID for device 0x%X: %ls\n", device, pDeviceContext->HardwareId));

    //
    // Query for current device's ClassName
    // 
    status = WdfDeviceAllocAndQueryProperty(device,
        DevicePropertyClassName,
        NonPagedPool,
        &deviceAttributes,
        &pDeviceContext->MemoryClassName
    );

    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "WdfDeviceAllocAndQueryProperty failed with status 0x%X", status));
        return status;
    }

    pDeviceContext->ClassName = WdfMemoryGetBuffer(pDeviceContext->MemoryClassName, NULL);
    KdPrint((DRIVERNAME "ClassName for device 0x%X: %ls\n", device, pDeviceContext->ClassName));

#pragma endregion

    //
    // Continue startup if loaded as XUSB/XGIP filter
    // 
    if (kmwcsstr(pDeviceContext->ClassName, L"XnaComposite")
        || kmwcsstr(pDeviceContext->ClassName, L"XboxComposite"))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "XUSB/XGIP device detected, loading...");
        pDeviceContext->IsXnaDevice = TRUE;
        goto continueInit;
    }

    //
    // Abort if unknown class
    // 
    if (!kmwcsstr(pDeviceContext->ClassName, L"HIDClass"))
    {
        KdPrint((DRIVERNAME "Unsupported device class detected, unloading...\n"));
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Only load below USB device in the stack
    // 
    if (!kmwcsstr(pDeviceContext->HardwareId, L"USB\\"))
    {
        KdPrint((DRIVERNAME "Topmost HID device detected, unloading...\n"));
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Check if device is XInput-compatible
    // 
    // See here: https://msdn.microsoft.com/en-US/library/windows/desktop/ee417014%28v=vs.85%29.aspx
    // 
    if (!kmwcsstr(pDeviceContext->HardwareId, L"IG_"))
    {
        KdPrint((DRIVERNAME "Regular HID device detected, unloading...\n"));
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Add HID USB device to its own collection
    // 
    WdfWaitLockAcquire(HidUsbDeviceCollectionLock, NULL);
    status = WdfCollectionAdd(HidUsbDeviceCollection, device);
    if (!NT_SUCCESS(status))
    {
        WdfWaitLockRelease(HidUsbDeviceCollectionLock);
        KdPrint((DRIVERNAME "WdfCollectionAdd failed with status 0x%X", status));
        return status;
    }
    WdfWaitLockRelease(HidUsbDeviceCollectionLock);

    pDeviceContext->IsHidUsbDevice = TRUE;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "HID USB device detected, loading...");

continueInit:

    KdPrint((DRIVERNAME "Compatible device detected, initializing...\n"));

    //
    // Initialize the I/O Package and any Queues
    //
    status = XnaGuardianQueueInitialize(device);

    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "XnaGuardianQueueInitialize failed with status 0x%X", status));
        return status;
    }

    //
    // Initialize request cancellation timer
    // 
    WDF_TIMER_CONFIG_INIT(&timerCfg, UsbBulkOrInterruptRequestTimerFunc);
    WDF_OBJECT_ATTRIBUTES_INIT(&deviceAttributes);

    deviceAttributes.ParentObject = device;

    status = WdfTimerCreate(&timerCfg, &deviceAttributes, &pDeviceContext->UsbBulkOrInterruptRequestTimer);
    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "Error creating output report timer 0x%x\n", status));
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
    ULONG               count;
    PDEVICE_CONTEXT     pDeviceContext;

    PAGED_CODE();

    KdPrint((DRIVERNAME "Entered XnaGuardianCleanupCallback\n"));

    pDeviceContext = DeviceGetContext(Device);

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

    if (pDeviceContext->IsHidUsbDevice)
    {
        KdPrint((DRIVERNAME "Removing HID USB device\n"));

        WdfWaitLockAcquire(HidUsbDeviceCollectionLock, NULL);
        WdfCollectionRemove(HidUsbDeviceCollection, Device);
        WdfWaitLockRelease(HidUsbDeviceCollectionLock);
    }
}
#pragma warning(pop) // enable 28118 again

