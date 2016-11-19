# HidGuardian
Blocks HID devices from user mode applications.

## The Problem
Games and other user-mode applications enumerate Joysticks, Gamepads and similar devices through various well-known APIs ([DirectInput](https://msdn.microsoft.com/en-us/library/windows/desktop/ee416842(v=vs.85).aspx), [XInput](https://msdn.microsoft.com/en-us/library/windows/desktop/hh405053(v=vs.85).aspx), [Raw Input](https://msdn.microsoft.com/en-us/library/windows/desktop/ms645536(v=vs.85).aspx)) and continously read their reported input states. The primary collection of devices available through DirectInput are [HID](https://en.wikipedia.org/wiki/Human_interface_device)-Class based devices. When emulating virtual devices with ViGEm the system (and subsequently the application) may not be able to distinguish between e.g. a "real" physical HID Gamepad which acts as a "feeder" and the virtual ViGEm device, therefore suffer from side effects like doubled input. Since coming up with a solution for each application available would become quite tedious a more generalized approach was necessary to reliably solve these issues.

## The Semi-Solution
A common way for intercepting the Game's communication with the input devices would be hooking the mentioned input APIs within the target process. While a stable and user-friendly implementation for the end-user might be achievable for some Processes, targeting the wide variety of Games available on the market is a difficult task. Hooking APIs involves manipulating the target processe's memory which also might falsly trigger Anti-Cheat systems and ban innocent users.

## The Real Solution

## Demo
Sony DualShock 4 and generic USB Gamepad connected:

![](http://content.screencast.com/users/Nefarius/folders/Snagit/media/c2d74568-c5fa-427b-a957-dc32dd31da2a/11.18.2016-23.46.png)

`HidGuardian.sys` active and hiding the DualShock 4:

![](http://content.screencast.com/users/Nefarius/folders/Snagit/media/116cc24b-c06b-4dcf-a3b0-759672ba4836/11.18.2016-23.47.png)
