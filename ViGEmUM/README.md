# ViGEm User-Mode DLL

![Disclaimer](http://nefarius.at/public/Alpha-Disclaimer.png)

## API documentation

### Initialize library
```c
VIGEM_API VIGEM_ERROR vigem_init();
```
#### Summary
Checks if the necessary drivers are accessible and initializes the libraries internals.
#### Remarks
Call this once before using any other API functions. All owned target devices get removed before every call of this function.
#### Parameters
None.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Shut down library
```c
VIGEM_API VOID vigem_shutdown();
```
#### Summary
Frees all used resources and disconnects all owned target devices.
#### Remarks
Optional, gets called automatically on process termination.
#### Parameters
None.
#### Returns
Nothing.

### Plug in target
```c
VIGEM_API VIGEM_ERROR vigem_target_plugin(
    _In_ VIGEM_TARGET_TYPE Type,
    _Out_ PVIGEM_TARGET Target);
```
#### Summary
Requests a desired target device to be added to the system.
#### Remarks
This function will try to acquire a free slot on the bus driver and request a new target device to be spawned.
#### Parameters
Name | Description
--- | ---
`Type` | The `VIGEM_TARGET` type this target device should emulate.
`Target` | A pointer to a `VIGEM_TARGET` object representing the target device to get plugged in.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Unplug target
```c
VIGEM_API VIGEM_ERROR vigem_target_unplug(
    _Out_ PVIGEM_TARGET Target);
```
#### Summary
Requests a desired target device to be removed from the system.
#### Remarks
This function will try to request the supplied target device to be unplugged.
#### Parameters
Name | Description
--- | ---
`Target` | A pointer to a `VIGEM_TARGET` object representing the target device to get unplugged.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Report to XUSB target
```c
VIGEM_API VIGEM_ERROR vigem_xusb_submit_report(
        _In_ VIGEM_TARGET Target,
        _In_ XUSB_REPORT Report);
```
#### Summary
Sends a report to a target emulating a wired Xbox 360 Controller device.
#### Remarks
This function updates the state of the virtual gamepad and has to be called periodically. If the target is no longer connected or not owned by the current process, this function fails.
#### Parameters
Name | Description
--- | ---
`Target` | A `VIGEM_TARGET` object representing the target device to send the report to.
`Report` | An [XINPUT_GAMEPAD](https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396) compatible (`XUSB_REPORT`) report structure to send to the target device.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Report to DS4 target
```c
VIGEM_API VIGEM_ERROR vigem_ds4_submit_report(
        _In_ VIGEM_TARGET Target,
        _In_ DS4_REPORT Report);
```
#### Summary
Sends a report to a target emulating a wired DualShock 4 Controller device.
#### Remarks
This function updates the state of the virtual gamepad and has to be called periodically. If the target is no longer connected or not owned by the current process, this function fails.
#### Parameters
Name | Description
--- | ---
`Target` | A `VIGEM_TARGET` object representing the target device to send the report to.
`Report` | A `DS4_REPORT` report structure to send to the target device.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Register callback for XUSB notifications
```c
VIGEM_API VIGEM_ERROR vigem_register_xusb_notification(
        _In_ PVIGEM_XUSB_NOTIFICATION Notification,
        _In_ VIGEM_TARGET Target);
```
#### Summary
Registers a callback function for notifications reported by the virtual Xbox 360 Controller.
#### Remarks
Feedback data like vibration state and LED index are reported to the callback function.
#### Parameters
Name | Description
--- | ---
`Notification` | A pointer to a notification callback function.
`Target` | A  `VIGEM_TARGET` object representing the target device to send the report to.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Register callback for DS4 notifications
```c
VIGEM_API VIGEM_ERROR vigem_register_xusb_notification(
        _In_ PVIGEM_XUSB_NOTIFICATION Notification,
        _In_ VIGEM_TARGET Target);
```
#### Summary
Registers a callback function for notifications reported by the virtual DualShock 4 Controller.
#### Remarks
Feedback data like vibration state and lightbar color are reported to the callback function.
#### Parameters
Name | Description
--- | ---
`Notification` | A pointer to a notification callback function.
`Target` | A  `VIGEM_TARGET` object representing the target device to send the report to.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.
