#pragma once
#include "afxwin.h"


namespace Context { struct CDediContext;}

// CFreescale dialog

class CFreescale : public CPropertyPage
{
	DECLARE_DYNAMIC(CFreescale)

public:
	CFreescale(Context::CDediContext& context);
	virtual ~CFreescale();

// Dialog Data
	enum { IDD = IDD_CONFIG_FREESCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void OnOK();

	DECLARE_MESSAGE_MAP()
public:
private:
	Context::CDediContext& m_context;
	CString m_cr;
};
