// ManualDetection.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "ManualDetection.h"

#include "memory_id.h"
#include "SerialFlashInfoEx.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <set>

const CString ALL_Manu(L"<All>"); 
const CString No_Recognized(L"<Not Recognized>"); 
const CString Auto_Detected(L"<Auto Detected Type(s)>"); 

// CManualDetection dialog

IMPLEMENT_DYNAMIC(CManualDetection, CDialog)

CManualDetection::CManualDetection(const std::vector<memory::memory_id>& auto_ids)
: CDialog(CManualDetection::IDD)
, m_strManu(_T(""))
, m_strChip(_T(""))
, m_ids()
, m_auto_ids(auto_ids)
, m_idx_selected(-1)
{
    ChipInfo::CSortedChipList sorted_list;
    m_ids = sorted_list.get_supported_chiplist_ordered_by_manufacturer();
}

CManualDetection::~CManualDetection()
{
}

void CManualDetection::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHIP_LIST, m_listChip);
    DDX_LBString(pDX, IDC_CHIP_LIST, m_strChip);
    DDX_Control(pDX, IDC_MANU, m_manu_filter);
}


BEGIN_MESSAGE_MAP(CManualDetection, CDialog)
    ON_BN_CLICKED(IDOK, &CManualDetection::OnOk)
    ON_LBN_DBLCLK(IDC_CHIP_LIST, &CManualDetection::OnSelectChip)
    ON_NOTIFY(NM_CLICK, IDC_MANU, &CManualDetection::OnNMClickManu)
END_MESSAGE_MAP()


// CManualDetection message handlers

void CManualDetection::OnOk()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    BOOST_FOREACH(const memory::memory_id& mid, m_ids)
    {
        m_idx_selected++;
        if(/*boost::algorithm::iequals(mid.Manufacturer, m_strManu.GetString()) || m_strManu == ALL_Manu || Auto_Detected == m_strManu) && */
            boost::algorithm::iequals(mid.TypeName, m_strChip.GetString())
            )
        {
            break;
        }
    }

    OnOK();
}

BOOL CManualDetection::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_manu_filter.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
    m_manu_filter.InsertColumn(0, _T("Manufacturer"), LVCFMT_LEFT, 100);
    m_manu_filter.SetColumnWidth(0,LVSCW_AUTOSIZE_USEHEADER);

    InitList();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CManualDetection::OnManuList()
{
   // UpdateData(TRUE);

    const vector<memory::memory_id>& ids = (Auto_Detected == m_strManu)? m_auto_ids : m_ids;

    m_listChip.ResetContent();
	BOOST_FOREACH(const memory::memory_id& mid, ids)
	{
		if(boost::algorithm::iequals(mid.Manufacturer, m_strManu.GetString()) || m_strManu == ALL_Manu || Auto_Detected == m_strManu) 
		{
			m_listChip.AddString(mid.TypeName.c_str());
		}
	}
	m_listChip.SetCurSel(ids.empty() ? -1 : 0);
    // TODO: Add your control notification handler code here
}

void CManualDetection::InitList()
{

    std::set<wstring> manus;

    int i = 0;
    // manual list
    m_manu_filter.InsertItem(i++, Auto_Detected.GetString());
    m_manu_filter.InsertItem(i++, ALL_Manu.GetString());
    //    m_listChip.AddString(No_Selection);

    BOOST_FOREACH(const memory::memory_id& mid, m_ids)
    {
        if(manus.insert(mid.Manufacturer).second)
        {
            m_manu_filter.InsertItem(i++, mid.Manufacturer.c_str());
        }
    }

    // chip list
    if(m_auto_ids.empty())
    {
        m_listChip.AddString(No_Recognized);
    }
    else
    {
        BOOST_FOREACH(const memory::memory_id& mid, m_auto_ids)
        {
            m_listChip.AddString(mid.TypeName.c_str());
        }
    }
    m_listChip.SetCurSel(m_auto_ids.empty() ? -1 : 0);
}

const memory::memory_id& CManualDetection::get_memory_id()
{
    if(m_ids.size() - 1 == m_idx_selected)
    {
        m_ids.push_back(memory::memory_id());
        return m_ids.back();
    }
    else
    {
        return m_ids.at(m_idx_selected);
    }
}

void CManualDetection::OnSelectChip()
{
    // TODO: Add your control notification handler code here
    OnOk();
}

void CManualDetection::OnNMClickManu(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    
    m_strManu = m_manu_filter.GetItemText(pNMItemActivate->iItem, 0);
    OnManuList();

    *pResult = 0;
}
