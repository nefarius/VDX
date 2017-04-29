# HidCerberus Library
## 3rd party dependencies
 * [POCO C++ libraries](https://pocoproject.org/)

## How to build
**Note:** this is currently built with Visual Studio 2015, adjust parameters accordingly for other versions!
 * [Download the POCO libraries](https://pocoproject.org/download/index.html) (Basic Edition is fine)
 * Extract the archive to an arbitrary location
 * Launch the `VS2015 x86 Native Tools Command Prompt` (`%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" x86`)
 * Move to extracted POCO folder
 * Build with `buildwin 140 build static_mt both Win32 nosamples`
 * Launch the `VS2015 x64 Native Tools Command Prompt` (`%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" amd64`)
 * Move to extracted POCO directory
 * Build with `buildwin 140 build static_mt both x64 nosamples`
 * Create an environment variable named `POCO_ROOT` pointing to the extracted POCO directory
 * Restart Visual Studio if necessary
 * Build the library
