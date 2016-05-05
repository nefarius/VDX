#include "busenum.h"
#include <wdmguid.h>
#include <usb.h>
#include <usbioctl.h>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, Bus_EvtDeviceAdd)
#pragma alloc_text (PAGE, Bus_EvtIoDeviceControl)
#pragma alloc_text (PAGE, Bus_EvtIoInternalDeviceControl)
#pragma alloc_text (PAGE, Bus_PlugInDevice)
#pragma alloc_text (PAGE, Bus_UnPlugDevice)
#pragma alloc_text (PAGE, Bus_EjectDevice)
#endif

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDFDRIVER driver;

    KdPrint(("Virtual Gamepad Emulation Bus Driver Entry\n"));

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
    WdfDeviceInitSetExclusive(DeviceInit, TRUE);

    WDF_CHILD_LIST_CONFIG_INIT(&config, sizeof(PDO_IDENTIFICATION_DESCRIPTION), Bus_EvtDeviceListCreatePdo);

    WdfFdoInitSetDefaultChildListConfig(DeviceInit, &config, WDF_NO_OBJECT_ATTRIBUTES);

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Error creating device 0x%x\n", status));
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = Bus_EvtIoDeviceControl;
    queueConfig.EvtIoInternalDeviceControl = Bus_EvtIoInternalDeviceControl;

    __analysis_assume(queueConfig.EvtIoStop != 0);
    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    __analysis_assume(queueConfig.EvtIoStop == 0);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed status 0x%x\n", status));
        return status;
    }

    status = WdfDeviceCreateDeviceInterface(device, &GUID_DEVINTERFACE_BUSENUM_VIGEM, NULL);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // TODO: required?
    busInfo.BusTypeGuid = GUID_BUS_TYPE_INTERNAL;
    busInfo.LegacyBusType = PNPBus;
    busInfo.BusNumber = 0;

    WdfDeviceSetBusInformationForChildren(device, &busInfo);

    return status;
}

VOID Bus_EvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t OutputBufferLength, IN size_t InputBufferLength, IN ULONG IoControlCode)
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    WDFDEVICE hDevice;
    size_t length = 0;
    PBUSENUM_PLUGIN_HARDWARE plugIn = NULL;
    PBUSENUM_UNPLUG_HARDWARE unPlug = NULL;
    PBUSENUM_EJECT_HARDWARE eject = NULL;


    UNREFERENCED_PARAMETER(OutputBufferLength);

    PAGED_CODE();

    hDevice = WdfIoQueueGetDevice(Queue);

    KdPrint(("Bus_EvtIoDeviceControl: 0x%p\n", hDevice));

    switch (IoControlCode)
    {
    case IOCTL_BUSENUM_PLUGIN_HARDWARE:

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_PLUGIN_HARDWARE), &plugIn, &length);

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

            status = Bus_PlugInDevice(hDevice, plugIn->SerialNo);
        }

        break;

    case IOCTL_BUSENUM_UNPLUG_HARDWARE:

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_UNPLUG_HARDWARE), &unPlug, &length);

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

        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_EJECT_HARDWARE), &eject, &length);

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

    default:
        break; // default status is STATUS_INVALID_PARAMETER
    }

    WdfRequestCompleteWithInformation(Request, status, length);
}

NTSTATUS Bus_PlugInDevice(_In_ WDFDEVICE Device, _In_ ULONG SerialNo)
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

    status = WdfChildListAddOrUpdateChildDescriptionAsPresent(WdfFdoGetDefaultChildList(Device), &description.Header, NULL);

    if (status == STATUS_OBJECT_NAME_EXISTS)
    {
        //
        // The description is already present in the list, the serial number is
        // not unique, return error.
        //
        status = STATUS_INVALID_PARAMETER;
    }

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

VOID Bus_EvtIoInternalDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
)
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(Request);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    NTSTATUS status = STATUS_INVALID_PARAMETER;
    WDFDEVICE hDevice;
    PIRP irp;
    PIO_STACK_LOCATION irpSp;
    PURB urb;

    PAGED_CODE();

    hDevice = WdfIoQueueGetDevice(Queue);

    KdPrint(("Bus_EvtIoInternalDeviceControl: 0x%p\n", hDevice));

    irp = WdfRequestWdmGetIrp(Request);

    irpSp = IoGetCurrentIrpStackLocation(irp);

    urb = irpSp->Parameters.Others.Argument1;

    switch (IoControlCode)
    {
    case IOCTL_INTERNAL_USB_SUBMIT_URB:

        switch (urb->UrbHeader.Function)
        {
        case URB_FUNCTION_CONTROL_TRANSFER:

            status = STATUS_UNSUCCESSFUL;
            break;

        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
            break;

        case URB_FUNCTION_SELECT_CONFIGURATION:
            break;

        case URB_FUNCTION_SELECT_INTERFACE:
            break;

        case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:

            switch (urb->UrbControlDescriptorRequest.DescriptorType)
            {
            case USB_DEVICE_DESCRIPTOR_TYPE:
                break;

            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                break;

            case USB_STRING_DESCRIPTOR_TYPE:
                break;

            case USB_INTERFACE_DESCRIPTOR_TYPE:
                break;

            case USB_ENDPOINT_DESCRIPTOR_TYPE:
                break;

            default:
                break;
            }
            break;
        }
        break;

    case IOCTL_INTERNAL_USB_GET_PORT_STATUS:
        break;

    case IOCTL_INTERNAL_USB_RESET_PORT:
        break;
    }
}

