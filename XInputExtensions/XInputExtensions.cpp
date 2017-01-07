// XInputExtensions.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "XInputExtensions.h"


// This is an example of an exported variable
XINPUTEXTENSIONS_API int nXInputExtensions=0;

// This is an example of an exported function.
XINPUTEXTENSIONS_API int fnXInputExtensions(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see XInputExtensions.h for the class definition
CXInputExtensions::CXInputExtensions()
{
    return;
}
