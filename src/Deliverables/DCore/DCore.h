// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DCORE_EXPORTS
#define DCORE_API __declspec(dllexport)
#else
#define DCORE_API __declspec(dllimport)
#endif

#include "..\DediPro\IProject.h"

// This class is exported from the DCore.dll
class DCORE_API CDCore 
{
public:
	CDCore(void);
	// TODO: add your methods here.
public:
	int getVersion();
};

extern DCORE_API int nDCore;

DCORE_API int fnDCore(void);

DCORE_API HPROJECT getProjectHandle(int target);



