# ViGEm User-Mode DLL
*Work in progress*

## API documentation

### Initialize library
```c
VIGEM_API VIGEM_ERROR vigem_init();
```
#### Summary
Checks if the necessary drivers are accessible and initializes the libraries internals.
#### Remarks
Call this once before using any other API functions.
#### Parameters
None.
#### Returns
`VIGEM_ERROR_NONE` on success or corresponding `_VIGEM_ERRORS` error code on failure.

### Shuts down library
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
Frees all used resources and disconnects all owned target devices.
#### Remarks
Optional, gets called automatically on process termination.
#### Parameters
Name | Description
--- | ---
`Type` | The `VIGEM_TARGET` type this target device should emulate.
`Target` | A pointer to a `VIGEM_TARGET` object representing the plugged in target device.
#### Returns
Nothing.
