#include "busenum.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusIface_PlugInTarget)
#pragma alloc_text(PAGE, BufIface_UnplugTarget)
#pragma alloc_text(PAGE, BufIface_XusbSubmitReport)
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
    return Bus_UnPlugDevice(Context, SerialNo);
}

NTSTATUS BufIface_XusbSubmitReport(IN PVOID Context, IN ULONG SerialNo, IN PXUSB_SUBMIT_REPORT Report)
{
    return Bus_XusbSubmitReport(Context, SerialNo, Report, TRUE);
}

