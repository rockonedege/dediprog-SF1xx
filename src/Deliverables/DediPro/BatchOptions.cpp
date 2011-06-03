// AutoSequence.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BatchOptions.h"
#include "ProgOptions.h"
#include "DediContext.h"

#include <sstream>

#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

using std::wstringstream;
using boost::lexical_cast;

// CAutoSequence dialog

IMPLEMENT_DYNAMIC(CAutoSequence, CPropertyPage)

CAutoSequence::CAutoSequence(Context::CDediContext& context)
	: CPropertyPage(CAutoSequence::IDD)
	//: CDialog(CAutoSequence::IDD, pParent)
    , m_context(context)
    , m_requireVerify(FALSE)
    , m_option(1)
    , m_addrFrom(L"0x0")
{

}

CAutoSequence::~CAutoSequence()
{
}

void CAutoSequence::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
//    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LST_SEQUENCES, m_lstSequences);
    DDX_Check(pDX, IDC_CHKVERIFY, m_requireVerify);
    DDX_Check(pDX, IDC_CHKRELOAD_FILE, m_requireReloadFile);
    DDX_Control(pDX, IDC_LST_FILES, m_lstFiles);
    DDX_Text(pDX, IDC_BATCH_ADDR_FROM, m_addrFrom);
    DDV_MaxChars(pDX, m_addrFrom, 6);

}


BEGIN_MESSAGE_MAP(CAutoSequence, CPropertyPage)
    ON_CONTROL_RANGE(BN_CLICKED,IDC_OPTPRESET_CUSTOM, IDC_OPTPRESET_DOWNLOAD_PARTIAL_FILE, &CAutoSequence::OnOption)
    ON_BN_CLICKED(IDC_CHKVERIFY, &CAutoSequence::OnToggleVerify)
    ON_BN_CLICKED(IDC_CHKRELOAD_FILE, &CAutoSequence::OnToggleReloadFile)
    ON_CONTROL_RANGE(EN_CHANGE, IDC_BATCH_ADDR_FROM, IDC_LENGTH, &CAutoSequence::OnAddrChange)
END_MESSAGE_MAP()

void CAutoSequence::OnAddrChange(UINT id)
{
    UpdateData(true);
}

void CAutoSequence::InitListStyles()
{
    //m_lstSequences.SetExtendedStyle(/*LVS_EX_BORDERSELECT | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT */);
    
    CRect rect;
    m_lstSequences.GetClientRect(&rect);
    int nColInterval = rect.Width()/4;

    m_lstSequences.InsertColumn(0, _T("Steps"), LVCFMT_LEFT, nColInterval*1);
    m_lstSequences.InsertColumn(1, _T("Actions"), LVCFMT_LEFT, rect.Width()-1*nColInterval);

    //m_lstFiles.SetExtendedStyle(LVS_EX_BORDERSELECT /*| LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER*/ | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_lstFiles.GetClientRect(&rect);
    nColInterval = rect.Width()/4;

    m_lstFiles.InsertColumn(0, _T("Types"), LVCFMT_LEFT, nColInterval*1);
    m_lstFiles.InsertColumn(1, _T("Pathes"), LVCFMT_LEFT, rect.Width()-1*nColInterval);

    //m_lstSequences.EnableWindow(false);

}

void CAutoSequence::FillFilesList()
{
    m_lstFiles.DeleteAllItems();

    // Use the LV_ITEM structure to insert the items
    LVITEM lvi;
    lvi.mask =  LVIF_TEXT ;

    lvi.iItem = 0;

    // Set subitem 0
    lvi.iSubItem = 0;
    lvi.pszText = L"Current File in Buffer:";
    m_lstFiles.InsertItem(&lvi);

//    CString currentFile(preference->getString(dedi_preference::CurrentFileInBuffer/*L"CurrentFileInBuffer"*/));
    wstring currentFile(m_context.file.CurrentFileInBuffer);
    if(currentFile.empty()) currentFile = L"No file in buffer currently";
    // Set subitem 1
    lvi.iSubItem =1;
    lvi.pszText = (LPTSTR)(LPCTSTR)(currentFile.c_str());
    m_lstFiles.SetItem(&lvi);

    //lvi.iItem = 1;

    //// Set subitem 0
    //lvi.iSubItem = 0;
    //lvi.pszText = L"File in the Last Launch";
    //m_lstFiles.InsertItem(&lvi);

    //CString lastFile(preference.GetUsersValue(L"LastLoadedFile"));
    //if(lastFile.IsEmpty()) lastFile = "No file saved path";
    //// Set subitem 1
    //lvi.iSubItem =1;
    //lvi.pszText = (LPTSTR)(LPCTSTR)(lastFile.GetString());
    //m_lstFiles.SetItem(&lvi);

}

