#ifdef _MSC_VER
#pragma once
#endif

#include <initguid.h>
#include <public.h>

#ifdef VIGEM_EXPORTS
#define VIGEM_API __declspec(dllexport)
#else
#define VIGEM_API __declspec(dllimport)
#endif

#define VIGEM_TARGETS_MAX   4

typedef enum _VIGEM_ERRORS
{
    VIGEM_ERROR_NONE = 0x0000,
    VIGEM_ERROR_BUS_NOT_FOUND,
    VIGEM_ERROR_NO_FREE_SLOT
} VIGEM_ERROR;

#define VIGEM_SUCCESS(_val_) (_val_ == VIGEM_ERROR_NONE)

//
// Represents a virtual gamepad object.
// 
typedef struct _VIGEM_TARGET
{
    IN ULONG Size;
    IN USHORT Version;
    IN ULONG SerialNo;
} VIGEM_TARGET, *PVIGEM_TARGET;

//
// Initializes a virtual gamepad object.
// 
VOID FORCEINLINE VIGEM_TARGET_INIT(
    _Out_ PVIGEM_TARGET Target
)
{
    RtlZeroMemory(Target, sizeof(VIGEM_TARGET));

    Target->Size = sizeof(VIGEM_TARGET);
    Target->Version = 1;
}

typedef VOID(CALLBACK* VIGEM_XUSB_NOTIFICATION)(
    VIGEM_TARGET Target, 
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    UCHAR LedNumber);

VIGEM_API VIGEM_ERROR vigem_init();

VIGEM_API VOID vigem_shutdown();

VIGEM_API VOID vigem_register_xusb_notification(
    IN VIGEM_XUSB_NOTIFICATION Notification, 
    IN VIGEM_TARGET Target);

VIGEM_API VIGEM_ERROR vigem_target_plugin(
    _In_ VIGEM_TARGET_TYPE Type,
    _Out_ PVIGEM_TARGET Target);

VIGEM_API DWORD vigem_xusb_submit_report(
    _In_ VIGEM_TARGET Target,
    _In_ XUSB_REPORT Report);
