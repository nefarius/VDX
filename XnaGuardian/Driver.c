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
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, XnaGuardianEvtDeviceAdd)
#pragma alloc_text (PAGE, XnaGuardianEvtDriverContextCleanup)
#endif


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    KdPrint((DRIVERNAME "Loading XnaGuardian [built: %s %s]\n", __DATE__, __TIME__));

    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = XnaGuardianEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
        XnaGuardianEvtDeviceAdd
    );

    status = WdfDriverCreate(DriverObject,
        RegistryPath,
        &attributes,
        &config,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "WdfDriverCreate failed 0x%X", status));
        WPP_CLEANUP(DriverObject);
        return status;
    }

    status = WdfCollectionCreate(WDF_NO_OBJECT_ATTRIBUTES,
        &FilterDeviceCollection);
    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfCollectionCreate failed with status 0x%x\n", status));
        return status;
    }

    status = WdfCollectionCreate(WDF_NO_OBJECT_ATTRIBUTES,
        &HidUsbDeviceCollection);
    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfCollectionCreate failed with status 0x%x\n", status));
        return status;
    }

    //
    // The wait-lock object has the driver object as a default parent.
    //

    status = WdfWaitLockCreate(WDF_NO_OBJECT_ATTRIBUTES,
        &FilterDeviceCollectionLock);
    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfWaitLockCreate failed with status 0x%x\n", status));
        return status;
    }

    status = WdfWaitLockCreate(WDF_NO_OBJECT_ATTRIBUTES,
        &HidUsbDeviceCollectionLock);
    if (!NT_SUCCESS(status))
    {
        KdPrint((DRIVERNAME "WdfWaitLockCreate failed with status 0x%x\n", status));
        return status;
    }

    KdPrint((DRIVERNAME "XnaGuardian loaded: 0x%X\n", status));

    return status;
}

NTSTATUS
XnaGuardianEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = XnaGuardianCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
XnaGuardianEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));

}
