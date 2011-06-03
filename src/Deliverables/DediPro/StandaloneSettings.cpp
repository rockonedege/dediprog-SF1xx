b// StandaloneSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "StandaloneSettings.h"
#include "PreferenceEx.h"

#include <iomanip>
#include <string>
#include <sstream>
#include <set>

#include <boost/assign.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>

#include "dedicontext.h"

extern CString chipSize2GeneralString(size_t t);
//{
//    std::wstringstream wss;
//    wss << t << ((t > 128) ? L" Kbit" : L" Mbit");
//
//    return wss.str().c_str();
//}
extern CString chipSize2AT45String(size_t t);
//{
//    std::wstringstream wss;
//    wss << L"AT45DB" << std::setfill(L'0')<<std::setw(2)
//        << t << ((64==t) ? L"2D (" : L"1D (") 
//        << t << L"Mbit + " << t*32 <<L"Kbit Mode Only)";
//
//    return wss.str().c_str();
//}

size_t GetSize(const CString& s);
//{
//    using namespace boost::assign;
//    
//    std::map<CString, size_t> dic = map_list_of<CString, size_t>
//        (chipSize2GeneralString(128), 128<<17)
//        (chipSize2GeneralString(64), 64<<17)
//        (chipSize2GeneralString(32), 32<<17)
//        (chipSize2GeneralString(16), 16<<17)
//        (chipSize2GeneralString(8), 8<<17)
//        (chipSize2GeneralString(4), 4<<17)
//        (chipSize2GeneralString(2), 2<<17)
//        (chipSize2GeneralString(1), 1<<17)
//        (chipSize2GeneralString(512),1<<16)
//        (chipSize2AT45String(64), (64<<17)/256*264)
//        (chipSize2AT45String(32), (32<<17)/256*264)
//        (chipSize2AT45String(16), (16<<17)*264/256)
//        (chipSize2AT45String(8), (8<<17)*264/256)
//        (chipSize2AT45String(4), (4<<17)*264/256)
//        (chipSize2AT45String(2), (2<<17)*264/256)
//        (chipSize2AT45String(1), (1<<17)*264/256);
//
//    return  dic[s];
//}


// CStandaloneSettings dialog

IMPLEMENT_DYNAMIC(CStandaloneSettings, CDialog)

CStandaloneSettings::CStandaloneSettings(size_t iMaxMemSize, Context::CDediContext& context)
: CDialog(CStandaloneSettings::IDD)
, m_cSequenceCfg(0)
, m_iMaxMemSize(iMaxMemSize)
, m_context(context)
, m_seltype(_T(""))
, m_finder(path_util::GetCommonAppDataPath())

{
}

CStandaloneSettings::~CStandaloneSettings()
{
}

void CStandaloneSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPELIST, m_typelist);
	DDX_LBString(pDX, IDC_TYPELIST, m_seltype);
	DDX_Control(pDX, IDC_FILTERLIST, m_listFilter);
}

BEGIN_MESSAGE_MAP(CStandaloneSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CStandaloneSettings::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_FILTERLIST, &CStandaloneSettings::OnFilter)
END_MESSAGE_MAP()

// CStandaloneSettings message handlers

BOOL CStandaloneSettings::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
	initListBox();
    load();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CStandaloneSettings::initListBox()
{
	m_listFilter.AddString(L"< ALL >");

	std::set<wstring> filters;

	BOOST_FOREACH(const tfit::item& i, m_finder.get_typelist())
    {
        m_typelist.AddString(i.type.c_str());

		if(filters.insert(i.manufacturer).second) m_listFilter.AddString(i.manufacturer.c_str());
		if(boost::algorithm::icontains(m_context.flashcard.FlashCard_TFITFile, i.type))
		{
			m_typelist.SetCurSel(m_typelist.GetCount() - 1);
		}
    }

	m_listFilter.SetCurSel(0);

}
void CStandaloneSettings::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateData(true);
    save();

    CDialog::OnOK();
}

void CStandaloneSettings::load()
{
    size_t idx = m_context.flashcard.FlashCard_OptionIndex;
    CheckRadioButton(IDC_SA_EPV, IDC_SA_PV, IDC_SA_EPV + idx);
    CheckDlgButton( IDC_ENABLE_VPP_ACC, m_context.flashcard.FlashCard_UsingVpp); 
}

void CStandaloneSettings::save()
{

    boost::array<int, 3> opt = 
    {
            ERASE | PROGRAM | VERIFY,
            BLANK_CHECK | ERASE | PROGRAM | VERIFY,
            PROGRAM | VERIFY
    };


    size_t idx = GetCheckedRadioButton(IDC_SA_EPV, IDC_SA_PV) - IDC_SA_EPV;
    m_context.flashcard.FlashCard_OptionIndex = idx;

    m_cSequenceCfg = opt[idx];
    m_context.flashcard.FlashCard_Option = m_cSequenceCfg;

    //m_context.flashcard.FlashCard_UsingVpp = IsDlgButtonChecked(IDC_ENABLE_VPP_ACC);
	m_context.flashcard.FlashCard_UsingVpp = tfit::CFindTFIT::is_vpp_version(m_seltype.GetString());
	if(!m_seltype.IsEmpty())
	{
		tfit::item	i = m_finder.get_selected(m_seltype.GetString());
			
	        m_context.flashcard.FlashCard_TargetType = m_seltype.GetString();
			m_context.flashcard.FlashCard_TFITFile = i.fullname;
			m_context.flashcard.FlashCard_TargetSizeInBytes = i.size;
			
	}

    m_context.flashcard.FlashCard_TFITFileForV116Below = m_context.flashcard.FlashCard_TFITFile;
}

void CStandaloneSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CStandaloneSettings::OnFilter()
{
	// TODO: Add your control notification handler code here
	CString filter;
	m_listFilter.GetText(m_listFilter.GetCurSel(), filter);

	m_typelist.ResetContent();
	BOOST_FOREACH(const tfit::item& i, m_finder.get_typelist())
    {
		if(filter.Compare(i.manufacturer.c_str()) == 0 || 0 == m_listFilter.GetCurSel()) 
		{
			m_typelist.AddString(i.type.c_str());

			if(boost::algorithm::icontains(m_context.flashcard.FlashCard_TFITFile, i.type))
			{
				m_typelist.SetCurSel(m_typelist.GetCount() - 1);
			}
		}
    }
}
