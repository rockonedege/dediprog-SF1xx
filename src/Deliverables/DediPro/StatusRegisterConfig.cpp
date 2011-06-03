// StatusRegisterConfig.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "StatusRegisterConfig.h"
#include "project.h"

#include <sstream>
#include <iomanip>

#include "numeric_conversion.h"

using std::wstringstream;


// CStatusRegisterConfig dialog

IMPLEMENT_DYNAMIC(CStatusRegisterConfig, CPropertyPage)

CStatusRegisterConfig::CStatusRegisterConfig(CProject* p, CWnd* pParent /*=NULL*/)
	: CPropertyPage(CStatusRegisterConfig::IDD)
    , m_project(p)
    , m_newValue(_T("00"))
{

}

CStatusRegisterConfig::~CStatusRegisterConfig()
{
}

void CStatusRegisterConfig::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_VALUE_CURRENT, m_currentValue);
    DDX_Control(pDX, IDC_REFRESH, m_refreshCurrentValue);
    DDX_Text(pDX, IDC_NEW_VALUE, m_newValue);
	DDV_MaxChars(pDX, m_newValue, 2);
}


BEGIN_MESSAGE_MAP(CStatusRegisterConfig, CPropertyPage)
    ON_BN_CLICKED(IDC_REFRESH, &CStatusRegisterConfig::OnRefreshValue)
    ON_BN_CLICKED(IDC_WRITE_NEW_VALUE, &CStatusRegisterConfig::OnWriteNewValue)
END_MESSAGE_MAP()

// CStatusRegisterConfig message handlers

BOOL CStatusRegisterConfig::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  Add extra initialization here
    m_currentValue.SetFontBold(TRUE).SetTextColor(colBlue).SetBkColor(colWhite);

    OnRefreshValue();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CStatusRegisterConfig::OnRefreshValue()
{
    // TODO: Add your control notification handler code here
    unsigned char c;
    if(m_project->readSR(c))
    {
        wstringstream wss;
        wss << std::setfill(L'0')<<std::uppercase<<std::setw(2)<<std::hex << c;
        m_currentValue.SetText(wss.str().c_str());
    }
    else
    {
        m_currentValue.SetText(L"unavailable");
    }
}

void CStatusRegisterConfig::OnWriteNewValue()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    
    if(m_project->writeSR(static_cast<unsigned char>(numeric_conversion::hexstring_to_size_t(m_newValue.GetString()))))
    {
        OnRefreshValue();
    }
    else
    {
        MessageBox(L"Failed to Write Status Register.");
    }
}
