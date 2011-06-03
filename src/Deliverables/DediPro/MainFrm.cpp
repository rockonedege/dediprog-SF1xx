// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DediPro.h"
#include "MainFrm.h"
#include "DediProView.h"
#include "PreferenceEx.h"
#include "DediContext.h"
#include "project.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CDediProView;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_ACTIVATEAPP()
    ON_WM_TIMER()
    ON_MESSAGE(WM_TIMER_CONTROL, OnTimeControl) 
    ON_MESSAGE(WM_GUI_CONTROL, OnEnableButton) 
    ON_MESSAGE(INIT_DLGBAR, InitModeBar) 
    ON_CONTROL_RANGE(BN_CLICKED, IDC_OPTCARD, IDC_OPTMEM2, &CMainFrame::OnOptTarget)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    //
    //if (!m_wndToolBar.CreateEx(this) ||
    //    !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    //{
    //    TRACE0("Failed to create toolbar\n");
    //    return -1;      // fail to create
    //}
    //m_wndToolBar.LoadTrueColorToolBar(24, IDR_MAINFRAME);


    if (!m_ToolBarCustom.Create(this, WS_CHILD | 
        WS_VISIBLE | CBRS_TOP | 
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC/**/) ||
        !m_ToolBarCustom.LoadToolBar(IDR_USERTOOLBAR))
    {
        TRACE0("Failed to create user toolbar\n");
        return -1; //Failed to create
    }

    if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
        CBRS_ALIGN_LEFT|CBRS_TOOLTIPS, AFX_IDW_DIALOGBAR))
    {
        TRACE0("Failed to create dialogbar\n");
        return -1;        // fail to create
    }
    else
    {

#ifdef WORKING_WITH_APPLICATION_MEMORIES_ONLY
        //m_wndDlgBar.GetDlgItem(IDC_OPTMEM1)->ShowWindow(SW_SHOW);
        //m_wndDlgBar.GetDlgItem(IDC_OPTMEM2)->ShowWindow(SW_SHOW);
#endif
#ifdef WORKING_WITH_FLASH_CARD_ONLY
        //m_wndDlgBar.GetDlgItem(IDC_OPTCARD)->ShowWindow(SW_SHOW);
#endif
#ifdef WORKING_WITH_CARD_AND_MEMORIES
        m_wndDlgBar.GetDlgItem(IDC_PROMPT)->ShowWindow(SW_SHOW);
        m_wndDlgBar.GetDlgItem(IDC_OPTMEM1)->ShowWindow(SW_SHOW);
        m_wndDlgBar.GetDlgItem(IDC_OPTMEM2)->ShowWindow(SW_SHOW);
        m_wndDlgBar.GetDlgItem(IDC_OPTCARD)->ShowWindow(SW_SHOW);

        m_wndDlgBar.GetDlgItem(IDC_OPTMEM1)->EnableWindow(true);
        m_wndDlgBar.GetDlgItem(IDC_OPTCARD)->EnableWindow(true);
#endif
    }

    if (!m_wndReBar.Create(this) ||
        //!m_wndReBar.AddBar(&m_wndToolBar) ||
        !m_wndReBar.AddBar(&m_ToolBarCustom)||
        !m_wndReBar.AddBar(&m_wndDlgBar) )
    {
        TRACE0("Failed to create rebar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // adding bar styles
    // TODO: Remove this if you don't want tool tips
    //m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
    //    CBRS_TOOLTIPS | CBRS_FLYBY);

    //Add bitmap to toolbar
    //m_ToolBarCustom.ModifyStyleEx(0,TBSTYLE_FLAT);
    
    m_ToolBarCustom.LoadTrueColorToolBar(
        48,
        IDB_TOOLBAR_DRAW,
        IDB_TOOLBAR_DRAW_HOT,
        IDB_TOOLBAR_DRAW_DISABLED);

    //Add DropDown Button
    //m_ToolBarCustom.AddDropDownButton(this, ID_TOOLBUTTON_AUTO, IDR_MENU_AUTO);
    //m_ToolBarCustom.AddDropDownButton(this, ID_TOOLBUTTON_PROG, IDR_MENU_PROG);

    //SetTimer(1, 500, NULL);

    //InitModeBar();

    SetTitle(L"Welcome to DediPro!");
    
    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    cs.lpszName = L"Dedipro programmer ";
    cs.style = WS_OVERLAPPED | WS_CAPTION /*| FWS_ADDTOTITLE*/
         | WS_THICKFRAME | WS_MINIMIZEBOX /*| WS_MAXIMIZEBOX | WS_MAXIMIZE */| WS_SYSMENU;

    //// optional - set initial window size
    cs.cx = 1024;
    cs.cy = 768;

    return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
    CFrameWnd::OnActivateApp(bActive, dwThreadID);

    // TODO: Add your message handler code here
    if(!IsWindow(m_wndProgressBar.GetSafeHwnd()))
        m_wndProgressBar.Create(L"No operation on-going.", 80, 100, TRUE, 0/*, &m_wndStatusBar*/);
}

LRESULT CMainFrame::OnTimeControl(WPARAM wParam , LPARAM lParam)
{
    int nLower, nUpper ;
    m_wndProgressBar.GetRange(nLower, nUpper) ;
    m_wndProgressBar.SetPos(nUpper) ;
    switch(wParam)
    {
    case START_TIMER :
        m_wndProgressBar.SetPos(nLower) ;
        m_wndProgressBar.SetText(L"Processing, please wait ...");
        SetTimer(1, (UINT)lParam, NULL) ;
        break ;
    case STOP_TIMER :
    default :
        m_wndProgressBar.SetPos(nLower) ;
        m_wndProgressBar.SetText(L"No operation on-going.");
        KillTimer(1) ;
    }

    return true ;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    static long lPos = 0;

    if(lPos > 100) lPos = 0;
    m_wndProgressBar.SetPos(lPos++);

    CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnOptTarget(UINT nID)
{
    wstring sPrompt(L"Current target: ");
    Context::STARTUP_MODE startupMode;

    switch(nID)
    {
        case IDC_OPTCARD:
            startupMode = Context::STARTUP_APPLI_CARD; 
            sPrompt = MODE_NAME_FLASH_CARD;
            SetCardToolBar();
            break;
        case IDC_OPTMEM1:
            startupMode = Context::STARTUP_APPLI_SF_1; 
            sPrompt = MODE_NAME_APPLICATION_MEMORY_1;
            SetMemoryToolBar();
            break;
        case IDC_OPTMEM2:
            startupMode = Context::STARTUP_APPLI_SF_2; 
            sPrompt = MODE_NAME_APPLICATION_MEMORY_2;
            SetMemoryToolBar();
            break;
        default:
            ;
    }

    ((CDediProView*)GetActiveView())->AppendLog(L"Warning: == Target Changed =="); 
    ((CDediProView*)GetActiveView())->AppendLog(sPrompt + L" selected.\n"
        L"Warning: Internal file buffer has been flushed. You might need to re-load any previous file, if any"); 

    CDediProView* p = ((CDediProView*)GetActiveView());
    if(p) p->reset(startupMode);
}

int CMainFrame::SetMemoryToolBar(void)
{
#ifdef INCLUDE_WORKING_WITH_APPLICATION_MEMORIES

    // Set the toolbar to show all commmands
    m_ToolBarCustom.SetButtons(NULL, 9);
    m_ToolBarCustom.SetButtonInfo(0, ID_TOOLBUTTON_SELTYPE,TBBS_BUTTON, 0);
    m_ToolBarCustom.SetButtonInfo(1, ID_TOOLBUTTON_LOADFILE,TBBS_BUTTON, 1);
    m_ToolBarCustom.SetButtonInfo(2, ID_TOOLBUTTON_BLANKCHK,TBBS_BUTTON, 2);
    m_ToolBarCustom.SetButtonInfo(3, ID_TOOLBUTTON_ERASE,TBBS_BUTTON, 3);
    m_ToolBarCustom.SetButtonInfo(4, ID_TOOLBUTTON_PROG,TBBS_BUTTON, 4);
    m_ToolBarCustom.SetButtonInfo(5, ID_TOOLBUTTON_VERIFY,TBBS_BUTTON, 5);
    m_ToolBarCustom.SetButtonInfo(6, ID_TOOLBUTTON_AUTO,TBBS_BUTTON, 6);
    m_ToolBarCustom.SetButtonInfo(7, ID_TOOLBUTTON_HEX,TBBS_BUTTON, 7);
    m_ToolBarCustom.SetButtonInfo(8, ID_TOOLBUTTON_ADVCFG,TBBS_BUTTON, 8/*8*/);
    //m_ToolBarCustom.SetButtonInfo(9, ID_TOOLBUTTON_FREESCALE,TBBS_BUTTON, 13/*8*/);

    //invalidate then call update handlers before painting
    m_ToolBarCustom.Invalidate();
    AfxGetApp()->OnIdle(-1);

#endif
    return 0;
}

int CMainFrame::SetCardToolBar(void)
{  
#ifdef INCLUDE_WORKING_WITH_FLASH_CARD

    // Set the toolbar to show all commmands
#ifdef _DEBUG
    m_ToolBarCustom.SetButtons(NULL, 5);
    m_ToolBarCustom.SetButtonInfo(4, ID_TOOLBUTTON_HEX,TBBS_BUTTON, 7);
#else
    m_ToolBarCustom.SetButtons(NULL, 4);
#endif
    m_ToolBarCustom.SetButtonInfo(0, ID_TOOLBUTTON_SELTYPE,TBBS_BUTTON, 0);
    m_ToolBarCustom.SetButtonInfo(1, ID_TOOLBUTTON_LOADFILE,TBBS_BUTTON, 1);
    m_ToolBarCustom.SetButtonInfo(2, ID_TOOLBUTTON_CARDCONFIG,TBBS_BUTTON, 8);
    m_ToolBarCustom.SetButtonInfo(3, ID_TOOLBUTTON_DLDCARD,TBBS_BUTTON, 9);

    // invalidate then call update handlers before painting
    m_ToolBarCustom.Invalidate();
    AfxGetApp()->OnIdle(-1);

#endif
    return 0;
}

LRESULT CMainFrame::OnEnableButton(WPARAM wParam , LPARAM lParam /* = 0 */)
{
    switch(wParam)
    {
        case ENABLE_BUTTON :
            //m_wndToolBar.EnableWindow(true) ;
            m_ToolBarCustom.EnableWindow(true) ;
            m_wndDlgBar.GetDlgItem(IDC_OPTMEM1)->EnableWindow(true);
            m_wndDlgBar.GetDlgItem(IDC_OPTMEM2)->EnableWindow(true); 
            m_wndDlgBar.GetDlgItem(IDC_OPTCARD)->EnableWindow(true);
            break ;
        case DISABLE_BUTTON :
            //m_wndToolBar.EnableWindow(false) ;
            m_ToolBarCustom.EnableWindow(false) ;
            m_wndDlgBar.GetDlgItem(IDC_OPTMEM1)->EnableWindow(false);
            m_wndDlgBar.GetDlgItem(IDC_OPTMEM2)->EnableWindow(false);     // 06/11/28 anderson
            m_wndDlgBar.GetDlgItem(IDC_OPTCARD)->EnableWindow(false);
            break ;
        default :
            break ;
    }
    return true ;
}

LRESULT CMainFrame::InitModeBar(WPARAM target_flash , LPARAM lParam /* = 0 */)
{
    switch((Context::STARTUP_MODE)target_flash)
    {
    case Context::STARTUP_APPLI_CARD:
            m_wndDlgBar.CheckRadioButton(IDC_OPTCARD, IDC_OPTMEM2, IDC_OPTCARD);
            SetCardToolBar();
            break;
    case Context::STARTUP_APPLI_SF_2:
            m_wndDlgBar.CheckRadioButton(IDC_OPTCARD, IDC_OPTMEM2, IDC_OPTMEM2);
            SetMemoryToolBar();
            break;
    case Context::STARTUP_APPLI_SF_1:
    default:
            m_wndDlgBar.CheckRadioButton(IDC_OPTCARD, IDC_OPTMEM2, IDC_OPTMEM1);
            SetMemoryToolBar();
    }
    return TRUE;
}

void CMainFrame::EnableModeBarOption(UINT id, bool bEnable)
{
    m_wndDlgBar.GetDlgItem(id)->EnableWindow(bEnable);
}
