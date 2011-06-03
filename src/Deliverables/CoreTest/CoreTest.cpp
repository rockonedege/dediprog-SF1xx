// CoreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CoreTest.h"

#include <iostream>
#include <iomanip>

#include "..\DCore\DCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
		wcout << L"DCore" << std::hex << fnDCore() << L"\n";
		wcout << L"DCore" << std::hex << nDCore << L"\n";

		CDCore core;
		wcout << L"DCore" << std::hex << core.getVersion();

	}

	return nRetCode;
}
