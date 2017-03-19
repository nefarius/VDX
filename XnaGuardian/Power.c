#include "Driver.h"


#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, XnaGuardianEvtDevicePrepareHardware)
#pragma alloc_text(PAGE, XnaGuardianEvtDeviceD0Entry)
#pragma alloc_text(PAGE, XnaGuardianEvtDeviceD0Exit)
#endif


_Use_decl_annotations_
NTSTATUS
XnaGuardianEvtDevicePrepareHardware(
    WDFDEVICE  Device,
    WDFCMRESLIST  ResourcesRaw,
    WDFCMRESLIST  ResourcesTranslated
)
{
    NTSTATUS                                status = STATUS_SUCCESS;
    PDEVICE_CONTEXT                         pDeviceContext;
    UCHAR                                   numberConfiguredPipes;
    UCHAR                                   index;
    WDF_USB_PIPE_INFORMATION                pipeInfo;
    WDFUSBPIPE                              pipe;
    WDF_USB_CONTINUOUS_READER_CONFIG        contReaderConfig;
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS     configParams;

    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE();

    KdPrint((DRIVERNAME "XnaGuardianEvtDevicePrepareHardware called\n"));

    pDeviceContext = DeviceGetContext(Device);

    // Create USB framework object
    if (pDeviceContext->UsbDevice == NULL && pDeviceContext->IsHidUsbDevice)
    {
        status = WdfUsbTargetDeviceCreate(Device, WDF_NO_OBJECT_ATTRIBUTES, &pDeviceContext->UsbDevice);
        if (!NT_SUCCESS(status))
        {
            KdPrint((DRIVERNAME "WdfUsbTargetDeviceCreate failed with status 0x%X\n", status));
            return status;
        }

        WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

        status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->UsbDevice,
            WDF_NO_OBJECT_ATTRIBUTES,
            &configParams);
        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "WdfUsbTargetDeviceSelectConfig failed with status 0x%X\n", status));
            return status;
        }

        pDeviceContext->UsbInterface = configParams.Types.SingleInterface.ConfiguredUsbInterface;
        numberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;

        //
        // Get pipe handles
        //
        for (index = 0; index < numberConfiguredPipes; index++) {

            WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

            pipe = WdfUsbInterfaceGetConfiguredPipe(
                pDeviceContext->UsbInterface,
                index, //PipeIndex,
                &pipeInfo
            );
            //
            // Tell the framework that it's okay to read less than
            // MaximumPacketSize
            //
            WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

            if (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType) {
                KdPrint((DRIVERNAME "Interrupt Pipe is 0x%p\n", pipe));
                pDeviceContext->InterruptPipe = pipe;
            }
        }

        WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&contReaderConfig,
            XnaGuardianEvtUsbTargetPipeReadComplete,
            pDeviceContext, // Context
            20); // TransferLength
                 //
                 // Reader requests are not posted to the target automatically.
                 // Driver must explictly call WdfIoTargetStart to kick start the
                 // reader. In this sample, it's done in D0Entry.
                 // By defaut, framework queues two requests to the target
                 // endpoint. Driver can configure up to 10 requests with CONFIG macro.
                 //
        status = WdfUsbTargetPipeConfigContinuousReader(pDeviceContext->InterruptPipe,
            &contReaderConfig);

        if (!NT_SUCCESS(status)) {
            KdPrint((DRIVERNAME "WdfUsbTargetPipeConfigContinuousReader failed with status %x\n", status));
            return status;
        }
    }

    return status;
}

//
// Gets called when the device reaches D0 state.
// 
NTSTATUS XnaGuardianEvtDeviceD0Entry(
    _In_ WDFDEVICE              Device,
    _In_ WDF_POWER_DEVICE_STATE PreviousState
)
{
    NTSTATUS            status;
    PDEVICE_CONTEXT     pDeviceContext;

    UNREFERENCED_PARAMETER(PreviousState);

    PAGED_CODE();

    KdPrint((DRIVERNAME "XnaGuardianEvtDeviceD0Entry called\n"));

    pDeviceContext = DeviceGetContext(Device);

    if (!pDeviceContext->IsHidUsbDevice) return STATUS_SUCCESS;

    //
    // Since continuous reader is configured for this interrupt-pipe, we must explicitly start
    // the I/O target to get the framework to post read requests.
    //
    status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe));
    if (!NT_SUCCESS(status)) {
        KdPrint((DRIVERNAME "Failed to start interrupt pipe: 0x%X\n", status));
    }

    return status;
}

//
// Gets called when the device leaves D0 state.
// 
NTSTATUS XnaGuardianEvtDeviceD0Exit(
    _In_ WDFDEVICE              Device,
    _In_ WDF_POWER_DEVICE_STATE TargetState
)
{
    PDEVICE_CONTEXT     pDeviceContext;

    UNREFERENCED_PARAMETER(TargetState);

    PAGED_CODE();

    pDeviceContext = DeviceGetContext(Device);

    if (pDeviceContext->IsHidUsbDevice)
    {
        WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe), WdfIoTargetCancelSentIo);
    }

    return STATUS_SUCCESS;
}

