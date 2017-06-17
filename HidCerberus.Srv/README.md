# HidCerberus Service
The HidCerberus Service abstracts away configuration tasks for the [HidGuardian Device Driver](../HidGuardian) and allows literally any 3rd party tool to interact with the driver via HTTP REST API.

## 3rd party dependencies
The following dependency libraries will be automatically resolved via [NuGet](http://www.nuget.org/) on first build.
 * [NancyFx](http://nancyfx.org/)
 * [Topshelf](http://topshelf-project.com/)
 * [Fody](https://github.com/Fody/Fody)
 * [Fody Costura](https://github.com/Fody/Costura)

## Manual Installation
 * Either run `HidCerberus.Srv.exe` standalone or install it as a Windows Service with `HidCerberus.Srv.exe install` in an elevated command prompt.
 * While you're in an elevated prompt, start the service with `HidCerberus.Srv.exe start`
 * You should now be able to navigate to the [configuration page](http://localhost:26762/).
