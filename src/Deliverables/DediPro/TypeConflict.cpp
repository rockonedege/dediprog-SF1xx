// TypeConflict.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TypeConflict.h"
#include "memory_id.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <algorithm>


// CTypeConflict dialog

IMPLEMENT_DYNAMIC(CTypeConflict, CDialog)

CTypeConflict::CTypeConflict(const vector<memory::memory_id>& ids)
: CDialog(CTypeConflict::IDD)
, m_ids(ids)
, m_selected(memory::memory_id())
{
   //m_selected = ids.begin();
}

CTypeConflict::~CTypeConflict()
{
}

void CTypeConflict::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LSTTYPE, m_listType);
}


BEGIN_MESSAGE_MAP(CTypeConflict, CDialog)
    ON_LBN_DBLCLK(IDC_LSTTYPE, &CTypeConflict::OnSelectType)
END_MESSAGE_MAP()


// CTypeConflict message handlers

void CTypeConflict::OnSelectType()
{
    // TODO: Add your control notification handler code here
    OnOK();
}

void CTypeConflict::OnOK()
{
    CString s;
    BOOST_FOREACH( const memory::memory_id& mid, m_ids )
    { 
        m_listType.GetText(m_listType.GetCurSel(), s);
        if(boost::algorithm::iequals(mid.TypeName, s.GetString()))
        {
            m_selected = mid;
        }
    }

    {
    vector<memory::memory_id>::const_iterator& itr(m_ids.begin());
    itr++;
    std::advance(itr, m_listType.GetCurSel());
    }

    CDialog::OnOK();
}


void CTypeConflict::FillList()
{
    BOOST_FOREACH( const memory::memory_id& mid, m_ids )
    {
        m_listType.AddString(mid.TypeName.c_str());    
    }
    m_listType.SetCurSel(0);
}

const memory::memory_id& CTypeConflict::GetChipId() 
{ 
    return m_selected; 
};  

BOOL CTypeConflict::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    FillList();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
