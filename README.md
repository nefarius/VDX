# XInput/x360ce to ViGEm sample application

Adds ViGEm support to Xbox 360/One controllers and unmodified x360ce (v3.x) application.

[![Build status](https://ci.appveyor.com/api/projects/status/5bsea7nmggf08w2n?svg=true)](https://ci.appveyor.com/project/nefarius/vdx) [![Discord](https://img.shields.io/discord/346756263763378176.svg)](https://discord.vigem.org) [![Website](https://img.shields.io/website-up-down-green-red/https/vigem.org.svg?label=ViGEm.org)](https://vigem.org/) [![GitHub followers](https://img.shields.io/github/followers/nefarius.svg?style=social&label=Follow)](https://github.com/nefarius) [![Twitter Follow](https://img.shields.io/twitter/follow/nefariusmaximus.svg?style=social&label=Follow)](https://twitter.com/nefariusmaximus)

![explorer_fkGCP5FSPx.png](assets/explorer_fkGCP5FSPx.png)

## How to use

👉 [Documentation](https://forums.vigem.org/topic/272/x360ce-to-vigem)

## How to build

### Prerequisites

- Visual Studio **2019** ([Community Edition](https://www.visualstudio.com/thank-you-downloading-visual-studio/?sku=Community&rel=16) is just fine)
- [Follow the Vcpkg Quick Start](https://github.com/Microsoft/vcpkg#quick-start) and install the following packages:
  - `.\vcpkg.exe install sfml:x86-windows-static sfml:x64-windows-static imgui:x86-windows-static imgui:x64-windows-static imgui-sfml:x86-windows-static imgui-sfml:x64-windows-static`
- [Follow the ViGEmClient Vcpkg Setup](https://github.com/ViGEm/ViGEmClient.vcpkg#usage)

## 3rd party credits

- [Simple and Fast Multimedia Library](https://www.sfml-dev.org/)
- [dear imgui](https://github.com/ocornut/imgui)
- [ImGui + SFML](https://github.com/eliasdaler/imgui-sfml)
