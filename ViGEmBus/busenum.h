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


#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <initguid.h>
#include "driver.h"
#include "public.h"
#include <usb.h>
#include <usbbusif.h>

#pragma region GUID definitions

// 
// For children emulating XUSB devices, the following dummy interfaces 
// have to be exposed by the PDO or else the child devices won't start
// 

// {70211B0E-0AFB-47DB-AFC1-410BF842497A}
DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_0,
    0x70211B0E, 0x0AFB, 0x47DB, 0xAF, 0xC1, 0x41, 0x0B, 0xF8, 0x42, 0x49, 0x7A);

// {B38290E5-3CD0-4F9D-9937-F5FE2B44D47A}
DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_1,
    0xB38290E5, 0x3CD0, 0x4F9D, 0x99, 0x37, 0xF5, 0xFE, 0x2B, 0x44, 0xD4, 0x7A);

// {2AEB0243-6A6E-486B-82FC-D815F6B97006}
DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_2,
    0x2AEB0243, 0x6A6E, 0x486B, 0x82, 0xFC, 0xD8, 0x15, 0xF6, 0xB9, 0x70, 0x06);


// 
// For children emulating XGIP devices, the following dummy interfaces 
// have to be exposed by the PDO or else the child devices won't start
// 

// {70211B0E-0AFB-47DB-AFC1-410BF842497A}
DEFINE_GUID(GUID_DEVINTERFACE_XGIP_UNKNOWN_0,
    0x70211B0E, 0x0AFB, 0x47DB, 0xAF, 0xC1, 0x41, 0x0B, 0xF8, 0x42, 0x49, 0x7A);

// {B38290E5-3CD0-4F9D-9937-F5FE2B44D47A}
DEFINE_GUID(GUID_DEVINTERFACE_XGIP_UNKNOWN_1,
    0xB38290E5, 0x3CD0, 0x4F9D, 0x99, 0x37, 0xF5, 0xFE, 0x2B, 0x44, 0xD4, 0x7A);

// {2AEB0243-6A6E-486B-82FC-D815F6B97006}
DEFINE_GUID(GUID_DEVINTERFACE_XGIP_UNKNOWN_2,
    0x2AEB0243, 0x6A6E, 0x486B, 0x82, 0xFC, 0xD8, 0x15, 0xF6, 0xB9, 0x70, 0x06);

// {DC7A8E51-49B3-4A3A-9E81-625205E7D729}
DEFINE_GUID(GUID_DEVINTERFACE_XGIP_UNKNOWN_3,
    0xDC7A8E51, 0x49B3, 0x4A3A, 0x9E, 0x81, 0x62, 0x52, 0x05, 0xE7, 0xD7, 0x29);

// {DEEE98EA-C0A1-42C3-9738-A04606C84E93}
DEFINE_GUID(GUID_DEVINTERFACE_XGIP_UNKNOWN_4,
    0xDEEE98EA, 0xC0A1, 0x42C3, 0x97, 0x38, 0xA0, 0x46, 0x06, 0xC8, 0x4E, 0x93);

#pragma endregion

#pragma once


#pragma region Macros

#define MAX_INSTANCE_ID_LEN             80
#define XUSB_DESCRIPTOR_SIZE	        0x0099
#define DS4_DESCRIPTOR_SIZE	            0x0029
#define DS4_CONFIGURATION_SIZE          0x0070
#define DS4_HID_REPORT_DESCRIPTOR_SIZE  0x01D3
#define HID_LANGUAGE_ID_LENGTH          0x04
#define DS4_MANUFACTURER_NAME_LENGTH    0x38
#define DS4_PRODUCT_NAME_LENGTH         0x28
#define DS4_OUTPUT_BUFFER_OFFSET        0x04
#define DS4_OUTPUT_BUFFER_LENGTH        0x05

#if defined(_X86_)
#define XUSB_CONFIGURATION_SIZE              0x00E4
#else
#define XUSB_CONFIGURATION_SIZE              0x0130
#endif

#define XGIP_DESCRIPTOR_SIZE	        0x0060
#define XGIP_CONFIGURATION_SIZE         0x88
#define XGIP_REPORT_SIZE                0x12

#define XUSB_REPORT_SIZE                20
#define XUSB_RUMBLE_SIZE                8
#define XUSB_LEDSET_SIZE                3
#define XUSB_LEDNUM_SIZE                1

#define DS4_REPORT_SIZE                 64
#define DS4_QUEUE_FLUSH_PERIOD          5

#define HID_REQUEST_GET_REPORT          0x01
#define HID_REQUEST_SET_REPORT          0x09
#define HID_REPORT_TYPE_FEATURE         0x03

