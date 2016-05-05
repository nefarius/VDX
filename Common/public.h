// {96E42B22-F5E9-42F8-B043-ED0F932F014F}
DEFINE_GUID(GUID_DEVINTERFACE_BUSENUM_VIGEM,
    0x96E42B22, 0xF5E9, 0x42F8, 0xB0, 0x43, 0xED, 0x0F, 0x93, 0x2F, 0x01, 0x4F);

// {02098E80-3C81-4CF0-A2C5-CD4D3B318902}
DEFINE_GUID(GUID_DEVINTERFACE_VIGEM,
    0x02098E80, 0x3C81, 0x4CF0, 0xA2, 0xC5, 0xCD, 0x4D, 0x3B, 0x31, 0x89, 0x02);

#pragma once

#define FILE_DEVICE_BUSENUM             FILE_DEVICE_BUS_EXTENDER
#define BUSENUM_IOCTL(_index_)          CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define BUSENUM_W_IOCTL(_index_)        CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_WRITE_DATA)
#define BUSENUM_R_IOCTL(_index_)        CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define BUSENUM_RW_IOCTL(_index_)       CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

#define IOCTL_BUSENUM_BASE 0x801

// 
// IO control codes
// 
#define IOCTL_BUSENUM_PLUGIN_HARDWARE   BUSENUM_W_IOCTL (IOCTL_BUSENUM_BASE + 0x0)
#define IOCTL_BUSENUM_UNPLUG_HARDWARE   BUSENUM_W_IOCTL (IOCTL_BUSENUM_BASE + 0x1)
#define IOCTL_BUSENUM_EJECT_HARDWARE    BUSENUM_W_IOCTL (IOCTL_BUSENUM_BASE + 0x2)
#define IOCTL_BUSENUM_REPORT_HARDWARE   BUSENUM_RW_IOCTL(IOCTL_BUSENUM_BASE + 0x3)

//
//  Data structure used in PlugIn and UnPlug ioctls
//

typedef enum _VIGEM_TARGET_TYPE
{
    // 
    // Microsoft Xbox 360 Controller (wired)
    // 
    Xbox360Wired,
    //
    // Sony DualShock 4 (wired)
    // 
    DualShock4Wired
} VIGEM_TARGET_TYPE, *PVIGEM_TARGET_TYPE;

typedef struct _BUSENUM_PLUGIN_HARDWARE
{
    //
    // sizeof (struct _BUSENUM_HARDWARE)
    //
    IN ULONG Size;

    //
    // Unique serial number of the device to be enumerated.
    // Enumeration will be failed if another device on the
    // bus has the same serial number.
    //

    IN ULONG SerialNo;

    // 
    // Type of the target device to emulate.
    // 
    VIGEM_TARGET_TYPE TargetType;
} BUSENUM_PLUGIN_HARDWARE, *PBUSENUM_PLUGIN_HARDWARE;

typedef struct _BUSENUM_UNPLUG_HARDWARE
{
    //
    // sizeof (struct _REMOVE_HARDWARE)
    //

    IN ULONG Size;

    //
    // Serial number of the device to be plugged out
    //
    ULONG SerialNo;

} BUSENUM_UNPLUG_HARDWARE, *PBUSENUM_UNPLUG_HARDWARE;

typedef struct _BUSENUM_EJECT_HARDWARE
{
    //
    // sizeof (struct _EJECT_HARDWARE)
    //

    IN ULONG Size;

    //
    // Serial number of the device to be ejected
    //

    ULONG SerialNo;

} BUSENUM_EJECT_HARDWARE, *PBUSENUM_EJECT_HARDWARE;

