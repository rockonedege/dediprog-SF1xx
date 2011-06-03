#pragma once

#include "resource.h"
//#include "afxwin.h"
//#include "afxcmn.h"
#include ".\3rd\label\label.h"
#include "HexGrid.h"

#include <vector>
#include ".\3rd\label\label.h"
using std::vector;

class CProject;

// CHexProDlg dialog

class CHexProDlg : public CDialog
{
    DECLARE_DYNAMIC(CHexProDlg)

public:
    CHexProDlg( CProject* project = NULL);   // standard constructor
    virtual ~CHexProDlg();

// Dialog Data
    enum { IDD = IDD_CONFIG_HEX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void OnOK(){};
    void OnCancel();
    void InitGrid();
    virtual BOOL OnInitDialog();
    afx_msg void OnShowOption(UINT nID);

    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void CHexProDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);

    afx_msg void OnOpenFile();
    afx_msg void OnReadMem();

    afx_msg wstring OnSaveAs2File(bool fromFile);
    afx_msg void OnExit();
    afx_msg void OnCompare();

    afx_msg void EnableInteractive(BOOL);

    afx_msg void OnJumpTo();
    afx_msg void OnNextDiff();
    
    void UpdateSourceCRC();
    void UpdateBufferCRC();
    void UpdateCRCDisplay(bool source, bool buffer);

    DECLARE_MESSAGE_MAP()
private:
    CProject*    m_project;

    LPARAM              m_lpCellSelected;
    HexGrid::CHexGrid   m_hexGrid;

    unsigned int        m_nIndex;
    bool                m_bDowloadOver;
    bool                m_bShowFileContent;
 
	bool		m_isDirty;

    wstring      m_crcFile;
    wstring      m_crcMem;
    wstring      m_crcBufferedFile;
    wstring      m_crcBufferedMem;

private:
    CLabel m_lblCRC;
    CLabel m_lblHex;
    CLabel m_lblAscii;

	CLabel m_fileinfo;
    CLabel m_chipinfo;
public:
    bool is_dirty();
    afx_msg void OnSave(UINT id);
private:
    CLabel m_warning_file;
    CLabel m_warning_chip;
};
