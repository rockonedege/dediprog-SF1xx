#pragma once

#include <map>
#include <set>
#include "afxwin.h"
#include ".\3rd\label\label.h"
#include ".\Core\Chip\DediInludes.h"

#include "memory_id.h"

using std::multimap;;
using std::set;

// CSelMemType dialog
class CSelMemType : public CDialog
{
    DECLARE_DYNAMIC(CSelMemType)

public:
    CSelMemType(const vector<memory::memory_id>& autoIds, wstring allIdentificationTrials);   // standard constructor
    virtual ~CSelMemType();

// Dialog Data
    enum { IDD = IDD_SELECT_MEMORY_TYPE };

public:
    memory::memory_id  GetSelectedChipId() const;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnOptTarget(UINT nID);
    afx_msg void OnSelectManufacture();

    DECLARE_MESSAGE_MAP()
public:
    // Populate the memory select list with available types
    void FillMemList(void);
    afx_msg void OnOK();

private:
    void InitMap();
    void FillManuList();

private:
    CLabel m_identifiedType;
    CLabel m_rawIDs;
    CString m_sManuType;
    CString m_sCustomMem;
    CComboBox m_ManuLst;
    CComboBox m_TypeLst;

    bool m_bCustomMem;
    memory::memory_id m_Id;
    const vector<memory::memory_id>& m_autoIds;
    multimap<wstring, wstring> m_manu_type;
    wstring m_allIdentificationTrials;
};
