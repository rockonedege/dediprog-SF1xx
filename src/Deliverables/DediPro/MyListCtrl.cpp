#include "stdafx.h"

#include "MyListCtrl.h"

#include <afxGlobals.h>

COLORREF CMyListCtrl::OnGetCellTextColor(int nRow, int nColum)
{
    if (!m_bColor)
    {
        return CMFCListCtrl::OnGetCellTextColor(nRow, nColum);
    }
    
    return(nRow % 2) == 0 ? RGB(0, 0, 0) : RGB(0, 0, 0);
}

COLORREF CMyListCtrl::OnGetCellBkColor(int nRow, int nColum)
{
    if (!m_bColor)
    {
        return CMFCListCtrl::OnGetCellBkColor(nRow, nColum);
    }

    if (m_bMarkSortedColumn && nColum == m_iSortedColumn)
    {
        return(nRow % 2) == 0 ? RGB(233, 221, 229) : RGB(255, 244, 234);
    }

    return(nRow % 2) == 0 ? RGB(253, 241, 249) : RGB(255, 244, 234);
}

HFONT CMyListCtrl::OnGetCellFont(int nRow, int nColum, DWORD /*dwData* = 0*/)
{
    if (!m_bModifyFont)
    {
        return NULL;
    }

    if (nColum == 2 &&(nRow >= 4 && nRow <= 8))
    {
        return afxGlobalData.fontDefaultGUIBold;
    }

    return NULL;
}

int CMyListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
    CString strItem1 = GetItemText((int)(lParam1 < lParam2 ? lParam1 : lParam2), iColumn);
    CString strItem2 = GetItemText((int)(lParam1 < lParam2 ? lParam2 : lParam1), iColumn);

    if (iColumn == 0)
    {
        int nItem1 = _ttoi(strItem1);
        int nItem2 = _ttoi(strItem2);
        return(nItem1 < nItem2 ? -1 : 1);
    }
    else
    {
        int iSort = _tcsicmp(strItem1, strItem2);
        return(iSort);
    }
}

void CMyListCtrl::SetAlternateColor()
{	
    m_bColor = true;
    RedrawWindow();
}
void CMyListCtrl::SetBoldFont()
{	
    m_bModifyFont = true;
    RedrawWindow();
}
