#include "busenum.h"
#include <wdmguid.h>
#include <usb.h>
#include <usbioctl.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, Bus_EvtDeviceAdd)
#pragma alloc_text (PAGE, Bus_EvtIoDeviceControl)
#pragma alloc_text (PAGE, Bus_EvtIoDefault)
#pragma alloc_text (PAGE, Bus_PlugInDevice)
#pragma alloc_text (PAGE, Bus_UnPlugDevice)
#pragma alloc_text (PAGE, Bus_EjectDevice)
#pragma alloc_text (PAGE, Bus_XusbSubmitReport)
#pragma alloc_text (PAGE, Bus_XusbQueueNotification)
#endif

///-------------------------------------------------------------------------------------------------
/// <summary>	Driver entry routine. </summary>
///
/// <remarks>
/// DriverEntry is the first routine called after a driver is loaded, and is responsible for
/// initializing the driver.
/// </remarks>
///
/// <param name="DriverObject">	The driver object. </param>
/// <param name="RegistryPath">	Full pathname of the registry file. </param>
///
/// <returns>
/// If the routine succeeds, it must return STATUS_SUCCESS. Otherwise, it must return one of the
/// error status values defined in Ntstatus.h.
/// </returns>
///-------------------------------------------------------------------------------------------------
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDFDRIVER driver;

    KdPrint(("Virtual Gamepad Emulation Bus Driver [built: %s %s]\n", __DATE__, __TIME__));

    WDF_DRIVER_CONFIG_INIT(&config, Bus_EvtDeviceAdd);

    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, &driver);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfDriverCreate failed with status 0x%x\n", status));
    }

    return status;
}

NTSTATUS Bus_EvtDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
{
    WDF_CHILD_LIST_CONFIG config;
    NTSTATUS status;
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    PNP_BUS_INFORMATION busInfo;
    //PFDO_DEVICE_DATA           deviceData;
    WDFQUEUE queue;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    KdPrint(("Bus_EvtDeviceAdd: 0x%p\n", Driver));

    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_BUS_EXTENDER);
    // TODO: necessary?
    WdfDeviceInitSetExclusive(DeviceInit, TRUE);

    // Prepare child list
    {
        WDF_CHILD_LIST_CONFIG_INIT(&config, sizeof(PDO_IDENTIFICATION_DESCRIPTION), Bus_EvtDeviceListCreatePdo);

        config.EvtChildListIdentificationDescriptionCompare = Bus_EvtChildListIdentificationDescriptionCompare;

        WdfFdoInitSetDefaultChildListConfig(DeviceInit, &config, WDF_NO_OBJECT_ATTRIBUTES);
    }

    // Create FDO
    {
        status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("Error creating device 0x%x\n", status));
            return status;
        }
    }

    // Create default I/O queue for FDO
    {
        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

        queueConfig.EvtIoDeviceControl = Bus_EvtIoDeviceControl;
        queueConfig.EvtIoDefault = Bus_EvtIoDefault;

        __analysis_assume(queueConfig.EvtIoStop != 0);
        status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
        __analysis_assume(queueConfig.EvtIoStop == 0);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfIoQueueCreate failed status 0x%x\n", status));
            return status;
        }
    }

    // Expose FDO interface
    {
        status = WdfDeviceCreateDeviceInterface(device, &GUID_DEVINTERFACE_BUSENUM_VIGEM, NULL);

        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    // TODO: required?
    busInfo.BusTypeGuid = GUID_BUS_TYPE_USB;
    busInfo.LegacyBusType = PNPBus;
    busInfo.BusNumber = 0;

    WdfDeviceSetBusInformationForChildren(device, &busInfo);

    return status;
}

