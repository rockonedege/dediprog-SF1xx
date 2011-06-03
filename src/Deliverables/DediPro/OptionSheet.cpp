#include "StdAfx.h"
#include "resource.h"
#include "OptionSheet.h"

COptionSheet::COptionSheet(LPCTSTR pszCaption)
    :CMFCPropertySheet(pszCaption)
{
    SetLook(CMFCPropertySheet::PropSheetLook_OutlookBar);
    SetIconsList (IDB_ADVCFG, 48, RGB(0,0,0));
}

COptionSheet::~COptionSheet(void)
{
}