void CAutoSequence::FillSequencesList(size_t option)
{
    enum
    {
        CUSTOM = 0,
        DOWNLOAD_WHOLE_CHIP,
        DOWNLOAD_WHOLE_CHIP_WITHOUT_BLANKCHECK,
        DOWNLOAD_PARTIAL_CHIP
    };

    boost::array<wstring, 7> ar =
    {
        L"Reload File.",
        L"Read from the chip",
        L"Blank Check",
        L"Erase Chip(if not blank)",
        L"Erase Chip",
        L"Program Chip ",
        L"Verify after operation completes"
    };

    m_lstSequences.DeleteAllItems();

    if(m_requireReloadFile)  AddOneSequenceRow(ar[0]);

    if(DOWNLOAD_WHOLE_CHIP_WITHOUT_BLANKCHECK != option) 
    {
        AddOneSequenceRow(ar[1]);
        AddOneSequenceRow(ar[2]);
        AddOneSequenceRow(ar[3]);
    }
    else
    {
        AddOneSequenceRow(ar[4]);
    }
    AddOneSequenceRow(ar[5]);
    
    UpdateData(true);

    if(m_requireVerify) AddOneSequenceRow(ar[6]);

}

void CAutoSequence::RetrieveSavedOptions()
{
    unsigned int sequence = 0;
    size_t option = 0;

    //CPreference *preference = CPreference::getInstance();
    //m_requireReloadFile = preference->getBool(dedi_preference::Batch_ReloadFile);
    m_requireReloadFile = m_context.Batch_ReloadFile;
    CheckDlgButton(IDC_CHKRELOAD_FILE,  m_requireReloadFile ? BST_CHECKED : BST_UNCHECKED);


    m_requireVerify = m_context.Batch_VerifyAfterCompletion;
    CheckDlgButton(IDC_CHKVERIFY,m_requireVerify ? BST_CHECKED : BST_UNCHECKED);

    if(0xFF != m_context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck) CheckDlgButton(IDC_CHK_FILL_WITH_00_NO_BLKCHK, BST_CHECKED);

    if(0xFF != m_context.Batch_PaddingCharacterWhenDownloadWithBlankCheck) CheckDlgButton(IDC_CHK_FILL_WITH_00_FOR_BLKCHK, BST_CHECKED);


    option = m_context.Batch_SelectedOption;
    CheckRadioButton(IDC_OPTPRESET_CUSTOM, IDC_OPTPRESET_DOWNLOAD_PARTIAL_FILE, IDC_OPTPRESET_CUSTOM + static_cast<unsigned int>(option));
    FillSequencesList(option);

    m_addrFrom.Format(L"%x",m_context.Batch_CustomizedAddrFrom);
    SetDlgItemText(IDC_BATCH_ADDR_FROM, m_addrFrom);
    


    return ;
}

