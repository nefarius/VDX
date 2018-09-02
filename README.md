# XInput/x360ce to ViGEm sample application

Adds ViGEm support to Xbox 360/One controllers and unmodified x360ce application.

[![Build status](https://ci.appveyor.com/api/projects/status/5bsea7nmggf08w2n?svg=true)](https://ci.appveyor.com/project/nefarius/vdx) [![Discord](https://img.shields.io/discord/346756263763378176.svg)](https://discord.gg/QTJpBX5)  [![Website](https://img.shields.io/website-up-down-green-red/https/vigem.org.svg?label=ViGEm.org)](https://vigem.org/) [![PayPal Donate](https://img.shields.io/badge/paypal-donate-blue.svg)](<https://paypal.me/NefariusMaximus>) [![Support on Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](<https://www.patreon.com/nefarius>) [![GitHub followers](https://img.shields.io/github/followers/nefarius.svg?style=social&label=Follow)](https://github.com/nefarius) [![Twitter Follow](https://img.shields.io/twitter/follow/nefariusmaximus.svg?style=social&label=Follow)](https://twitter.com/nefariusmaximus)

![explorer_2018-08-31_21-56-18.png](https://lh3.googleusercontent.com/-Ib62elVsRtQ/W4mdcwQj0uI/AAAAAAAABuM/ajU1PiueT7IRHfO68wTKhuaAdgTsPZFEgCHMYCw/s0/explorer_2018-08-31_21-56-18.png)

## How to use

- [Setup ViGEm](https://docs.vigem.org/#!vigem-bus-driver-installation.md) if you haven't already.
- Get [x360ce](https://github.com/x360ce/x360ce#download) (don't mix x86 and x64!)
  - If x360ce is new to you [check out its requirements first](https://github.com/x360ce/x360ce#system-requirements).
- Get the [Microsoft Visual C++ Redistributable für Visual Studio 2017](https://visualstudio.microsoft.com/de/downloads/) package
- [Download VDX](https://buildbot.vigem.org/builds/VDX/master/) (grab highest version number for most recent build)
- Configure your gamepad(s) with x360ce
- Place x360ce and the following files in one common directory (**NOT** the game directory!):
  - `XInput1_3.dll` (the one crated by x360ce!)
  - `VDX.exe` (and DLLs)
- Run `VDX.exe`
- If you've set up x360ce correctly your configured Gamepad(s) should be in `Connected` state. Select the type of emulation you'd like to enable and hit `Connect`.
- Run your game and enjoy

## How to build

### Prerequisites

- Visual Studio **2017** ([Community Edition](https://www.visualstudio.com/thank-you-downloading-visual-studio/?sku=Community&rel=15) is just fine)
- [.NET Core SDK 2.1](https://www.microsoft.com/net/download/dotnet-core/2.1) (or greater, required for building only)
- [Vcpkg](https://github.com/Microsoft/vcpkg) (used to acquire SFML)
  - [Follow the Quick Start](https://github.com/Microsoft/vcpkg#quick-start)
  - `.\vcpkg install sfml:x64-windows-static`
  - `.\vcpkg install sfml:x86-windows-static`
  - `.\vcpkg.exe integrate install`

## 3rd party stuff used

- [Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
- [dear imgui](https://github.com/ocornut/imgui)
- [ImGui + SFML](https://github.com/eliasdaler/imgui-sfml)
