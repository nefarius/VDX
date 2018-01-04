# XInput/x360ce to ViGEm sample application
Adds ViGEm support to unmodified x360ce application.

![](https://lh3.googleusercontent.com/-WND-yPLUyZI/Wd-qN0XTbNI/AAAAAAAAAZI/Yeumwl11FYM2HGVJbYP4H1I9o-O6FWn3ACHMYCw/s0/VDX_x64_2017-10-12_19-45-31.png)

## Requirements
[ViGEm set up](https://github.com/nefarius/ViGEm/wiki/Driver-Installation), obviously.

## How to use
- Get [x360ce](https://github.com/x360ce/x360ce#download) (don't mix x86 and x64!)
  - If x360ce is new to you [check out its requirements first](https://github.com/x360ce/x360ce#system-requirements).
- Get the [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) package
- [Download VDX](https://downloads.vigem.org/stable/latest/windows/x86_64/VDX_x86_x64_latest.zip)
- Configure your gamepad(s) with x360ce
- Place x360ce and the following files in one common directory (**NOT** the game directory!):
  - `XInput1_3.dll` (the one crated by x360ce!)
  - `VDX.exe` (or `VDX_x64.exe` for the x64 version)
- Run `VDX.exe`
- If you've set up x360ce correctly your configured Gamepad(s) should be in `Connected` state. Select the type of emulation you'd like to enable and hit `Connect`.
- Run your game and enjoy

## 3rd party stuff used
 - [Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
 - [dear imgui](https://github.com/ocornut/imgui)
