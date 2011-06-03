// DCore.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "DCore.h"

#include "..\DediPro\project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int nDCore = 1;

int fnDCore(void)
{
	return 0x0475;
}

CDCore::CDCore(void)
{
	
}

int CDCore::getVersion(void)
{
	return 0x0500;
}

HPROJECT getProjectHandle(int target)
{
    return new CProject(CProject::USER_GROUP::DLL_USERS, (Context::STARTUP_MODE)target);
}












// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
	}

	return nRetCode;
}
