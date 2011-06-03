#include "StdAfx.h"
#include "HexGrid.h"

#include "util.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "numeric_conversion.h"

namespace HexGrid
{
    vector<unsigned char> CHexGrid::m_vFile;
    vector<unsigned char> CHexGrid::m_vMem;
    bool CHexGrid::m_ShowASCII = false;
    std::pair<int, int> CHexGrid::m_focusCell = std::make_pair(-1, -1);
    std::pair<int, int> CHexGrid::m_changedCell = std::make_pair(-1, -1);


    BOOL CALLBACK GridCallback(GV_DISPINFO *pDispInfo, LPARAM /*lParam*/ ) 
    {

        int nRow, nCol;
        nRow = pDispInfo->item.row;
        nCol = pDispInfo->item.col;

        // seperator column
        if( 17 == nCol)
        {
            // read only
            pDispInfo->item.nState |= GVIS_READONLY;
            return TRUE;
        }

        // Fixed row and col
        if(0 == nRow || 0 == nCol)
        {
            // read only
            pDispInfo->item.nState |= GVIS_READONLY;

            // font
            wcscpy_s(pDispInfo->item.lfFont.lfFaceName, L"Arial");
            pDispInfo->item.lfFont.lfWeight = 14;
            pDispInfo->item.lfFont.lfWeight = FW_BOLD;

            if(!(nRow|nCol)) // row=col=0
            {
                pDispInfo->item.strText = L"Address"; 
                return TRUE;
            }

            //color
            pDispInfo->item.crBkClr = colWhite;
            pDispInfo->item.crFgClr = (0 == nCol) ? colBlue : colBlack;

            if(nRow > 0)    // row = ?,col = 0
            {
                pDispInfo->item.strText.Format(L"0x%06X",(nRow-1) << 4); 

            }
            else    // row = 0, col = ?
            {
                if( nCol < 17)
                {
                    pDispInfo->item.strText.Format(L"+%X",nCol-1);

                }
                else    //col = [18, 34]
                {
                    pDispInfo->item.strText.Format(L"+%X",nCol-18); 

                }
            }
        }
        else    // data
        {
            unsigned int iIdx;
            vector<unsigned char>* pBuffer = (nCol < 17) ? &CHexGrid::m_vFile : &CHexGrid::m_vMem;
            iIdx = (nCol < 17) ? ((nRow-1)<<4) + nCol-1 : ((nRow-1)<<4) + nCol-18;

            if(17 == nCol)
            {
                //pDispInfo->item.strText.Format(L"%c",L"|");
                pDispInfo->item.nState |= GVIS_READONLY;
            }
            else
            {
                if(iIdx < pBuffer->size())
                {
                    pDispInfo->item.strText.Format(CHexGrid::m_ShowASCII ? L"%c" : L"%02X", pBuffer->at(iIdx));
                    if (!CHexGrid::m_ShowASCII) 
                    {
                        pDispInfo->item.nState &= ~GVIS_READONLY;
                    }
                    else
                    {
                        pDispInfo->item.nState |= GVIS_READONLY;
                        pDispInfo->item.crBkClr = colBeige;
                    }

                }
                else
                {
                    pDispInfo->item.nState |= GVIS_READONLY;
                }

                // coloring
                if(iIdx < min(CHexGrid::m_vFile.size(), CHexGrid::m_vMem.size()))
                {
                    if(CHexGrid::m_vFile.at(iIdx) != CHexGrid::m_vMem.at(iIdx)) 
                    {
                        pDispInfo->item.lfFont.lfWeight = 8;
                        //pDispInfo->item.lfFont.lfWeight = FW_BOLD;
                        pDispInfo->item.crFgClr = colWhite;
                        pDispInfo->item.crBkClr = colRed;
                    }

                }
                else
                {
                    //pDispInfo->item.lfFont.lfWeight = 8;
                    //pDispInfo->item.lfFont.lfWeight = FW_BOLD;
                    //pDispInfo->item.crFgClr = colWhite;
                    //pDispInfo->item.crBkClr = colRed;
                }

            }

            // m_changed cells
            //if(nRow == CHexGrid::m_changedCell.first)
            //{
            //    if(nCol == CHexGrid::m_changedCell.second)
            //    {
            //            pDispInfo->item.lfFont.lfWeight = 10;
            //            //pDispInfo->item.lfFont.lfItalic = TRUE;
            //            pDispInfo->item.lfFont.lfWeight = FW_HEAVY;
            //            //pDispInfo->item.crFgClr = colRed;
            //            pDispInfo->item.crBkClr = colRed;
            //    }
            //}

            // focusesd cells
            if(nRow == CHexGrid::m_focusCell.first)
            {
                if( ((nCol == CHexGrid::m_focusCell.second + 1) && !CHexGrid::m_vFile.empty())||  
                    ((nCol == CHexGrid::m_focusCell.second + 18)&& !CHexGrid::m_vMem.empty()))
                {
                    pDispInfo->item.lfFont.lfWeight = 10;
                    //pDispInfo->item.lfFont.lfItalic = TRUE;
                    pDispInfo->item.lfFont.lfWeight = FW_HEAVY;
                    pDispInfo->item.crFgClr = colWhite;
                    pDispInfo->item.crBkClr = colOrange;
                }
            }
            //if(iIdx < min(CHexGrid::m_vFile.size(), CHexGrid::m_vMem.size())
            //{
            //    if(nCol < 17) 
            //    {
            //        pDispInfo->item.strText.Format(L"%02X",CHexGrid::m_vFile.at(iIdx));
            //        pDispInfo->item.nState &= ~GVIS_READONLY;
            //    }
            //    else
            //    {
            //        pDispInfo->item.strText.Format(L" %c",CHexGrid::m_vMem.at(iIdx));
            //        pDispInfo->item.nState |= GVIS_READONLY;
            //        //pDispInfo->item.crBkClr = colWheat;
            //    }

            //    if(iIdx < vrefBuffer.size())
            //    {
            //        if(HexGrid::m_vFile.at(iIdx) != HexGrid::m_vMem.at(iIdx)) 
            //        {
            //            pDispInfo->item.lfFont.lfWeight = 8;
            //            pDispInfo->item.lfFont.lfWeight = FW_BOLD;
            //            pDispInfo->item.crFgClr = colWhite;
            //            pDispInfo->item.crBkClr = colRed;
            //        }
            //    }
            //    else
            //    {
            //        pDispInfo->item.crBkClr = colWhite;
            //    }

            //}
            //else // remaining space
            //{
            //    pDispInfo->item.nState |= GVIS_READONLY;
            //}

        }
        return TRUE;
    }

