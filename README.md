# x360ce to ViGEm sample application
Adds ViGEm support to unmodified x360ce application.

![](https://lh3.googleusercontent.com/-hkVia9DAzRA/WagauZ7JsZI/AAAAAAAAAQ0/emEbEHo4PjMvkIzTvx9EPkabw34xlJbOwCHMYCw/s0/VDX_2017-08-31_16-18-32.png)

## How to use
- Get [x360ce](https://github.com/x360ce/x360ce) (don't mix x86 and x64!)
  - If x360ce is new to you [check out its requirements first](https://github.com/x360ce/x360ce#system-requirements).
- Get the [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) package
- [Download VDX](https://github.com/nefarius/ViGEm/releases/download/v1.12.1.0/VDX_1.12.1.0.zip)
- Configure your gamepad(s) with x360ce
- Place x360ce and the following files in one common directory (**NOT** the game directory!):
  - `XInput1_3.dll` (the one crated by x360ce!)
  - `VDX.exe` (or `VDX_x64.exe` for the x64 version)
- Run `VDX.exe`
- Run your game and enjoy

## 3rd party stuff used
 - [Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
 - [dear imgui](https://github.com/ocornut/imgui)
