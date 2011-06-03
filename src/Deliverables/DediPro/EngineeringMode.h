#pragma once
#include ".\3rd\label\label.h"


class CProject;

// CEngineeringMode dialog

class CEngineeringMode : public /*CDialog*/ CPropertyPage
{
	DECLARE_DYNAMIC(CEngineeringMode)

public:
	CEngineeringMode(CProject* p, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEngineeringMode();

// Dialog Data
	enum { IDD = IDD_CONFIG_ENGINEERING_MODE };

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
    afx_msg void OnSend();
	afx_msg void OnNeedReturn();
    afx_msg void OnNeedMonitor();
    void    RefreshSR();

private:
    CProject* m_p;

	CLabel m_in;
    CLabel m_srValue;
    CLabel m_warning;
	CString m_out_stream;
public:
};
