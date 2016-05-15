#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <ntintsafe.h>
#include <initguid.h>
#include "public.h"

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
} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDO_DEVICE_DATA, PdoGetData)


//
// Prototypes of functions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;

EVT_WDF_IO_QUEUE_IO_DEFAULT Bus_EvtIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Bus_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Bus_EvtIoInternalDeviceControl;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;

EVT_WDF_DEVICE_PREPARE_HARDWARE Bus_EvtDevicePrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY Bus_EvtDeviceD0Entry;


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

