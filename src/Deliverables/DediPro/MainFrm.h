// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include ".\3rd\Progressbar\Progressbar.h"
#include ".\3rd\TrueColorToolBar\TrueColorToolBar.h"


class CMainFrame : public CFrameWnd
{
    
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar              m_wndStatusBar;
    CTrueColorToolBar       m_wndToolBar;
    CReBar                  m_wndReBar;
    CProgressBar            m_wndProgressBar;
    CTrueColorToolBar       m_ToolBarCustom;

// Generated message map functions
protected:
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnActivateApp(BOOL bActive, DWORD dwThreadID);
    DECLARE_MESSAGE_MAP()
public:
    // user defined message
    afx_msg void    OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnTimeControl(WPARAM wParam , LPARAM lParam) ;
    afx_msg LRESULT OnEnableButton(WPARAM wParam , LPARAM lParam = 0)  ;
    afx_msg void    OnOptTarget(UINT nID);
    int             SetMemoryToolBar(void);
    int             SetCardToolBar(void);
    CDialogBar      m_wndDlgBar;
private:
    LRESULT InitModeBar(WPARAM wParam , LPARAM lParam /* = 0 */);
public:
    void            EnableModeBarOption(UINT id, bool bEnable);

};


