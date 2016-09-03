#include "busenum.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusIface_PlugInTarget)
#pragma alloc_text(PAGE, BufIface_UnplugTarget)
#pragma alloc_text(PAGE, BufIface_XusbSubmitReport)
#endif


NTSTATUS BusIface_PlugInTarget(IN PVOID Context, IN ULONG SerialNo, IN VIGEM_TARGET_TYPE TargetType)
{
    return Bus_PlugInDevice(Context, SerialNo, TargetType);
}

NTSTATUS BufIface_UnplugTarget(IN PVOID Context, IN ULONG SerialNo)
{
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(SerialNo);

    return STATUS_SUCCESS;
}

NTSTATUS BufIface_XusbSubmitReport(IN PVOID Context, IN ULONG SerialNo, IN PXUSB_SUBMIT_REPORT Report)
{
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(SerialNo);
    UNREFERENCED_PARAMETER(Report);

    return STATUS_SUCCESS;
}