///-------------------------------------------------------------------------------------------------
/// <summary>A driver's EvtIoDeviceControl event callback function processes a specified device
/// I/O control request.</summary>
///
/// <remarks>Benjamin, 22.05.2016.</remarks>
///
/// <param name="Queue">                A handle to the framework queue object that is associated
///                                     with the I/O request.</param>
/// <param name="Request">           A handle to a framework request object.</param>
/// <param name="OutputBufferLength">   The length, in bytes, of the request's output buffer, if
///                                     an output buffer is available.</param>
/// <param name="InputBufferLength">    The length, in bytes, of the request's input buffer, if
///                                     an input buffer is available.</param>
/// <param name="IoControlCode">        The driver-defined or system-defined I/O control code
///                                     (IOCTL) that is associated with the request.</param>
///
/// <returns>None.</returns>
///-------------------------------------------------------------------------------------------------
VOID Bus_EvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength, IN size_t InputBufferLength, IN ULONG IoControlCode)
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    WDFDEVICE hDevice;
    size_t length = 0;
    PBUSENUM_PLUGIN_HARDWARE plugIn = NULL;
    PBUSENUM_UNPLUG_HARDWARE unPlug = NULL;
    PBUSENUM_EJECT_HARDWARE eject = NULL;
    PXUSB_SUBMIT_REPORT xusbSubmit = NULL;
    PXUSB_REQUEST_NOTIFICATION xusbNotify = NULL;

    UNREFERENCED_PARAMETER(OutputBufferLength);

    PAGED_CODE();

    hDevice = WdfIoQueueGetDevice(Queue);

    KdPrint(("Bus_EvtIoDeviceControl: 0x%p\n", hDevice));

    switch (IoControlCode)
    {
    case IOCTL_BUSENUM_PLUGIN_HARDWARE:

        KdPrint(("IOCTL_BUSENUM_PLUGIN_HARDWARE\n"));

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_PLUGIN_HARDWARE), (PVOID)&plugIn, &length);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if ((sizeof(BUSENUM_PLUGIN_HARDWARE) == plugIn->Size) && (length == InputBufferLength))
        {
            if (plugIn->SerialNo == 0)
            {
                status = STATUS_INVALID_PARAMETER;
                break;
            }

            status = Bus_PlugInDevice(hDevice, plugIn->SerialNo, plugIn->TargetType);
        }

        break;

    case IOCTL_BUSENUM_UNPLUG_HARDWARE:

        KdPrint(("IOCTL_BUSENUM_UNPLUG_HARDWARE\n"));

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_UNPLUG_HARDWARE), (PVOID)&unPlug, &length);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if ((sizeof(BUSENUM_UNPLUG_HARDWARE) == unPlug->Size) && (length == InputBufferLength))
        {
            status = Bus_UnPlugDevice(hDevice, unPlug->SerialNo);
        }

        break;

    case IOCTL_BUSENUM_EJECT_HARDWARE:

        KdPrint(("IOCTL_BUSENUM_EJECT_HARDWARE\n"));

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_EJECT_HARDWARE), (PVOID)&eject, &length);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if (eject->Size == InputBufferLength)
        {
            status = Bus_EjectDevice(hDevice, eject->SerialNo);
        }

        break;

    case IOCTL_XUSB_SUBMIT_REPORT:

        KdPrint(("IOCTL_XUSB_SUBMIT_REPORT\n"));

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(XUSB_SUBMIT_REPORT), (PVOID)&xusbSubmit, &length);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if ((sizeof(XUSB_SUBMIT_REPORT) == xusbSubmit->Size) && (length == InputBufferLength))
        {
            if (xusbSubmit->SerialNo == 0)
            {
                status = STATUS_INVALID_PARAMETER;
                break;
            }

            status = Bus_XusbSubmitReport(hDevice, xusbSubmit->SerialNo, xusbSubmit);
        }

        break;

    case IOCTL_XUSB_REQUEST_NOTIFICATION:

        KdPrint(("IOCTL_XUSB_REQUEST_NOTIFICATION\n"));

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(XUSB_REQUEST_NOTIFICATION), (PVOID)&xusbNotify, &length);

        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if ((sizeof(XUSB_REQUEST_NOTIFICATION) == xusbNotify->Size) && (length == InputBufferLength))
        {
            if (xusbNotify->SerialNo == 0)
            {
                status = STATUS_INVALID_PARAMETER;
                break;
            }

            status = Bus_XusbQueueNotification(hDevice, xusbNotify->SerialNo, Request);
        }

        break;

    default:
        KdPrint(("UNKNOWN IOCTL CODE 0x%x\n", IoControlCode));
        break; // default status is STATUS_INVALID_PARAMETER
    }

    WdfRequestCompleteWithInformation(Request, status, length);
}

