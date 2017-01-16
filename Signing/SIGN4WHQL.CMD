@echo off
SET mypath=%~dp0

cd /d "%mypath%"
makecab /d SourceDir="%mypath%\..\x64\Release\ViGEm Drivers Redistributable" /f ViGEm.ddf
SignTool sign /v /ac "C:\Program Files (x86)\Windows Kits\10\CrossCertificates\GlobalSign Root CA.crt" /n "Wohlfeil.IT e.U." /t http://timestamp.verisign.com/scripts/timstamp.dll "%mypath%\Disk1\ViGEm_x64.cab"
pause