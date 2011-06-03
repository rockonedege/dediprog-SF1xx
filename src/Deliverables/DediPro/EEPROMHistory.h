#pragma once

#include "resource.h"
#include <boost/utility.hpp>


class CDediProView;

// CEEPROMHistory dialog

class CEEPROMHistory : public CDialog, private boost::noncopyable
{
    DECLARE_DYNAMIC(CEEPROMHistory)

public:
    CEEPROMHistory(CWnd* pParent = NULL);   // standard constructor
    virtual ~CEEPROMHistory();

// Dialog Data
    enum { IDD = IDD_EEPROM_HISTORY };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
private:
    UINT m_iSuccess;
    UINT m_iErrErase;
    UINT m_iErrProg;
    UINT m_iErrVerify;
    CDediProView* m_pParentView;
private:
    afx_msg void OnReset();
    virtual BOOL OnInitDialog();
};


