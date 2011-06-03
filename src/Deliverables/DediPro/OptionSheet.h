#pragma once
#include "afxpropertysheet.h"

class COptionSheet :
    public CMFCPropertySheet
{
public:
    COptionSheet(LPCTSTR pszCaption);
    ~COptionSheet(void);
};
