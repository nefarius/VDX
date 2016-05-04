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

// Microsoft Xbox 360 Controller (wired) Hardware-IDs
#define X360WIRED_HARDWARE_IDS          L"USB\\VID_045E&PID_028E&REV_0114\0USB\\VID_045E&PID_028E\0"
#define X360WIRED_HARDWARE_IDS_LENGTH   sizeof(X360WIRED_HARDWARE_IDS)

// XUSB-Class compatible IDs
#define X360WIRED_COMPATIBLE_IDS        L"USB\\MS_COMP_XUSB10\0USB\\Class_FF&SubClass_5D&Prot_01\0USB\\Class_FF&SubClass_5D\0USB\\Class_FF\0"
#define X360WIRED_COMPATIBLE_IDS_LENTH  sizeof(X360WIRED_COMPATIBLE_IDS_LENTH)

// Sony DualShock 4 Controller (wired) Hardware-IDs
#define DS4WIRED_HARDWARE_IDS           L"USB\\VID_054C&PID_05C4&REV_0100\0USB\\VID_054C&PID_05C4\0"
#define DS4WIRED_HARDWARE_IDS_LENTH     sizeof(DS4WIRED_HARDWARE_IDS)

#define DS4WIRED_COMPATIBLE_IDS         L"\0\0"
#define DS4WIRED_COMPATIBLE_IDS_LENTH   sizeof(DS4WIRED_COMPATIBLE_IDS)

#define MAX_INSTANCE_ID_LEN     80
#define MAX_COMPATIBLE_IDS_LEN  255

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
    DWORD CallingProcessId;
} PDO_DEVICE_DATA, *PPDO_DEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PDO_DEVICE_DATA, PdoGetData)


//
// Prototypes of functions
//

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD Bus_EvtDeviceAdd;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Bus_EvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Bus_EvtIoInternalDeviceControl;

EVT_WDF_CHILD_LIST_CREATE_DEVICE Bus_EvtDeviceListCreatePdo;


NTSTATUS
Bus_PlugInDevice(
    _In_ WDFDEVICE Device,
         _In_ ULONG SerialNo
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

