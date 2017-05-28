# XnaGuardian
Adds various extensions to the `XInput` API on a driver level.

![Disclaimer](http://nefarius.at/public/Alpha-Disclaimer.png)

## Why
This projects satisfies a fairly specific demand. Picture the following setup:
 * The user utilizes an Xbox 360/Xbox One controller with a compatible game.
 * Button inputs shall be sent to the game unobstructed by default.
 * One or more foreign devices provide distinct inputs for movemets which shall be mapped to the thumb axes.
 * The resulting *merged* inputs have to be picked up by a wide variety of games without the need for code adjustments or other tricks like DLL injection, API hooking or Proxy DLLs.
 * The button/axis override/merging behavior should be as open and dynamic as possible.

## How
To achive the described demand the flow of input data has to be intercepted and manipulated at multiple levels in different driver stacks. A class filter driver provides the necessary logic and attaches itself at multiple device classes where it fulfills class-specific filtering.

### XnaComposite
Microsoft Xbox 360 (wired or wireless) and compatible controllers are grouped by the `XnaComposite` (`{D61CA365-5AF4-4486-998B-9DB4734C6CA3}`) class. Currently the official Microsoft `XUSB22.sys` function driver gets attached to these devices, which - among other things - provides an interface for the `XInput*` user-mode libraries and axposes a virtual HID device for DirectInput compatibility.

### XboxComposite
TBD

### HIDClass
TBD

## Demo
TDB

## Manual Installation
```
devcon.exe install XnaGuardian.inf Root\XnaGuardian
devcon.exe classfilter XnaComposite upper -XnaGuardian
devcon.exe classfilter XboxComposite upper -XnaGuardian
devcon.exe classfilter HIDClass lower -XnaGuardian
```

## Manual Removal
```
devcon.exe classfilter HIDClass lower !XnaGuardian
devcon.exe classfilter XboxComposite upper !XnaGuardian
devcon.exe classfilter XnaComposite upper !XnaGuardian
devcon.exe remove Root\XnaGuardian
```

