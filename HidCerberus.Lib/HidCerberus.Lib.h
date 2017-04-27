// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HIDCERBERUSLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HIDCERBERUSLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HIDCERBERUSLIB_EXPORTS
#define HIDCERBERUSLIB_API __declspec(dllexport)
#else
#define HIDCERBERUSLIB_API __declspec(dllimport)
#endif

// This class is exported from the HidCerberus.Lib.dll
class HIDCERBERUSLIB_API CHidCerberusLib {
public:
	CHidCerberusLib(void);
	// TODO: add your methods here.
};

extern HIDCERBERUSLIB_API int nHidCerberusLib;

HIDCERBERUSLIB_API int fnHidCerberusLib(void);
