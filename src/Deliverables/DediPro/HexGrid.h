#pragma once
#include ".\3rd\gridctrl\gridctrl.h"

namespace HexGrid
{

    class CHexGrid :
        public CGridCtrl
    {
    public:
        CHexGrid(void);
        ~CHexGrid(void);

    public:
        void SetFileBuffer(const vector<unsigned char>& file);
        void SetChipBuffer(const vector<unsigned char>& chip);
        void GetData(vector<unsigned char>& file, vector<unsigned char>& mem);
        void JumpTo(int row, int col);
        void GoToNextDiff();
        std::pair<wstring, wstring> CalculateCRCs();
    private:
        BOOL ValidateEdit(int nRow, int nCol, LPCTSTR str);
        void SetBuffer(const vector<unsigned char>& v, bool isFile);
        void OnEndEditCell(int nRow, int nCol, CString str);
    public:
        static vector<unsigned char> m_vFile;
        static vector<unsigned char> m_vMem;
        static bool m_ShowASCII;
        static std::pair<int, int> m_focusCell;
        static std::pair<int, int> m_changedCell;

    };

}// namespace HexGrid