// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DEDICORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DEDICORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DEDICORE_EXPORTS
#define DEDICORE_API __declspec(dllexport)
#else
#define DEDICORE_API __declspec(dllimport)
#endif

#include "..\DediPro\IProject.h"


// This class is exported from the DediCore.dll
class DEDICORE_API CDediCore {
public:
	CDediCore(void);
	// TODO: add your methods here.
};

extern DEDICORE_API int nDediCore;



// dedi interfaces
DEDICORE_API int dedi_get_version(void);
DEDICORE_API HPROJECT getProjectHandle(int target);
