// FlashCardSettings.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FlashCardSettings.h"
////#include "Preferences.h"
#include "util.h"
#include "DediContext.h"
#include "PreferenceEx.h"

#include "project.h"

#include <iomanip>
#include <map>
#include <boost/assign.hpp>
#include <sstream>

// CFlashCardSettings dialog

IMPLEMENT_DYNAMIC(CFlashCardSettings, CDialog)

CFlashCardSettings::CFlashCardSettings(CProject* project /*=NULL*/)
    : CDialog(CFlashCardSettings::IDD)
    , m_iMaxMemSize(/*iMaxMemSize*/project->getChipSizeInBytes())
    , m_scboSizeSelected(_T(""))
    , m_cSequenceCfg(0x04)
    , m_bVppSupported(FALSE)
    , m_project(project)
{

}

CFlashCardSettings::~CFlashCardSettings()
{
}

void CFlashCardSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CBOSIZELST, m_cboSizeLst);
    DDX_CBString(pDX, IDC_CBOSIZELST, m_scboSizeSelected);
    DDX_Check(pDX, IDC_CHECK1, m_bVppSupported);
}


BEGIN_MESSAGE_MAP(CFlashCardSettings, CDialog)
END_MESSAGE_MAP()


// CFlashCardSettings message handlers

BOOL CFlashCardSettings::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    FillSizeLst();
    CheckButtons();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
void CFlashCardSettings::CheckButtons()
{
    unsigned char c = m_project->get_context().flashcard.FlashCard_Option;

    CheckDlgButton(IDC_CHKBLANKCHECK, c&0x01 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_CHKERASE, c&0x02 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_CHKPROG, c&0x04 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_CHKVERIFY, c&0x08 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_CHKPROTECT, c&0x10 ? BST_CHECKED : BST_UNCHECKED);
}

CString chipSize2GeneralString(size_t t)
{
    std::wstringstream wss;
    wss << t << ((t > 128) ? L" Kbit" : L" Mbit");

    return wss.str().c_str();
}
CString chipSize2AT45String(size_t t)
{
    std::wstringstream wss;
    wss << L"AT45DB" << std::setfill(L'0')<<std::setw(2)
        << t << ((64==t) ? L"2D (" : L"1D (") 
        << t << L"Mbit + " << t*32 <<L"Kbit Mode Only)";

    return wss.str().c_str();
}

void CFlashCardSettings::FillSizeLst()
{
    m_cboSizeLst.ResetContent();

    switch(m_iMaxMemSize)
    {
        case 0x1000000: // 128Mb   
            m_cboSizeLst.AddString(chipSize2GeneralString(128));
        case 0x800000: // 64Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(64));
            m_cboSizeLst.AddString(chipSize2AT45String(64));
        case 0x400000: // 32Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(32));
            m_cboSizeLst.AddString(chipSize2AT45String(32));
        case 0x200000: // 16Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(16)); 
            m_cboSizeLst.AddString(chipSize2AT45String(16));
        case 0x100000: // 8Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(8));
            m_cboSizeLst.AddString(chipSize2AT45String(8));
        case 0x80000: // 4Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(4));  
            m_cboSizeLst.AddString(chipSize2AT45String(4));
        case 0x40000: // 2Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(2));
            m_cboSizeLst.AddString(chipSize2AT45String(2));
        case 0x20000: // 1Mbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(1));
            m_cboSizeLst.AddString(chipSize2AT45String(1));
        case 0x10000: // 512Kbit   
            m_cboSizeLst.AddString(chipSize2GeneralString(512)); 
        default:  
            ;
    }
    m_cboSizeLst.SetCurSel(0);
}

size_t CFlashCardSettings::GetSelSize()  const
{
    return m_iSelSize;

}
unsigned char CFlashCardSettings::GetSequenceCfg()  const
{
    return m_cSequenceCfg;
}

