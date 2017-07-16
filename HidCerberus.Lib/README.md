# HidCerberus Library
The HidCerberus Library interacts with the [HidCerberus Service](../../../tree/master/HidCerberus.Srv) and allows the calling process to bypass the filter driver to gain access to hidden HID devices.

## 3rd party dependencies
 * [POCO C++ libraries](https://pocoproject.org/) 
   * Will be installed automatically via NuGet on first build

## How to use
### Native C/C++
``` C
typedef BOOL (WINAPI* HidGuardianOpen_t)();
typedef BOOL (WINAPI* HidGuardianClose_t)();

static HidGuardianOpen_t fpOpen;
static HidGuardianOpen_t fpClose;

auto cerberus = LoadLibrary(L"HidCerberus.Lib.dll");

fpOpen = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianOpen"));
fpClose = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianClose"));

// Before opening HID device
if (fpOpen && fpOpen()) printf("Success");

// Before program exit
if (fpClose && fpClose()) printf("Success");
```
