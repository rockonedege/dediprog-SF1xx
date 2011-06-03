#pragma once
#include "afxwin.h"

#include "resource.h"

#include "TFITFinder.h"

namespace Context{struct CDediContext;}

// CStandaloneSettings dialog

class CStandaloneSettings : public CDialog
{
    DECLARE_DYNAMIC(CStandaloneSettings)

public:
    CStandaloneSettings(size_t iMaxMemSize,  Context::CDediContext& context);   // standard constructor
    virtual ~CStandaloneSettings();

    // Dialog Data
    enum { IDD = IDD_CONFIG_STANDALONE_BOARD };
    enum 
    {
        BLANK_CHECK = 0x01,
        ERASE       = 0x02,
        PROGRAM     = 0x04,
        VERIFY      = 0x08,
        PROTECT     = 0x10
    };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnFilter();
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

private:
	void	initListBox();
    void	save();
    void	load();
private:
    unsigned char   m_cSequenceCfg;
    size_t          m_iMaxMemSize;
private:
	CListBox	m_listFilter;
    CListBox    m_typelist;
    Context::CDediContext& m_context;
	CString		m_seltype;

	tfit::CFindTFIT m_finder;
};