#define VIGEM_POOL_TAG                  0x45476956 // "EGiV"

#pragma endregion

#pragma region Helpers

//
// Returns the current caller process id.
// 
#define CURRENT_PROCESS_ID() ((DWORD)((DWORD_PTR)PsGetCurrentProcessId() & 0xFFFFFFFF))

#define IS_OWNER(_pdo_) (_pdo_->OwnerProcessId == CURRENT_PROCESS_ID())

//
// Extracts the HID Report ID from the supplied class request.
//
#define HID_GET_REPORT_ID(_req_) ((_req_->Value) & 0xFF)

//
// Extracts the HID Report type from the supplied class request.
//
#define HID_GET_REPORT_TYPE(_req_) ((_req_->Value >> 8) & 0xFF)

#pragma endregion

//
// Used to identify children in the device list of the bus.
// 
typedef struct _PDO_IDENTIFICATION_DESCRIPTION
{
    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER Header; // should contain this header

    ULONG SerialNo;

    // 
    // PID of the process creating this PDO
    // 
    DWORD OwnerProcessId;

    //
    // Device type this PDO is emulating
    // 
    VIGEM_TARGET_TYPE TargetType;
} PDO_IDENTIFICATION_DESCRIPTION, *PPDO_IDENTIFICATION_DESCRIPTION;

//
// The PDO device-extension (context).
//
typedef struct _PDO_DEVICE_DATA
{
    //
    // Unique serial number of the device on the bus
    // 
    ULONG SerialNo;

    // 
    // PID of the process creating this PDO
    // 
    DWORD OwnerProcessId;

    //
    // Device type this PDO is emulating
    // 
    VIGEM_TARGET_TYPE TargetType;

} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDO_DEVICE_DATA, PdoGetData)

//
// XUSB-specific device context data.
// 
typedef struct _XUSB_DEVICE_DATA
{
    //
    // Rumble buffer
    //
    UCHAR Rumble[XUSB_RUMBLE_SIZE];

    //
    // LED number (represents XInput slot index)
    //
    UCHAR LedNumber;

    //
    // Report buffer
    //
    UCHAR Report[XUSB_REPORT_SIZE];

    //
    // Queue for incoming interrupt transfer
    //
    WDFQUEUE PendingUsbInRequests;

    //
    // Queue for inverted calls
    //
    WDFQUEUE PendingNotificationRequests;
} XUSB_DEVICE_DATA, *PXUSB_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(XUSB_DEVICE_DATA, XusbGetData)

//
// Represents a MAC address.
//
typedef struct _MAC_ADDRESS
{
    UCHAR Vendor0;
    UCHAR Vendor1;
    UCHAR Vendor2;
    UCHAR Nic0;
    UCHAR Nic1;
    UCHAR Nic2;
} MAC_ADDRESS, *PMAC_ADDRESS;

//
// DS4-specific device context data.
// 
typedef struct _DS4_DEVICE_DATA
{
    //
    // HID Input Report buffer
    //
    UCHAR Report[DS4_REPORT_SIZE];

    //
    // Output report cache
    //
    DS4_OUTPUT_REPORT OutputReport;

    //
    // Queue for incoming interrupt transfer
    //
    WDFQUEUE PendingUsbInRequests;

    //
    // Timer for dispatching interrupt transfer
    //
    WDFTIMER PendingUsbInRequestsTimer;

    //
    // Queue for inverted calls
    //
    WDFQUEUE PendingNotificationRequests;

    //
    // Auto-generated MAC address of the target device
    //
    MAC_ADDRESS TargetMacAddress;

    //
    // Default MAC address of the host (not used)
    //
    MAC_ADDRESS HostMacAddress;
} DS4_DEVICE_DATA, *PDS4_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DS4_DEVICE_DATA, Ds4GetData)

typedef struct _XGIP_DEVICE_DATA
{
    UCHAR Report[XGIP_REPORT_SIZE];

    //
    // Queue for incoming interrupt transfer
    //
    WDFQUEUE PendingUsbInRequests;

    //
    // Timer for dispatching interrupt transfer
    //
    WDFTIMER PendingUsbInRequestsTimer;

    //
    // Queue for inverted calls
    //
    WDFQUEUE PendingNotificationRequests;
} XGIP_DEVICE_DATA, *PXGIP_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(XGIP_DEVICE_DATA, XgipGetData)


#pragma region WDF callback prototypes

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;

EVT_WDF_FILE_CLEANUP  Bus_FileCleanup;

EVT_WDF_IO_QUEUE_IO_DEFAULT Bus_EvtIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Bus_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Bus_EvtIoInternalDeviceControl;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;

EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE Bus_EvtChildListIdentificationDescriptionCompare;

