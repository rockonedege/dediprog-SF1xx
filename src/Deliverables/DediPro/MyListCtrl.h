#pragma once
#include <afxlistctrl.h>



class CMyListCtrl : public CMFCListCtrl
{
	virtual COLORREF OnGetCellTextColor(int nRow, int nColum);
	virtual COLORREF OnGetCellBkColor(int nRow, int nColum);
	virtual HFONT OnGetCellFont(int nRow, int nColum, DWORD dwData = 0);

	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

public:
    CMyListCtrl()
        :m_bColor(false)
        ,m_bModifyFont(false)
    {
        EnableMarkSortedColumn();
    }
    void SetAlternateColor();
    void SetBoldFont();

public:
	bool m_bColor;
	bool m_bModifyFont;
};