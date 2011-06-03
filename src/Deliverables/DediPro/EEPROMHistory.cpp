// EEPROMHistory.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EEPROMHistory.h"
#include ".\Core\Chip\ProgBoard.h"
#include "project.h"
#include "DediProView.h"

// CEEPROMHistory dialog

IMPLEMENT_DYNAMIC(CEEPROMHistory, CDialog)

CEEPROMHistory::CEEPROMHistory(CWnd* pParent /*=NULL*/)
    : CDialog(CEEPROMHistory::IDD, pParent)
    , m_iSuccess(0)
    , m_iErrErase(0)
    , m_iErrProg(0)
    , m_iErrVerify(0)
    , m_pParentView((CDediProView*) pParent)
{
}

CEEPROMHistory::~CEEPROMHistory()
{
}

void CEEPROMHistory::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_CNTSUCCESS, m_iSuccess);
    DDX_Text(pDX, IDC_CNTERRERASE, m_iErrErase);
    DDX_Text(pDX, IDC_CNTERRPROG, m_iErrProg);
    DDX_Text(pDX, IDC_CNTERRVERIFY, m_iErrVerify);

}


BEGIN_MESSAGE_MAP(CEEPROMHistory, CDialog)
    ON_BN_CLICKED(IDC_BTNRESET, &CEEPROMHistory::OnReset)
END_MESSAGE_MAP()


// CEEPROMHistory message handlers

void CEEPROMHistory::OnReset()
{
    programmer_board::CProgBoard board(m_pParentView->m_pProject.get());

    if(m_pParentView)
    {
        vector<unsigned char> vc;
        if( board.ResetCntrEEPROMOnCard())
        {
            m_iSuccess = 0;
            m_iErrErase = 0;
            m_iErrProg = 0;
            m_iErrVerify = 0;
            UpdateData(FALSE);
            MessageBox(L"Reset Completed.");
        }
        else
        {
            MessageBox(L"Reset Failed.");
        }
    }
}

BOOL CEEPROMHistory::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    programmer_board::CProgBoard board(m_pParentView->m_pProject.get());
    if(m_pParentView)
    {
        vector<unsigned char> vc;
        if(board.ReadAllFromEEPROMOnCard(vc))
        {
            if(vc.size() < 0x30) return TRUE;

            m_iSuccess = ((UINT)vc[0x20])<<24 
                         | ((UINT)vc[0x21])<<16
                         | ((UINT)vc[0x22])<<8
                         | ((UINT)vc[0x23])
                         & 0xFFFFFFFF;

            m_iErrErase = ((UINT)vc[0x24])<<24 
                         | ((UINT)vc[0x25])<<16
                         | ((UINT)vc[0x26])<<8
                         | ((UINT)vc[0x27])
                         & 0xFFFFFFFF;

            m_iErrProg = ((UINT)vc[0x28])<<24 
                         | ((UINT)vc[0x29])<<16
                         | ((UINT)vc[0x2A])<<8
                         | ((UINT)vc[0x2B])
                         & 0xFFFFFFFF;

            m_iErrVerify = ((UINT)vc[0x2C])<<24 
                         | ((UINT)vc[0x2D])<<16
                         | ((UINT)vc[0x2E])<<8
                         | ((UINT)vc[0x2F])
                         & 0xFFFFFFFF;

            if(0xFFFFFFFF == m_iSuccess) m_iSuccess = 0;
            if(0xFFFFFFFF == m_iErrErase) m_iErrErase = 0;
            if(0xFFFFFFFF == m_iErrProg) m_iErrProg = 0;
            if(0xFFFFFFFF == m_iErrVerify) m_iErrVerify = 0;
        }
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
