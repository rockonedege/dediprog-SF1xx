// DediPro.h : main header file for the DediPro application
//
#pragma once


#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CDediProApp:
// See DediPro.cpp for the implementation of this class
//

class CDediProApp : public CWinApp
{
public:
    CDediProApp();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileCloseProject();
    afx_msg void OnHelpUsermanual();
    afx_msg void OnHelpLaunchcalculater();
// Overrides
public:
    virtual BOOL InitInstance();
    bool IsSingleInstance();
    void SetTitle(const CString& vendor);
// Implementation
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
public:
};

extern CDediProApp theApp;