VOID Bus_EvtIoDefault(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request
)
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(Request);

    KdPrint(("Bus_EvtIoDefault called\n"));
}

NTSTATUS Bus_PlugInDevice(_In_ WDFDEVICE Device, _In_ ULONG SerialNo, _In_ VIGEM_TARGET_TYPE TargetType)
{
    PDO_IDENTIFICATION_DESCRIPTION description;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Initialize the description with the information about the newly
    // plugged in device.
    //
    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

    description.SerialNo = SerialNo;
    description.TargetType = TargetType;

    status = WdfChildListAddOrUpdateChildDescriptionAsPresent(WdfFdoGetDefaultChildList(Device), &description.Header, NULL);

    if (status == STATUS_OBJECT_NAME_EXISTS)
    {
        //
        // The description is already present in the list, the serial number is
        // not unique, return error.
        //
        status = STATUS_INVALID_PARAMETER;
    }

    KdPrint(("Bus_PlugInDevice exiting with 0x%x\n", status));

    return status;
}

NTSTATUS Bus_UnPlugDevice(WDFDEVICE Device, ULONG SerialNo)
{
    NTSTATUS status;
    WDFCHILDLIST list;

    PAGED_CODE();

    list = WdfFdoGetDefaultChildList(Device);

    if (0 == SerialNo)
    {
        //
        // Unplug everybody.  We do this by starting a scan and then not reporting
        // any children upon its completion
        //
        status = STATUS_SUCCESS;

        WdfChildListBeginScan(list);

        WdfChildListEndScan(list);
    }
    else
    {
        PDO_IDENTIFICATION_DESCRIPTION description;

        WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

        description.SerialNo = SerialNo;

        status = WdfChildListUpdateChildDescriptionAsMissing(list, &description.Header);

        if (status == STATUS_NO_SUCH_DEVICE)
        {
            //
            // serial number didn't exist. Remap it to a status that user
            // application can understand when it gets translated to win32
            // error code.
            //
            status = STATUS_INVALID_PARAMETER;
        }
    }

    return status;
}

NTSTATUS Bus_XusbSubmitReport(WDFDEVICE Device, ULONG SerialNo, PXUSB_SUBMIT_REPORT Report)
{
    NTSTATUS status = STATUS_SUCCESS;
    WDFCHILDLIST list;
    WDF_CHILD_RETRIEVE_INFO info;
    WDFDEVICE  hChild;
    PPDO_DEVICE_DATA pdoData;
    PXUSB_DEVICE_DATA xusbData;
    WDFREQUEST usbRequest;
    PIRP pendingIrp;
    PIO_STACK_LOCATION irpStack;
    BOOLEAN changed;


    PAGED_CODE();

    KdPrint(("Entered Bus_XusbSubmitReport\n"));

    list = WdfFdoGetDefaultChildList(Device);

    PDO_IDENTIFICATION_DESCRIPTION description;

    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

    description.SerialNo = SerialNo;

    WDF_CHILD_RETRIEVE_INFO_INIT(&info, &description.Header);

    hChild = WdfChildListRetrievePdo(list, &info);

    if (hChild == NULL)
    {
        KdPrint(("Bus_XusbSubmitReport: PDO with serial %d not found\n", SerialNo));
        return STATUS_NO_SUCH_DEVICE;
    }

    pdoData = PdoGetData(hChild);
    if (pdoData == NULL)
    {
        KdPrint(("Bus_XusbSubmitReport: PDO context not found\n"));
        return STATUS_INVALID_PARAMETER;
    }

    xusbData = XusbGetData(hChild);
    if (xusbData == NULL)
    {
        KdPrint(("Bus_XusbSubmitReport: XUSB context not found\n"));
        return STATUS_INVALID_PARAMETER;
    }

    //if (pdoData->OwnerProcessId != CURRENT_PROCESS_ID())
    //{
    //    KdPrint(("Bus_XusbSubmitReport: PID mismatch\n"));
    //    return STATUS_ACCESS_DENIED;
    //}

    changed = (RtlCompareMemory(xusbData->Report + 2, &Report->Report, sizeof(XUSB_REPORT)) != sizeof(XUSB_REPORT));

    if (changed)
    {
        KdPrint(("Bus_XusbSubmitReport: received new report\n"));

        status = WdfIoQueueRetrieveNextRequest(xusbData->PendingUsbRequests, &usbRequest);

        if (NT_SUCCESS(status))
        {
            KdPrint(("Bus_XusbSubmitReport: pending IRP found\n"));

            pendingIrp = WdfRequestWdmGetIrp(usbRequest);
            irpStack = IoGetCurrentIrpStackLocation(pendingIrp);

            // get USB request block
            PURB urb = (PURB)irpStack->Parameters.Others.Argument1;

            // get transfer buffer
            PUCHAR Buffer = (PUCHAR)urb->UrbBulkOrInterruptTransfer.TransferBuffer;
            // set buffer length to report size
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength = XUSB_REPORT_SIZE;

            xusbData->Report[1] = 0x14;

            RtlCopyBytes(xusbData->Report + 2, &Report->Report, sizeof(XUSB_REPORT));
            RtlCopyBytes(Buffer, xusbData->Report, XUSB_REPORT_SIZE);

            WdfRequestComplete(usbRequest, status);
        }
    }

    return status;
}

