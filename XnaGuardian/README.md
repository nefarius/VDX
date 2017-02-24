# XnaGuardian
Adds various extensions to the `XInput` API on a driver level.

![Disclaimer](http://nefarius.at/public/Alpha-Disclaimer.png)

## The Problem
TDB

## The Semi-Solution
TDB

## The Real Solution
TDB

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
devcon.exe remove Root\XnaGuardian
devcon.exe classfilter XboxComposite upper !XnaGuardian
devcon.exe classfilter XnaComposite upper !XnaGuardian
devcon.exe classfilter HIDClass lower !XnaGuardian
```

