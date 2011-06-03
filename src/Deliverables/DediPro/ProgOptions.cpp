// ProgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProgOptions.h"
#include "BatchOptions.h"
#include "dedicontext.h"
#include "numeric_conversion.h"
// CProgOptions dialog

IMPLEMENT_DYNAMIC(CProgOptions, CPropertyPage)

CProgOptions::CProgOptions(Context::CDediContext& context)
//: CDialog(CProgOptions::IDD, pParent)
: CPropertyPage(CProgOptions::IDD/*, pParent*/)
, m_length(_T(""))
, m_addrFrom(_T(""))
, m_context(context)
{
    //using namespace dedi_preference;
    //m_partialProgram = (m_bShowAll) ? Prog_PartialProgram : Batch_PartialProgram;
    //m_CustomizedAddrFrom = Prog_CustomizedAddrFrom;
    //m_CustomizedDataLength = Prog_CustomizedDataLength;
}

CProgOptions::~CProgOptions()
{
}

void CProgOptions::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_LENGTH, m_length);
    DDV_MaxChars(pDX, m_length, 9);
    DDX_Text(pDX, IDC_ADDR_FROM, m_addrFrom);
    DDV_MaxChars(pDX, m_addrFrom, 8);
    DDX_Control(pDX, IDC_NOTE, m_note);
    DDX_Check(pDX, IDC_CHKFILL_WITH_00, m_bPadForWholeChipProgram);
}


BEGIN_MESSAGE_MAP(CProgOptions, CPropertyPage)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_OPTION_WHOLECHIP, IDC_OPTION_PARTIALCHIP, &CProgOptions::OnOpt)
    ON_CONTROL_RANGE(EN_CHANGE, IDC_ADDR_FROM, IDC_LENGTH, &CProgOptions::OnAddrChange)
END_MESSAGE_MAP()


// CProgOptions message handlers

void CProgOptions::OnOK()
{
    UpdateData(true);
    SaveAll();

    //CDialog::OnOK();
}

void CProgOptions::OnAddrChange(UINT id)
{
    UpdateData(true);
}

void CProgOptions::OnOpt(UINT id)
{
    bool bEnabled;

    bEnabled = (id == IDC_OPTION_WHOLECHIP)? false:true;

    GetDlgItem(IDC_ADDR_FROM)->EnableWindow(bEnabled);
    GetDlgItem(IDC_LENGTH)->EnableWindow(bEnabled);
    GetDlgItem(IDC_CHKFILL_WITH_00)->EnableWindow(!bEnabled);
}

BOOL CProgOptions::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  Add extra initialization here

    m_note.SetTextColor(colRed)
        .SetTransparent(true)
        .SetText(
        L"* Settings in this page are only meant for singly fired program instructions, "
        L"which must be appplied to a blank(i.e. erased) area.\r\n\r\n"
        L"* To configure program instructions that are embedded in composite sequences, See \"Batch Configurations\". ");

    bool b(m_context.Prog_PartialProgram);
    CheckRadioButton(IDC_OPTION_WHOLECHIP, IDC_OPTION_PARTIALCHIP, b ? IDC_OPTION_PARTIALCHIP : IDC_OPTION_WHOLECHIP);
    OnOpt(b ? IDC_OPTION_PARTIALCHIP : IDC_OPTION_WHOLECHIP);

    FillWithCustomValues();
    OnAddrChange(0);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CProgOptions::FillWithCustomValues()
{

    m_addrFrom  = numeric_conversion::to_hex_string<6>(m_context.Prog_CustomizedAddrFrom, false).c_str();
    m_length    = numeric_conversion::to_hex_string<6>(m_context.Prog_CustomizedDataLength, false).c_str();
	m_bPadForWholeChipProgram = (Context::CDediContext::NO_PADDING != m_context.Prog_PaddingCharacterWhenProgrammingWholeChip); 

    if(L"0" == m_length) m_length = L"";

    UpdateData(false);
}

void CProgOptions::SaveAll()
{
    UpdateData(true);

    m_context.Prog_CustomizedAddrFrom = numeric_conversion::hexstring_to_size_t(m_addrFrom.GetString());
    m_context.Prog_CustomizedDataLength = numeric_conversion::hexstring_to_size_t(m_length.GetString());

	m_context.Prog_PaddingCharacterWhenProgrammingWholeChip = m_bPadForWholeChipProgram ? 0x00 : 0xFF;

    m_context.Prog_PartialProgram = IDC_OPTION_PARTIALCHIP == GetCheckedRadioButton(IDC_OPTION_WHOLECHIP, IDC_OPTION_PARTIALCHIP);
}