BOOL CAutoSequence::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    //CDialog::OnInitDialog();

    InitListStyles();
    FillFilesList();
    RetrieveSavedOptions();

    SetModified(TRUE);
  
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoSequence::SaveUserOptions()
{
    UpdateData(true);

    m_context.Batch_VerifyAfterCompletion = TRUE == m_requireVerify;
    m_context.Batch_ReloadFile = TRUE == m_requireReloadFile;
   
    size_t option = GetCheckedRadioButton(IDC_OPTPRESET_CUSTOM, IDC_OPTPRESET_DOWNLOAD_PARTIAL_FILE) - IDC_OPTPRESET_CUSTOM;
    m_context.Batch_SelectedOption/*L"Batch_SelectedOption"*/ = option;

    m_context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck = IsDlgButtonChecked(IDC_CHK_FILL_WITH_00_NO_BLKCHK) ? 0x00 : 0xFF;
    m_context.Batch_PaddingCharacterWhenDownloadWithBlankCheck = IsDlgButtonChecked(IDC_CHK_FILL_WITH_00_FOR_BLKCHK) ? 0x00 : 0xFF;

    m_context.Batch_CustomizedAddrFrom = numeric_conversion::hexstring_to_size_t(m_addrFrom.GetString());
	m_context.Batch_PartialProgram = IDC_OPTPRESET_DOWNLOAD_PARTIAL_FILE == GetCheckedRadioButton(IDC_OPTPRESET_DOWNLOAD_WHOLE_FILE, IDC_OPTPRESET_DOWNLOAD_PARTIAL_FILE);
}

void CAutoSequence::OnOK()
{
    SaveUserOptions();

    // CDialog::OnOK();
}

void CAutoSequence::OnOption(UINT id)
{
    m_option = id - IDC_OPTPRESET_CUSTOM;
    FillSequencesList(m_option);

    m_context.Batch_PartialProgram = m_option < 3;
}

void CAutoSequence::OnToggleReloadFile()
{
    UpdateData(true);

    wstring s(L"Reload File.");
    if(m_requireReloadFile)
    {
        AddOneSequenceRow(s, 0); 
    }
    else
    {
        RemoveOneSequenceRow(0);
    }
}

void CAutoSequence::OnToggleVerify()
{
    UpdateData(true);

    wstring s(L"Verify after operation completes.");
    if(m_requireVerify)
    {
        AddOneSequenceRow(s); 
    }
    else
    {
        RemoveOneSequenceRow(m_lstSequences.GetItemCount() - 1);
    }
}

void CAutoSequence::RemoveOneSequenceRow(UINT i/* = -1*/)
{
    m_lstSequences.DeleteItem(i);

    wstringstream ssIdx;
    wstring sIdx;

    // Use the LV_ITEM structure to insert the items
    LVITEM lvi;
    lvi.mask =  LVIF_TEXT ;

    for(int newIdx = 0; newIdx <  m_lstSequences.GetItemCount(); ++newIdx)
    {
        // Set subitem 0
        lvi.iItem = newIdx;
        lvi.iSubItem = 0;

        ssIdx.str(L"");
        ssIdx << newIdx + 1;
        sIdx = ssIdx.str();
        lvi.pszText = (LPTSTR)(LPCTSTR)(sIdx.c_str());
        
        m_lstSequences.SetItem(&lvi);
    }
}

void CAutoSequence::AddOneSequenceRow(wstring s, UINT i/* = -1*/)
{
    unsigned int idx = (-1 == i) ? m_lstSequences.GetItemCount(): i;
    wstringstream ssIdx;
    wstring sIdx;

    ssIdx << idx + 1;
    sIdx = ssIdx.str();

    // Use the LV_ITEM structure to insert the items
    LVITEM lvi;
    lvi.mask =  LVIF_TEXT ;

    lvi.iItem = idx;

    // Set subitem 0
    lvi.iSubItem = 0;
    lvi.pszText = (LPTSTR)(LPCTSTR)(sIdx.c_str());
    m_lstSequences.InsertItem(&lvi);


    // Set subitem 1
    lvi.iSubItem =1;
    lvi.pszText = (LPTSTR)(LPCTSTR)(s.c_str());

    m_lstSequences.SetItem(&lvi);

    if(-1 == i) return;

    // refresh  item indexes
    for(int newIdx = idx + 1; newIdx <  m_lstSequences.GetItemCount(); ++newIdx)
    {
        // Set subitem 0
        lvi.iItem = newIdx;
        lvi.iSubItem = 0;

        wstring text(lexical_cast<wstring>(newIdx + 1));
        lvi.pszText = (LPTSTR)(LPCTSTR)(text.c_str());
        
        m_lstSequences.SetItem(&lvi);
    }

}