EVT_WDF_DEVICE_PREPARE_HARDWARE Bus_EvtDevicePrepareHardware;

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Pdo_EvtIoInternalDeviceControl;

EVT_WDF_TIMER Ds4_PendingUsbRequestsTimerFunc;
EVT_WDF_TIMER Xgip_PendingUsbRequestsTimerFunc;

EVT_WDFDEVICE_WDM_IRP_PREPROCESS Pdo_EvtDeviceWdmIrpPreprocess;

EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST Pdo_EvtDeviceProcessQueryInterfaceRequest;

#pragma endregion

#pragma region Bus enumeration-specific functions

NTSTATUS
Bus_PlugInDevice(
    _In_ WDFDEVICE Device,
    _In_ ULONG SerialNo,
    _In_ VIGEM_TARGET_TYPE TargetType
);

NTSTATUS
Bus_UnPlugDevice(
    WDFDEVICE Device,
    ULONG SerialNo
);

NTSTATUS
Bus_CreatePdo(
    _In_ WDFDEVICE Device,
    _In_ PWDFDEVICE_INIT ChildInit,
    _In_ PPDO_IDENTIFICATION_DESCRIPTION Description
);

NTSTATUS
Bus_XusbSubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PXUSB_SUBMIT_REPORT Report
);

NTSTATUS
Bus_QueueNotification(
    WDFDEVICE Device,
    ULONG SerialNo,
    WDFREQUEST Request
);

NTSTATUS
Bus_Ds4SubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PDS4_SUBMIT_REPORT Report
);

NTSTATUS
Bus_XgipSubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PXGIP_SUBMIT_REPORT Report
);

NTSTATUS
Bus_SubmitReport(
    WDFDEVICE Device,
    ULONG SerialNo,
    PVOID Report
);


#pragma endregion

#pragma region USB-specific functions

BOOLEAN USB_BUSIFFN UsbPdo_IsDeviceHighSpeed(IN PVOID BusContext);
NTSTATUS USB_BUSIFFN UsbPdo_QueryBusInformation(IN PVOID BusContext, IN ULONG Level, IN OUT PVOID BusInformationBuffer, IN OUT PULONG BusInformationBufferLength, OUT PULONG BusInformationActualLength);
NTSTATUS USB_BUSIFFN UsbPdo_SubmitIsoOutUrb(IN PVOID BusContext, IN PURB Urb);
NTSTATUS USB_BUSIFFN UsbPdo_QueryBusTime(IN PVOID BusContext, IN OUT PULONG CurrentUsbFrame);
VOID USB_BUSIFFN UsbPdo_GetUSBDIVersion(IN PVOID BusContext, IN OUT PUSBD_VERSION_INFORMATION VersionInformation, IN OUT PULONG HcdCapabilities);
NTSTATUS UsbPdo_GetDeviceDescriptorType(PURB urb, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_GetConfigurationDescriptorType(PURB urb, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_GetStringDescriptorType(PURB urb, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_SelectConfiguration(PURB urb, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_SelectInterface(PURB urb, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_BulkOrInterruptTransfer(PURB urb, WDFDEVICE Device, WDFREQUEST Request);
NTSTATUS UsbPdo_AbortPipe(WDFDEVICE Device);
NTSTATUS UsbPdo_ClassInterface(PURB urb, WDFDEVICE Device, PPDO_DEVICE_DATA pCommon);
NTSTATUS UsbPdo_GetDescriptorFromInterface(PURB urb, PPDO_DEVICE_DATA pCommon);

#pragma endregion

//
// Utility functions
//

VOID ReverseByteArray(PUCHAR Array, INT Length);
VOID GenerateRandomMacAddress(PMAC_ADDRESS Address);

//
// XUSB-specific functions
// 
NTSTATUS Xusb_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription);
NTSTATUS Xusb_PrepareHardware(WDFDEVICE Device);
NTSTATUS Xusb_AssignPdoContext(WDFDEVICE Device, PPDO_IDENTIFICATION_DESCRIPTION Description);

//
// XGIP-specific functions
// 
NTSTATUS Xgip_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription);
NTSTATUS Xgip_PrepareHardware(WDFDEVICE Device);
NTSTATUS Xgip_AssignPdoContext(WDFDEVICE Device);

//
// DS4-specific functions
// 
NTSTATUS Ds4_PreparePdo(PWDFDEVICE_INIT DeviceInit, PUNICODE_STRING DeviceId, PUNICODE_STRING DeviceDescription);
NTSTATUS Ds4_PrepareHardware(WDFDEVICE Device);
NTSTATUS Ds4_AssignPdoContext(WDFDEVICE Device, PPDO_IDENTIFICATION_DESCRIPTION Description);

