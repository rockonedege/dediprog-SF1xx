#pragma once
#include ".\3rd\label\label.h"
//#include ".\3rd\foobutton\foobutton.h"


class CProject;

// CStatusRegisterConfig dialog

class CStatusRegisterConfig : public CPropertyPage
{
	DECLARE_DYNAMIC(CStatusRegisterConfig)

public:
	CStatusRegisterConfig(CProject* p, CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatusRegisterConfig();

// Dialog Data
	enum { IDD = IDD_CONFIG_SR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    void OnOK(){};

    afx_msg void OnRefreshValue();
    afx_msg void OnWriteNewValue();

	DECLARE_MESSAGE_MAP()
private:
    CLabel m_currentValue;
    /*FooButton*/CButton m_refreshCurrentValue;
private:
    CProject* m_project;
    CString m_newValue;
};
