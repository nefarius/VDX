# ViGEm
Virtual Gamepad Emulation Driver

Follow development progress on [Twitter](https://twitter.com/CNefarius) and [YouTube](https://www.youtube.com/user/nefarius2k8)! Like my stuff? [Support me on Patreon](https://www.patreon.com/nefarius)!

This repository contains projects related to USB device emulation and HID device manipulation on the Windows platform.

## Projects
- [ViGEm Bus Driver](../../tree/master/ViGEmBus) - a Windows kernel-mode driver which supports emulating various well-known USB gamepads.
- [ViGEm User-Mode DLL](../../tree/master/ViGEmUM) - a Windows user-mode library which simplifies interaction with the bus driver.
- [ViGEm User-Mode Assembly](../../tree/master/ViGEmUM.NET) - a managed wrapper assembly around the user-mode library for .NET applications.
- [ViGEmTester](../../tree/master/ViGEmTester) - general purpose test application for debugging purposes.
- [ViGEmTester.NET](../../tree/master/ViGEmTester.NET) - general purpose managed test application for debugging purposes.
- [x360ce to ViGEm sample application](../../tree/master/x360ce2ViGEm) - sample application adding ViGEm support to [x360ce](https://github.com/x360ce/x360ce).
- [HidGuardian](../../tree/master/HidGuardian) - a Windows kernel-mode filter driver which can hide HID devices from user-mode applications.
- [HidCerberus.Srv](../../tree/master/HidCerberus.Srv) - a Windows Service handling white-listed processes for interaction with `HidGuardian`
- [HidCerberus.Lib](../../tree/master/HidCerberus.Lib) - a Windows user-mode library for interaction with `HidCerberus.Srv`