size_t GetSize(const CString& s)
{
    using namespace boost::assign;
    
    std::map<CString, size_t> dic = map_list_of<CString, size_t>
        (chipSize2GeneralString(128), 128<<17)
        (chipSize2GeneralString(64), 64<<17)
        (chipSize2GeneralString(32), 32<<17)
        (chipSize2GeneralString(16), 16<<17)
        (chipSize2GeneralString(8), 8<<17)
        (chipSize2GeneralString(4), 4<<17)
        (chipSize2GeneralString(2), 2<<17)
        (chipSize2GeneralString(1), 1<<17)
        (chipSize2GeneralString(512),1<<16)
        (chipSize2AT45String(64), (64<<17)/256*264)
        (chipSize2AT45String(32), (32<<17)/256*264)
        (chipSize2AT45String(16), (16<<17)*264/256)
        (chipSize2AT45String(8), (8<<17)*264/256)
        (chipSize2AT45String(4), (4<<17)*264/256)
        (chipSize2AT45String(2), (2<<17)*264/256)
        (chipSize2AT45String(1), (1<<17)*264/256);

    return  dic[s];

}

bool CFlashCardSettings::isAT45() const
{
    return !(-1 ==  m_scboSizeSelected.Find(L"AT45"));
}

void CFlashCardSettings::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateData(true);

    Context::CDediContext& context = m_project->get_context();

    m_cSequenceCfg = IsDlgButtonChecked(IDC_CHKBLANKCHECK) ? (m_cSequenceCfg|0x01) :(m_cSequenceCfg & 0xFE) ;
    m_cSequenceCfg = IsDlgButtonChecked(IDC_CHKERASE) ? (m_cSequenceCfg|0x02) :(m_cSequenceCfg & 0xFD) ;
    m_cSequenceCfg = IsDlgButtonChecked(IDC_CHKPROG) ? (m_cSequenceCfg|0x04) :(m_cSequenceCfg & 0xFB) ;
    m_cSequenceCfg = IsDlgButtonChecked(IDC_CHKVERIFY) ? (m_cSequenceCfg|0x08) :(m_cSequenceCfg & 0xF7) ;
    m_cSequenceCfg = IsDlgButtonChecked(IDC_CHKPROTECT) ? (m_cSequenceCfg|0x10) :(m_cSequenceCfg & 0xEF) ;

    //dedi_preference::CPreference *preference = dedi_preference::CPreference::getInstance();

    //preference->setInt(dedi_preference::FlashCard_Option, m_cSequenceCfg);
    //preference->setInt(dedi_preference::FlashCard_UsingVpp, IsDlgButtonChecked(IDC_ENABLE_VPP_ACC));
    context.flashcard.FlashCard_Option = m_cSequenceCfg;
    context.flashcard.FlashCard_UsingVpp = IsDlgButtonChecked(IDC_ENABLE_VPP_ACC) > 0;


	size_t sel_size = GetSize(m_scboSizeSelected);
    //preference->setInt(dedi_preference::FlashCard_TargetSizeInBytes, sel_size);
    context.flashcard.FlashCard_TargetSizeInBytes = sel_size;

    size_t sizePostFix = sel_size >> 17;

    std::wstringstream wss;
    wss.str(L"");
    wss << (isAT45() ? L"AT45TFIT":L"TFIT")   << sizePostFix << ((IsVppSupported()) ? L"VPP.bin" : L".bin");
    //preference->setString(dedi_preference::FlashCard_TFITFile, wss.str().c_str());
    context.flashcard.FlashCard_TFITFile = wss.str().c_str();

    wss.str(L"");
    wss << L"TFIT"  << sizePostFix << ((IsVppSupported()) ? L"VPPold.bin" : L"old.bin");
    //preference->setString(dedi_preference::FlashCard_TFITFileForV116Below, wss.str().c_str());
    context.flashcard.FlashCard_TFITFileForV116Below = wss.str().c_str();

    //PreferenceEx::CPreferenceWriter().persist_contexts(context);
    CDialog::OnOK();
}

