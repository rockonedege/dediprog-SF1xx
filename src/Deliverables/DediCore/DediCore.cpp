// DediCore.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DediCore.h"
#include "..\DediPro\project.h"

// This is an example of an exported variable
DEDICORE_API int nDediCore=0;


// This is the constructor of a class that has been exported.
// see DediCore.h for the class definition
CDediCore::CDediCore()
{
	return;
}


// dedi interfaces

// This is an example of an exported function.
DEDICORE_API int dedi_get_version(void)
{
    //major.minor.build
	return 0x010000;
}

DEDICORE_API HPROJECT getProjectHandle(int target)
{
	return new CProject(CProject::DLL_USERS, (Context::STARTUP_MODE)target);
}
