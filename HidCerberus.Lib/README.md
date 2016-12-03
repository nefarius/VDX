# HidCerberus.Lib
## How to use
### Native C/C++
Dynamically declare the `HidGuardianOpen()` and `HidGuardianClose()` functions and call them:
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

### .NET (C#)
Reference the `HidCerberus.Lib.dll` assembly in your project and call the static methods:
```csharp
HidCerberus.Lib.HidCerberus.HidGuardianOpen();
// Open the desired device, do your stuff with it
HidCerberus.Lib.HidCerberus.HidGuardianClose();
```
