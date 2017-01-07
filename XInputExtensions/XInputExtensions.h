// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XINPUTEXTENSIONS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XINPUTEXTENSIONS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XINPUTEXTENSIONS_EXPORTS
#define XINPUTEXTENSIONS_API __declspec(dllexport)
#else
#define XINPUTEXTENSIONS_API __declspec(dllimport)
#endif

// This class is exported from the XInputExtensions.dll
class XINPUTEXTENSIONS_API CXInputExtensions {
public:
	CXInputExtensions(void);
	// TODO: add your methods here.
};

extern XINPUTEXTENSIONS_API int nXInputExtensions;

XINPUTEXTENSIONS_API int fnXInputExtensions(void);
