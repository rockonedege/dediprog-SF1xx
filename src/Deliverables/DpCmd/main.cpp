/**
 * DediCmd.cpp : Defines the entry point for the console application.
 * 
 * Copyright (c) 2006 by <Tan Zhi>
 */

#include "stdafx.h"
#include "DpCmd.h"
#include "moduleversion.h"

#include <string>
using std::string;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// const

static void PrintVersionInfo();
static int  Dispatch(int ac, TCHAR* av[]);      /// dispatch options

CWinApp theApp; // The one and only application object

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
	}
	else
	{
        PrintVersionInfo();

        CDpCmd dpcmd;
        dpcmd.Dispatch(argc, argv);
    }

#ifdef _DEBUG
    std::wcout<< L"\n";
    system("pause");
#endif

	return 0;
}

void PrintVersionInfo()
{
    using std::wcout;
    wcout<< L"DpCmd 4.0.0 "<< L"Engine Version: " 
        << DediVersion::GetDediproVersion(true)
         //<< std::endl
         << std::endl;

    //wcout<< L"Copyright (C) 2006 - 2007 Dediprog.  All rights reserved."
    //    << std::endl << std::endl ;
}



