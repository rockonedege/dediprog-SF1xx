// Freescale.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "Freescale.h"
#include "dedicontext.h"
#include "numeric_conversion.h"

// CFreescale dialog

IMPLEMENT_DYNAMIC(CFreescale, CPropertyPage)

CFreescale::CFreescale(Context::CDediContext& context)
	: CPropertyPage(CFreescale::IDD)
	, m_context(context)
	, m_cr(numeric_conversion::to_hex_string<2>(m_context.chip.mcf_config_register, false).c_str())
{

}

CFreescale::~CFreescale()
{
}

void CFreescale::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONFIG_REGISTER, m_cr);
}


BEGIN_MESSAGE_MAP(CFreescale, CPropertyPage)
END_MESSAGE_MAP()


// CFreescale message handlers

BOOL CFreescale::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

//	m_cr = numeric_conversion::to_hex_string<2>(m_context.chip.mcf_config_register, false).c_str();
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFreescale::OnOK()
{
	UpdateData();
	m_context.chip.mcf_config_register = numeric_conversion::hexstring_to_size_t<wchar_t>(m_cr.GetString());

	CPropertyPage::OnOK();
}
