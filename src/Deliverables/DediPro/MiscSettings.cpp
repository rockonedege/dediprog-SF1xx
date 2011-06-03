// MiscSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "MiscSettings.h"
#include "PreferenceEx.h"
#include "dedicontext.h"

#include <boost/array.hpp>

// CMiscSettings dialog

IMPLEMENT_DYNAMIC(CMiscSettings, CPropertyPage)

CMiscSettings::CMiscSettings(Context::CDediContext& context)
	: CPropertyPage(CMiscSettings::IDD)
    , m_context(context)
{

}

CMiscSettings::~CMiscSettings()
{
}

void CMiscSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiscSettings, CPropertyPage)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_VCC3_5, IDC_VCC1_8, &CMiscSettings::OnSelectVcc)
END_MESSAGE_MAP()


// CMiscSettings message handlers

BOOL CMiscSettings::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  Add extra initialization here

    initVpp();
    initVcc();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CMiscSettings::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class

    m_context.power.Misc_ApplyVpp = IsDlgButtonChecked(IDC_CHK_VPP) == BST_CHECKED;

    CPropertyPage::OnOK();
}

void CMiscSettings::OnSelectVcc(UINT id)
{
    // TODO: Add your control notification handler code here
    boost::array<power::VCC_VALUE, 2> vcc = 
    {
        power::vcc3_5V,
        power::vcc1_8V
    };

    m_context.power.vcc = vcc[id - IDC_VCC3_5];
}

void CMiscSettings::initVcc()
{
    if(power::vcc3_5V == m_context.power.vcc)
    {
        CheckRadioButton(IDC_VCC3_5, IDC_VCC1_8, IDC_VCC3_5);
    }
    else if (power::vcc1_8V == m_context.power.vcc)
    {
        CheckRadioButton(IDC_VCC3_5, IDC_VCC1_8, IDC_VCC1_8);
    }
    else
    {}
}

void CMiscSettings::initVpp()
{
    bool applyVpp = m_context.power.Misc_ApplyVpp;
    CheckDlgButton(IDC_CHK_VPP, applyVpp ? BST_CHECKED : BST_UNCHECKED);
}