    CHexGrid::CHexGrid(void)
    {
        SetVirtualMode(true);
        SetCallbackFunc (GridCallback, 0/*m_lpCellSelected*/);
        // boost::function<BOOL (GV_DISPINFO *, LPARAM)>  fun;
        // fun = boost::bind(&CHexGrid::GridCallback, this, _1, _2);

        //SetCallbackFunc ((GRIDCALLBACK)fun.target<BOOL (GV_DISPINFO *, LPARAM)> (), 0 /*m_lpCellSelected*/);

        int nColCnt = 34;
        SetColumnCount(nColCnt);
        SetRowCount(1);

        SetDefCellMargin(4);

        SetGridLines(GVL_HORZ);
        SetGridBkColor(colWhite);

        SetRowResize(false);
        EnableToolTips(false);

        // cell height
        SetDefCellHeight(20);
        SetRowHeight(0, 25);

        // cell width
        for(unsigned int i = 1; i < 34; ++i)
        {
            SetColumnWidth(i, (i < 18) ? 25 : 25);
        }
        SetColumnWidth(0, 68);
        SetColumnWidth(17,20);

        SetEditable(true);

    }

    CHexGrid::~CHexGrid(void)
    {
    }

    void CHexGrid::SetFileBuffer(const vector<unsigned char>& file)
    {
        SetBuffer(file, true);
    }

    void CHexGrid::SetChipBuffer(const vector<unsigned char>& chip)
    {
        SetBuffer(chip, false);
    }

    void CHexGrid::SetBuffer(const vector<unsigned char>& v, bool isFile)
    {
        vector<unsigned char>* p = isFile ? &m_vFile : &m_vMem;

		int rowcnt = static_cast<int>(((v.size() + 0xF) >>4 )+ 1); // don't forget caption row

        if(rowcnt > GetRowCount()) SetRowCount(rowcnt);

        *p = v;
        Invalidate();
    }

    void CHexGrid::GetData(vector<unsigned char>& file, vector<unsigned char>& mem)
    {
        m_vFile.swap(file);
        m_vMem.swap(mem);
    }

    BOOL CHexGrid::ValidateEdit(int nRow, int nCol, LPCTSTR str)
    {
        wstring s(str);
        unsigned int idx;
        vector<unsigned char>* pBuffer = (nCol < 17) ? &CHexGrid::m_vFile : &CHexGrid::m_vMem;
        idx = (nCol < 17) ? ((nRow-1)<<4) + nCol-1 : ((nRow-1)<<4) + nCol-18;
        pBuffer->at(idx) = static_cast<unsigned char>(numeric_conversion::hexstring_to_size_t(s));

        CHexGrid::m_changedCell = std::make_pair(nRow, nCol);

        Invalidate();
        return TRUE;
    }

    void CHexGrid::OnEndEditCell(int nRow, int nCol, CString str)
    {
        CGridCtrl::OnEndEditCell( nRow,  nCol,  str);
    }

    void CHexGrid::JumpTo(int row, int col)
    {
        CRect rect;
        GetClientRect(rect);

        if(row < GetRowCount())
        {
            SetScrollPos32(SB_VERT, (__int64)(row) * m_nVScrollMax / GetRowCount());
            SetFocusCell(row, 0);
        }
        else
        {
            SetScrollPos32(SB_VERT, m_nVScrollMax);
        }

        CHexGrid::m_focusCell.first = row;
        CHexGrid::m_focusCell.second = col;

        InvalidateRect(rect);

    }

    void CHexGrid::GoToNextDiff()
    {
        // TODO: Add your control notification handler code here
        static size_t idx;

        size_t max = min(m_vFile.size(), m_vMem.size());
        for(size_t i = idx; i < max;  i++)
        {
            if(m_vFile.at(i) != m_vMem.at(i))
            {
                JumpTo((i >> 4) + 1, i & 0x0F);
                idx = i+1;
                return;
            }
        }

        //JumpTo((max >> 4) + 1);
        idx = 0; // rewind

        return;
    }



    std::pair<wstring, wstring> CHexGrid::CalculateCRCs()
    {
        return std::make_pair( crc::CRC32InStringFormat(m_vFile), crc::CRC32InStringFormat(m_vMem));
    }


} // namespace HexGrid