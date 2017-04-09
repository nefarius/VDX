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
    WDF_OBJECT_ATTRIBUTES           deviceAttributes;
    WDFDEVICE                       device;
    NTSTATUS                        status;
    PDEVICE_CONTEXT                 pDeviceContext;
    WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;

    PAGED_CODE();

#pragma region Set up filter device

    WdfFdoInitSetFilter(DeviceInit);

    //
    // Set PNP & power callbacks.
    // 
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    pnpPowerCallbacks.EvtDevicePrepareHardware = XnaGuardianEvtDevicePrepareHardware;
    pnpPowerCallbacks.EvtDeviceD0Entry = XnaGuardianEvtDeviceD0Entry;

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

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
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreate failed with status %!STATUS!", status);
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
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceAllocAndQueryProperty failed with status %!STATUS!", status);
        return status;
    }

    pDeviceContext->HardwareId = WdfMemoryGetBuffer(pDeviceContext->MemoryHardwareId, NULL);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "HardwareID: %ls", pDeviceContext->HardwareId);

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
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceAllocAndQueryProperty failed with status %!STATUS!", status);
        return status;
    }

    pDeviceContext->ClassName = WdfMemoryGetBuffer(pDeviceContext->MemoryClassName, NULL);
    KdPrint((DRIVERNAME "ClassName for device 0x%X: %ls\n", device, pDeviceContext->ClassName));
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "ClassName: %ls", pDeviceContext->ClassName);

#pragma endregion

#pragma region Identify device

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
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Unsupported device class detected, unloading...");
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Only load below USB device in the stack
    // 
    if (!kmwcsstr(pDeviceContext->HardwareId, L"USB\\"))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Topmost HID device detected, unloading...");
        return STATUS_NOT_SUPPORTED;
    }

    //
    // Check if device is XInput-compatible
    // 
    // See here: https://msdn.microsoft.com/en-US/library/windows/desktop/ee417014%28v=vs.85%29.aspx
    // 
    if (!kmwcsstr(pDeviceContext->HardwareId, L"IG_"))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Regular HID device detected, unloading...");
        return STATUS_NOT_SUPPORTED;
    }

#pragma endregion

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

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Compatible device detected, initializing...");

    //
    // Initialize the I/O Package and any Queues
    //
    status = XnaGuardianQueueInitialize(device);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "XnaGuardianQueueInitialize failed with status %!STATUS!", status);
        return status;
    }

    if (pDeviceContext->IsHidUsbDevice)
    {
        //
        // Initialize USB request queues.
        // 
        status = UpperUsbInterruptRequestsQueueInitialize(device);

        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "UpperUsbInterruptRequestsQueueInitialize failed with status %!STATUS!", status);
            return status;
        }
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
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfCollectionAdd failed with status code %!STATUS!", status);
    }
    WdfWaitLockRelease(FilterDeviceCollectionLock);

    //
    // Create a control device
    //
    status = FilterCreateControlDevice(device);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "FilterCreateControlDevice failed with status %!STATUS!",
            status);
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

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");

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
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "Removing HID USB device from collection");

        WdfWaitLockAcquire(HidUsbDeviceCollectionLock, NULL);
        WdfCollectionRemove(HidUsbDeviceCollection, Device);
        WdfWaitLockRelease(HidUsbDeviceCollectionLock);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit");
}
#pragma warning(pop) // enable 28118 again

