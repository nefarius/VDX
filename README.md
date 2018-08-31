# XInput/x360ce to ViGEm sample application

Adds ViGEm support to Xbox 360/One controllers and unmodified x360ce application.

![explorer_2018-08-31_21-56-18.png](https://lh3.googleusercontent.com/-Ib62elVsRtQ/W4mdcwQj0uI/AAAAAAAABuM/ajU1PiueT7IRHfO68wTKhuaAdgTsPZFEgCHMYCw/s0/explorer_2018-08-31_21-56-18.png)

## Requirements

[ViGEm set up](https://docs.vigem.org/#!vigem-bus-driver-installation.md), obviously.

## How to use

- Get [x360ce](https://github.com/x360ce/x360ce#download) (don't mix x86 and x64!)
  - If x360ce is new to you [check out its requirements first](https://github.com/x360ce/x360ce#system-requirements).
- Get the [Microsoft Visual C++ Redistributable für Visual Studio 2017](https://visualstudio.microsoft.com/de/downloads/) package
- [Download VDX](https://downloads.vigem.org/stable/latest/windows/x86_64/VDX_x86_x64_latest.zip)
- Configure your gamepad(s) with x360ce
- Place x360ce and the following files in one common directory (**NOT** the game directory!):
  - `XInput1_3.dll` (the one crated by x360ce!)
  - `VDX.exe` (and DLLs)
- Run `VDX.exe`
- If you've set up x360ce correctly your configured Gamepad(s) should be in `Connected` state. Select the type of emulation you'd like to enable and hit `Connect`.
- Run your game and enjoy

## 3rd party stuff used

- [Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
- [dear imgui](https://github.com/ocornut/imgui)
- [ImGui + SFML](https://github.com/eliasdaler/imgui-sfml)
