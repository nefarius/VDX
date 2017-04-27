// HidCerberus.Lib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HidCerberus.Lib.h"


// This is an example of an exported variable
HIDCERBERUSLIB_API int nHidCerberusLib=0;

// This is an example of an exported function.
HIDCERBERUSLIB_API int fnHidCerberusLib(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see HidCerberus.Lib.h for the class definition
CHidCerberusLib::CHidCerberusLib()
{
    return;
}
