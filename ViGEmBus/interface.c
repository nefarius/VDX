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


#include "busenum.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusIface_PlugInTarget)
#pragma alloc_text(PAGE, BufIface_UnplugTarget)
#pragma alloc_text(PAGE, BufIface_XusbSubmitReport)
#pragma alloc_text(PAGE, BusInterfaceReference)
#pragma alloc_text(PAGE, BusInterfaceDereference)
#endif


NTSTATUS BusIface_PlugInTarget(
    IN PVOID Context,
    IN ULONG SerialNo,
    IN VIGEM_TARGET_TYPE TargetType,
    IN USHORT VendorId,
    IN USHORT ProductId)
{
    return Bus_PlugInDevice(Context, SerialNo, TargetType, VendorId, ProductId, TRUE);
}

NTSTATUS BufIface_UnplugTarget(IN PVOID Context, IN ULONG SerialNo)
{
    return Bus_UnPlugDevice(Context, SerialNo, TRUE);
}

NTSTATUS BufIface_XusbSubmitReport(IN PVOID Context, IN ULONG SerialNo, IN PXUSB_SUBMIT_REPORT Report)
{
    return Bus_XusbSubmitReport(Context, SerialNo, Report, TRUE);
}

VOID BusInterfaceReference(
    _In_ PVOID Context
)
{
    PFDO_DEVICE_DATA    pFdoData;

    KdPrint((DRIVERNAME "BusInterfaceReference called\n"));

    PAGED_CODE();

    pFdoData = FdoGetData((WDFDEVICE)Context);

    if (!pFdoData)
    {
        KdPrint((DRIVERNAME "FDO context not found\n"));
        return;
    }

    pFdoData->InterfaceReferenceCounter++;
}

VOID BusInterfaceDereference(
    _In_ PVOID Context
)
{
    PFDO_DEVICE_DATA    pFdoData;

    KdPrint((DRIVERNAME "BusInterfaceDereference called\n"));

    PAGED_CODE();

    pFdoData = FdoGetData((WDFDEVICE)Context);

    if (!pFdoData)
    {
        KdPrint((DRIVERNAME "FDO context not found\n"));
        return;
    }

    pFdoData->InterfaceReferenceCounter--;
}

