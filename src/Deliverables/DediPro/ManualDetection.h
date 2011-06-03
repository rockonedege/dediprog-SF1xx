#pragma once
#include "afxwin.h"
#include <afxglobals.h>

#include <vector>
#include "MyListCtrl.h"
// CManualDetection dialog

namespace memory { struct memory_id;}


class CMyList : public CMFCListCtrl
{
    virtual COLORREF OnGetCellTextColor(int nRow, int nColum)
    {
        return(0 == nRow) ? RGB(200, 0, 0) : RGB(0, 0, 0);
    }
    virtual COLORREF OnGetCellBkColor(int nRow, int nColum)
    {
        return CMFCListCtrl::OnGetCellBkColor(nRow, nColum);
    }
    virtual HFONT OnGetCellFont(int nRow, int nColum, DWORD dwData = 0)
    {
        if(0 == nRow)
        {
            return afxGlobalData.fontDefaultGUIBold;

        }
        return NULL;
    }
public:
    CMyList()
    {
    }


};





class CManualDetection : public CDialog
{
    DECLARE_DYNAMIC(CManualDetection)

public:
    CManualDetection(const std::vector<memory::memory_id>& auto_ids);   // standard constructor
    virtual ~CManualDetection();

public:
    const memory::memory_id& get_memory_id();

    // Dialog Data
    enum { IDD = IDD_MEMORY_DETECTION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

private:
    void InitList();

private:
    virtual BOOL OnInitDialog();
    afx_msg void OnManuList();
    afx_msg void OnSelectChip();
    afx_msg void OnOk();
private:
    CListBox m_listChip;
    CString m_strManu;
    CString m_strChip;

    std::vector<memory::memory_id> m_ids;
    const std::vector<memory::memory_id>& m_auto_ids;
    size_t m_idx_selected;
public:
private:
    CMyList m_manu_filter;
public:
    afx_msg void OnNMClickManu(NMHDR *pNMHDR, LRESULT *pResult);
};
