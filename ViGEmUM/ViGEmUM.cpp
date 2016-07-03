// ViGEmUM.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ViGEmUM.h"


// This is an example of an exported variable
VIGEMUM_API int nViGEmUM=0;

// This is an example of an exported function.
VIGEMUM_API int fnViGEmUM(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see ViGEmUM.h for the class definition
CViGEmUM::CViGEmUM()
{
    return;
}
