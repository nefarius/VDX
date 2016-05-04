#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <initguid.h>
#include "public.h"

#pragma once

// Microsoft Xbox 360 Controller (wired) Hardware-IDs
#define X360WIRED_HARDWARE_IDS          L"USB\\VID_045E&PID_028E&REV_0114\0USB\\VID_045E&PID_028E\0"
#define X360WIRED_HARDWARE_IDS_LENGTH   sizeof(X360WIRED_HARDWARE_IDS)

// XUSB-Class compatible IDs
#define X360WIRED_COMPATIBLE_IDS        L"USB\\MS_COMP_XUSB10\0USB\\Class_FF&SubClass_5D&Prot_01\0USB\\Class_FF&SubClass_5D\0USB\\Class_FF\0"
#define X360WIRED_COMPATIBLE_IDS_LENTH  sizeof(X360WIRED_COMPATIBLE_IDS_LENTH)

// Sony DualShock 4 Controller (wired) Hardware-IDs
#define DS4WIRED_HARDWARE_IDS           L"USB\VID_054C&PID_05C4&REV_0100\0USB\VID_054C&PID_05C4\0"
#define DS4WIRED_HARDWARE_IDS_LENTH     sizeof(DS4WIRED_HARDWARE_IDS)


typedef struct _PDO_IDENTIFICATION_DESCRIPTION
{
    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER Header; // should contain this header

    ULONG SerialNo;

    size_t CchHardwareIds;

    _Field_size_bytes_(CchHardwareIds) PWCHAR HardwareIds;

} PDO_IDENTIFICATION_DESCRIPTION, *PPDO_IDENTIFICATION_DESCRIPTION;

//
// This is PDO device-extension.
//
typedef struct _PDO_DEVICE_DATA
{
    // Unique serial number of the device on the bus

    ULONG SerialNo;

} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDO_DEVICE_DATA, PdoGetData)

//
// The device extension of the bus itself.  From whence the PDO's are born.
//

typedef struct _FDO_DEVICE_DATA
{
    ULONG Dummy; // TODO: what

} FDO_DEVICE_DATA, *PFDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FDO_DEVICE_DATA, FdoGetData)


//
// Prototypes of functions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Bus_EvtIoDeviceControl;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE Bus_EvtChildListIdentificationDescriptionCompare;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP Bus_EvtChildListIdentificationDescriptionCleanup;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE Bus_EvtChildListIdentificationDescriptionDuplicate;

NTSTATUS
Bus_PlugInDevice(
    _In_ WDFDEVICE       Device,
    _In_ PWCHAR          HardwareIds,
    _In_ size_t          CchHardwareIds,
    _In_ ULONG           SerialNo
);

NTSTATUS
Bus_UnPlugDevice(
    WDFDEVICE   Device,
    ULONG       SerialNo
);


NTSTATUS
Bus_EjectDevice(
    WDFDEVICE   Device,
    ULONG       SerialNo
);

NTSTATUS
Bus_CreatePdo(
    _In_ WDFDEVICE       Device,
    _In_ PWDFDEVICE_INIT ChildInit,
    _In_reads_(MAX_INSTANCE_ID_LEN) PCWSTR HardwareIds,
    _In_ ULONG           SerialNo
);