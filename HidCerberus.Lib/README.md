# HidCerberus.Lib
## How to use
### Native C/C++
```cpp
typedef void (WINAPI* HidGuardianOpen_t)();
typedef void (WINAPI* HidGuardianClose_t)();

HMODULE cerberus = LoadLibrary(L"HidCerberus.Lib.dll");
HidGuardianOpen_t fpOpen = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianOpen"));
HidGuardianOpen_t fpClose = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianClose"));

fpOpen();
// Open the desired device, do your stuff with it
fpClose();
```
