// SelMemType.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DediProView.h"
#include "SelMemType.h"
//#include "project.h"
#include "TypeConflict.h"
#include "SerialFlashInfoEx.h"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "memory_id.h"

#include <sstream>

// CSelMemType dialog

IMPLEMENT_DYNAMIC(CSelMemType, CDialog)

// pay special attention to the declaration of members 
CSelMemType::CSelMemType(const vector<memory::memory_id>& autoIds, wstring allIdentificationTrials)
        : CDialog(CSelMemType::IDD)
        ,m_autoIds(autoIds) 
        ,m_bCustomMem(true)
        ,m_allIdentificationTrials(allIdentificationTrials)
{
}

CSelMemType::~CSelMemType()
{
}

void CSelMemType::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_MEMTYPLIST, m_sCustomMem);
    DDX_Control(pDX, IDC_MANUFACTURE, m_ManuLst);
    DDX_Control(pDX, IDC_MEMTYPLIST, m_TypeLst);
    DDX_CBString(pDX, IDC_MANUFACTURE, m_sManuType);
    DDX_Control(pDX, IDC_IDENTIFIED_TYPE, m_identifiedType);
    DDX_Control(pDX, IDC_RAW_RESULTS, m_rawIDs);
}


BEGIN_MESSAGE_MAP(CSelMemType, CDialog)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_SELEC_AUTO, IDC_SELEC_MANU, &CSelMemType::OnOptTarget)
    ON_CBN_SELCHANGE(IDC_MANUFACTURE, &CSelMemType::OnSelectManufacture)
END_MESSAGE_MAP()


// CSelMemType message handlers

void CSelMemType::OnOptTarget(UINT nID)
{
    m_bCustomMem = (IDC_SELEC_MANU == nID)? true : false;

    GetDlgItem(IDC_MEMTYPLIST)->EnableWindow(m_bCustomMem);
    GetDlgItem(IDC_MANUFACTURE)->EnableWindow(m_bCustomMem);
}

BOOL CSelMemType::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    m_identifiedType.SetFontBold(true).SetTextColor(colRed);
    m_rawIDs.SetTextColor(colBlue);

    if( m_autoIds.empty() )
    {
        CheckRadioButton(IDC_SELEC_AUTO, IDC_SELEC_MANU, IDC_SELEC_MANU);
        GetDlgItem(IDC_SELEC_AUTO)->EnableWindow(false);
        GetDlgItem(IDC_RAW_RESULTS)->ShowWindow(SW_SHOW);
        m_bCustomMem = true;

        CString sDetails(L"Diagnostic Details: ");
        sDetails += m_allIdentificationTrials.c_str();
        m_rawIDs.SetText( sDetails );
    }
    else
    {
        CheckRadioButton(IDC_SELEC_AUTO, IDC_SELEC_MANU, IDC_SELEC_AUTO);

        std::wstringstream wss;
        wss << L"[ ";
        BOOST_FOREACH(const memory::memory_id& mid, m_autoIds)
        {
            wss << mid.TypeName << L"  ";
        }
        wss << L"]";

        m_identifiedType.SetText(wss.str().c_str());
        m_bCustomMem = false;

    }

    InitMap();
    FillManuList();
    FillMemList() ;

    GetDlgItem(IDC_MEMTYPLIST)->EnableWindow(m_bCustomMem);
    GetDlgItem(IDC_MANUFACTURE)->EnableWindow(m_bCustomMem);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

// Populate the memory select list with available types
void CSelMemType::InitMap()
{
    m_manu_type.clear();

    BOOST_FOREACH(const memory::memory_id& mid, m_autoIds)
    {
        m_manu_type.insert(std::make_pair(mid.Manufacturer, mid.TypeName));
    }

    //ci.GetManufacturerTypeMap(m_manu_type);
}

void CSelMemType::FillManuList()
{
    m_ManuLst.AddString(L" < All > ");
    BOOST_FOREACH(const memory::memory_id& mid, m_autoIds)
    {
        m_ManuLst.AddString(mid.Manufacturer.c_str());
    }
    m_ManuLst.SetCurSel(0);

    //set<wstring> setManu;

    //CChipInfo ci;
    //ci.GetManufacturerSet(setManu);

    //set<wstring>::iterator itr, itr_end;
    //itr_end = setManu.end();
    //
    //setManu.insert(L" < All > ");
    //for(itr = setManu.begin(); itr != itr_end; ++itr)
    //    m_ManuLst.AddString(itr->c_str());
    //m_ManuLst.SetCurSel(0);
}

void CSelMemType::FillMemList(void)
{
    m_TypeLst.ResetContent();
    UpdateData(true);

    BOOST_FOREACH(const memory::memory_id& mid, m_autoIds)
    {
        if(L" < All > " == m_sManuType)
        {
            m_TypeLst.AddString(mid.TypeName.c_str());
        }
        else
        {
            if(boost::algorithm::iequals(m_sManuType.GetString(), mid.Manufacturer))
            {
                m_TypeLst.AddString(mid.TypeName.c_str());
            }
        }
    }

    m_TypeLst.AddString(L" < No Choice > ");
    m_TypeLst.SetCurSel(0);

    //std::pair< multimap<wstring, wstring>::iterator,
    //    multimap<wstring, wstring>::iterator > itrRange;

    //if(L" < All > " == m_sManuType)
    //{
    //    itrRange = make_pair(m_manu_type.begin(), m_manu_type.end()); 
    //}
    //else
    //{
    //    itrRange = m_manu_type.equal_range(m_sManuType.GetString());
    //}

    //multimap<wstring, wstring>::iterator itr;
    //for(itr = itrRange.first; itr != itrRange.second; ++itr)
    //    m_TypeLst.AddString(itr->second.c_str());
    //m_TypeLst.AddString(L" < No Choice > ");
    //m_TypeLst.SetCurSel(0);
}

void CSelMemType::OnSelectManufacture()
{
    // TODO: Add your control notification handler code here
    FillMemList() ;
}


void CSelMemType::OnOK()
{
    // TODO: Add your control notification handler code here

    if(!m_bCustomMem)   // auto
    {
        if( m_autoIds.size() > 1)
        {
            CTypeConflict tc(m_autoIds);
            tc.DoModal();
            m_Id = tc.GetChipId();
        }
        else
        {
            m_Id = *m_autoIds.begin();
        }
    }
    else    // manual
    {
        UpdateData(true);

        ChipInfo::CSortedChipList sorted_list;
        BOOST_FOREACH(const memory::memory_id& mid, sorted_list.get_supported_chiplist_ordered_by_typename())
        {
            if(boost::algorithm::iequals(mid.TypeName, m_sCustomMem.GetString()))
            {
                m_Id =  mid;
            }
        }
    }

    CDialog::OnOK();
}

memory::memory_id  CSelMemType::GetSelectedChipId() const
{ 
     return m_Id;
}
