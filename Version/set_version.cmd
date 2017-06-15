@echo off
SET mypath=%~dp0
SET solution=%mypath:~0,-1%\..

rem HidCerberus Library
honestus --version-from-file version.txt --target-file "%solution%\HidCerberus.Lib\HidCerberus.Lib.rc" --resource.file-version --resource.product-version

rem HidCerberus Server
honestus --version-from-file version.txt --target-file "%solution%\HidCerberus.Srv\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version

rem HidGuardian Driver
honestus --version-from-file version.txt --target-file "%solution%\HidGuardian\HidGuardian.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\HidGuardian\HidGuardian.vcxproj" --vcxproj.inf-time-stamp

rem ViGEm Bus Driver
honestus --version-from-file version.txt --target-file "%solution%\ViGEmBus\ViGEmBus.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\ViGEmBus\ViGEmBus.vcxproj" --vcxproj.inf-time-stamp

rem x360ce2ViGEm Application
honestus --version-from-file version.txt --target-file "%solution%\x360ce2ViGEm\x360ce2ViGEm.rc" --resource.file-version --resource.product-version

rem XnaGuardian Driver
honestus --version-from-file version.txt --target-file "%solution%\XnaGuardian\XnaGuardian.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\XnaGuardian\XnaGuardian.vcxproj" --vcxproj.inf-time-stamp

rem XInputExtensions Library
honestus --version-from-file version.txt --target-file "%solution%\XInputExtensions\XInputExtensions.rc" --resource.file-version --resource.product-version

rem ViGEmUM.NET Library
honestus --version-from-file version.txt --target-file "%solution%\ViGEmUM.NET\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version

rem ViGEmUM Library
honestus --version-from-file version.txt --target-file "%solution%\ViGEmUM\ViGEmUM.rc" --resource.file-version --resource.product-version
