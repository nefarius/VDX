#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <initguid.h>
#include "public.h"
#include <usb.h>
#include <usbbusif.h>

// 
// For children emulating XUSB devices, the following dummy interfaces 
// have to be exposed by the PDO or else the child devices won't start
// 
DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_0,
    0x70211B0E, 0x0AFB, 0x47DB, 0xAF, 0xC1, 0x41, 0x0B, 0xF8, 0x42, 0x49, 0x7A);

DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_1,
    0xB38290E5, 0x3CD0, 0x4F9D, 0x99, 0x37, 0xF5, 0xFE, 0x2B, 0x44, 0xD4, 0x7A);

DEFINE_GUID(GUID_DEVINTERFACE_XUSB_UNKNOWN_2,
    0x2AEB0243, 0x6A6E, 0x486B, 0x82, 0xFC, 0xD8, 0x15, 0xF6, 0xB9, 0x70, 0x06);

#pragma once

//
// Static information
// 

#define MAX_INSTANCE_ID_LEN             80
#define MAX_DEVICE_DESCRIPTION_LEN      128
#define DESCRIPTOR_SIZE	                0x0099

#if defined(_X86_)
#define CONFIGURATION_SIZE              0x00E4
#else
#define CONFIGURATION_SIZE              0x0130
#endif

#define RUMBLE_SIZE                     8
#define LEDSET_SIZE                     3
#define LEDNUM_SIZE                     1

//
// Helpers
// 

#define CURRENT_PROCESS_ID() ((DWORD)((DWORD_PTR)PsGetCurrentProcessId() & 0xFFFFFFFF))


typedef struct _PDO_IDENTIFICATION_DESCRIPTION
{
    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER Header; // should contain this header

    ULONG SerialNo;

    VIGEM_TARGET_TYPE TargetType;
} PDO_IDENTIFICATION_DESCRIPTION, *PPDO_IDENTIFICATION_DESCRIPTION;

//
// This is PDO device-extension.
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
// Prototypes of functions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;

EVT_WDF_IO_QUEUE_IO_DEFAULT Bus_EvtIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Bus_EvtIoDeviceControl;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;

EVT_WDF_DEVICE_PREPARE_HARDWARE Bus_EvtDevicePrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY Bus_EvtDeviceD0Entry;

// Experimental
EVT_WDF_IO_QUEUE_IO_DEFAULT RawPdo_EvtIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL RawPdo_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL RawPdo_EvtIoInternalDeviceControl;

EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST  Pdo_ProcessQueryInterfaceRequest;

//
// Bus enumeration-specific functions
// 
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
Bus_EjectDevice(
    WDFDEVICE Device,
    ULONG SerialNo
);

NTSTATUS
Bus_CreatePdo(
    _In_ WDFDEVICE Device,
    _In_ PWDFDEVICE_INIT ChildInit,
    _In_ ULONG SerialNo,
    _In_ VIGEM_TARGET_TYPE TargetType
);

//
// USB-specific functions
// 
BOOLEAN USB_BUSIFFN UsbPdo_IsDeviceHighSpeed(IN PVOID BusContext);
