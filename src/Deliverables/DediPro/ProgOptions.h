#pragma once
#include "PreferenceEx.h"
#include ".\3rd\label\label.h"

#include <boost\tuple\tuple.hpp>
using boost::tuples::tuple;

namespace Context{struct CDediContext;}

// CProgOptions dialog

class CProgOptions : public /*CDialog*/ CPropertyPage
{
public:

	DECLARE_DYNAMIC(CProgOptions)

public:
	CProgOptions(Context::CDediContext& context);   // standard constructor
	virtual ~CProgOptions();

// Dialog Data
	enum { IDD = IDD_CONFIG_PROG };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    void OnSelectPadding();
    void OnOK();
    void OnOpt(UINT id);
    void OnAddrChange(UINT id);
    void FillWithCustomValues();
    void SaveAll();

DECLARE_MESSAGE_MAP()
private:

    CString m_length;
    CString m_addrFrom;

    //bool m_bRunNow;
    //bool m_bShowAll;
private:
    CLabel m_note;
private:
    BOOL m_bPadForWholeChipProgram;
    Context::CDediContext& m_context;
};