//
// Experimental
// 
NTSTATUS Bus_XusbQueueNotification(WDFDEVICE Device, ULONG SerialNo, WDFREQUEST Request)
{
    NTSTATUS status;
    WDFCHILDLIST list;
    WDF_CHILD_RETRIEVE_INFO  info;
    WDFDEVICE  hChild;
    PXUSB_DEVICE_DATA xusbData;

    PAGED_CODE();

    list = WdfFdoGetDefaultChildList(Device);

    PDO_IDENTIFICATION_DESCRIPTION description;

    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

    description.SerialNo = SerialNo;

    WDF_CHILD_RETRIEVE_INFO_INIT(&info, &description.Header);

    hChild = WdfChildListRetrievePdo(list, &info);

    if (hChild == NULL)
    {
        KdPrint(("Bus_XusbQueueNotification: PDO with serial %d not found\n", SerialNo));
        return STATUS_NO_SUCH_DEVICE;
    }

    xusbData = XusbGetData(hChild);

    status = WdfRequestForwardToIoQueue(Request, xusbData->PendingNotificationRequests);

    return (NT_SUCCESS(status)) ? STATUS_PENDING : STATUS_UNSUCCESSFUL;
}

NTSTATUS Bus_EjectDevice(WDFDEVICE Device, ULONG SerialNo)
{
    WDFDEVICE hChild;
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    WDFCHILDLIST list;

    PAGED_CODE();

    list = WdfFdoGetDefaultChildList(Device);

    //
    // A zero serial number means eject all children
    //
    if (0 == SerialNo)
    {
        WDF_CHILD_LIST_ITERATOR iterator;

        WDF_CHILD_LIST_ITERATOR_INIT(&iterator, WdfRetrievePresentChildren);

        WdfChildListBeginIteration(list, &iterator);

        for (; ;)
        {
            WDF_CHILD_RETRIEVE_INFO childInfo;
            PDO_IDENTIFICATION_DESCRIPTION description;
            BOOLEAN ret;

            //
            // Init the structures.
            //
            WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));
            WDF_CHILD_RETRIEVE_INFO_INIT(&childInfo, &description.Header);

            WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

            //
            // Get the device identification description
            //
            status = WdfChildListRetrieveNextDevice(list, &iterator, &hChild, &childInfo);

            if (!NT_SUCCESS(status) || status == STATUS_NO_MORE_ENTRIES)
            {
                break;
            }

            ASSERT(childInfo.Status == WdfChildListRetrieveDeviceSuccess);

            //
            // Use that description to request an eject.
            //
            ret = WdfChildListRequestChildEject(list, &description.Header);

            if (!ret)
            {
                WDFVERIFY(ret);
            }
        }

        WdfChildListEndIteration(list, &iterator);

        if (status == STATUS_NO_MORE_ENTRIES)
        {
            status = STATUS_SUCCESS;
        }
    }
    else
    {
        PDO_IDENTIFICATION_DESCRIPTION description;

        WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

        description.SerialNo = SerialNo;

        if (WdfChildListRequestChildEject(list, &description.Header))
        {
            status = STATUS_SUCCESS;
        }
    }

    return status;
}

