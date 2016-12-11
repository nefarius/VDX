@echo off
SET mypath=%~dp0
SET solution=%mypath:~0,-1%\..

honestus --version-from-file version.txt --target-file "%solution%\HidCerberus.Lib\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version
honestus --version-from-file version.txt --target-file "%solution%\HidCerberus.Srv\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version
honestus --version-from-file version.txt --target-file "%solution%\HidGuardian\HidGuardian.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\HidGuardian\HidGuardian.vcxproj" --vcxproj.inf-time-stamp
honestus --version-from-file version.txt --target-file "%solution%\ViGEmBus\ViGEmBus.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\ViGEmBus\ViGEmBus.vcxproj" --vcxproj.inf-time-stamp
honestus --version-from-file version.txt --target-file "%solution%\x360ce2ViGEm\x360ce2ViGEm.rc" --resource.file-version --resource.product-version
