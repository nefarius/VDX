# HidCerberus Library
## 3rd party dependencies
 * [POCO C++ libraries](https://pocoproject.org/)

## How to build
**Note:** this is currently built with Visual Studio 2015, adjust parameters accordingly for other versions!
 1. [Download the POCO libraries](https://pocoproject.org/download/index.html) (Basic Edition is fine)
 2. Extract the archive to an arbitrary location
 3. Launch the `VS2015 x86 Native Tools Command Prompt` (`%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" x86`)
 4. Move to extracted POCO folder
 5. Build with `buildwin 140 build static_mt both Win32 nosamples`
 6. Launch the `VS2015 x64 Native Tools Command Prompt` (`%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" amd64`)
 7. Move to extracted POCO directory
 8. Build with `buildwin 140 build static_mt both x64 nosamples`
 9. Create an environment variable named `POCO_ROOT` pointing to the extracted POCO directory
 10. Restart Visual Studio if necessary
 11. Build the library
 12. ???
 13. Profit!

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
