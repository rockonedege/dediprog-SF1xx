// DediPro.cpp : Defines the class behaviors for the application.
//
 
#include "stdafx.h"
#include "DediPro.h"
#include "MainFrm.h"
#include "DediProDoc.h"
#include "DediProView.h"

#include "moduleversion.h"
#include "SerialFlashInfoEx.h"
#include "memory_id.h"
#include "numeric_conversion.h"

#include "VppVccPower.h"
#include "MyListCtrl.h"

#include <boost/foreach.hpp>
#include <tlhelp32.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
private:
    virtual BOOL OnInitDialog();
    void InitSupportListStyle();
    void InitSupportListData();
    void FillSupportList();
    void OnBnClickedOk();
private:
    CString m_sVer;
    CString m_sCopywrite;
    CLabel m_HomepageURL;
    CLabel m_supportemail;
    CMyListCtrl m_lstSupportedChips;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_sVer(_T(""))
, m_sCopywrite(_T(""))
{
}

void CAboutDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    m_HomepageURL.SetHyperLink(L"www.dediprog.com")
        .SetLink(true, false)
        .SetTextColor(RGB(0,0,255))
        .SetFontUnderline(TRUE)
        .SetLinkCursor(LoadCursor(NULL, IDC_HAND));
    m_supportemail.SetMailLink(true, false)
        .SetTextColor(RGB(0,0,255))
        .SetFontUnderline(TRUE)
        .SetLinkCursor(LoadCursor(NULL, IDC_HAND));

    m_sVer += L"DediProgrammer Pro GUI Console\r\nVersion: ";
    m_sVer += DediVersion::GetDediproVersion(true).c_str();
//    m_sVer += L"\r\nCopyright (C) 2006 - 2007\r\n\r\n";
//#ifdef  WORKING_WITH_CARD_AND_MEMORIES
//    m_sVer += DEFAULT_MODE;
//#endif

    m_sCopywrite += L"WARNING: This computer program is protected by "
        L"copyright law and international treaties. Unauthorized "
        L"duplication or distribution of this program, "
        L"or any portion of it, may result in severe civil or "
        L"criminal penalties, and will be prosecuted to the maximum "
        L"extent possible under the law.";

    m_lstSupportedChips.SetAlternateColor();
    FillSupportList();

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::InitSupportListStyle()
{
    m_lstSupportedChips.SetExtendedStyle(LVS_EX_BORDERSELECT /*| LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER*/ | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    CRect rect;
    m_lstSupportedChips.GetClientRect(&rect);
    int nColInterval = rect.Width()/24;

    m_lstSupportedChips.InsertColumn(0, _T("No."), LVCFMT_LEFT, 2*nColInterval);
    m_lstSupportedChips.InsertColumn(1, _T("Type"), LVCFMT_LEFT, 6*nColInterval);
    m_lstSupportedChips.InsertColumn(2, _T("Manufacturer"), LVCFMT_LEFT, 7*nColInterval);
    m_lstSupportedChips.InsertColumn(3, _T("ID Code"), LVCFMT_LEFT,6*nColInterval);
    m_lstSupportedChips.InsertColumn(4, _T("Vpp"), LVCFMT_LEFT,2*nColInterval);
    //m_lstSupportedChips.InsertColumn(4, _T("Alternative ID"), LVCFMT_LEFT,5*nColInterval);

}

void CAboutDlg::InitSupportListData()
{
    // Use the LV_ITEM structure to insert the items
    LVITEM lvi;
    lvi.mask =  LVIF_TEXT ;

    ChipInfo::CSortedChipList sorted_list;

    CString strItem;
    int idx = 0;
    BOOST_FOREACH(const memory::memory_id& mid, sorted_list.get_supported_chiplist_ordered_by_manufacturer())
    {
        // Insert the first item
        lvi.iItem = idx;

        // Set subitem 0
        lvi.iSubItem = 0;
        strItem.Format(_T(" %2d"), ++idx);
        lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
        m_lstSupportedChips.InsertItem(&lvi);


        // Set subitem 1 - 4
        lvi.iSubItem = 1;
        lvi.pszText = (LPTSTR)(LPCTSTR)(mid.TypeName.c_str());
        m_lstSupportedChips.SetItem(&lvi);

        lvi.iSubItem = 2;
        lvi.pszText = (LPTSTR)(LPCTSTR)(mid.Manufacturer.c_str());
        m_lstSupportedChips.SetItem(&lvi);


        lvi.iSubItem = 3;
        strItem = numeric_conversion::to_hex_string<4>(mid.UniqueID, true).c_str();
        if(mid.DualID) strItem += (L" & " + numeric_conversion::to_hex_string<4>(mid.AlternativeID, false)).c_str();
        strItem.Trim();
        lvi.pszText = (LPTSTR)(LPCTSTR)(strItem == L"0" ? L"" : strItem.GetString());
        m_lstSupportedChips.SetItem(&lvi);

        lvi.iSubItem = 4;
        strItem = power::helper::convert_vpp_to_str(mid.VppSupport).c_str();
        strItem.Trim();
        lvi.pszText = (LPTSTR)(LPCTSTR)(strItem.GetString());
        m_lstSupportedChips.SetItem(&lvi);
    }

}
void CAboutDlg::FillSupportList()
{
    InitSupportListStyle();
    InitSupportListData();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_VER, m_sVer);
    DDX_Text(pDX, IDC_COPYWRITE, m_sCopywrite);
    DDX_Control(pDX, IDC_URL, m_HomepageURL);
    DDX_Control(pDX, IDC_SUPPORTMAIL, m_supportemail);
    DDX_Control(pDX, IDC_LSTSUPPORTED_CHIPS, m_lstSupportedChips);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CDediProApp

BEGIN_MESSAGE_MAP(CDediProApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CDediProApp::OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CDediProApp::OnFileNew)
//    ON_COMMAND(ID_FILE_OPEN, &CDediProApp::OnFileOpen)
//    ON_COMMAND(ID_FILE_CLOSEPROJECT, &CDediProApp::OnFileCloseProject)
    ON_COMMAND(ID_HELP_USERMANUAL, &CDediProApp::OnHelpUsermanual)
    ON_COMMAND(ID_HELP_LAUNCHCALCULATER, &CDediProApp::OnHelpLaunchcalculater)
END_MESSAGE_MAP()


// CDediProApp construction

CDediProApp::CDediProApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CDediProApp object

CDediProApp theApp;

int process_main()//(int argc, char* argv[])
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32); 
    
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    CString str,str_name,str_view=L"";
    int    flg_find = 0;
    while(bMore)
    {
        str_name.Format(_T("%s"), pe32.szExeFile);    
        if( str_name == L"DpCmd.exe" )
        {
            flg_find = 1;
        }

        bMore = ::Process32Next(hProcessSnap, &pe32);
    }

    ::CloseHandle(hProcessSnap);

    if( flg_find == 1 )    return 1;
    else
        return 0;
}

// CDediProApp initialization

BOOL CDediProApp::InitInstance()
{
    if(!IsSingleInstance()) 
    {
        AfxMessageBox(L"Dediprog application is already running! ", MB_ICONSTOP | MB_OK);
        return false;
    }

	int ret_proc = process_main();
    if( ret_proc == 1 )
    {
		AfxMessageBox(L"One instance of Dediprog GUI or Console application is already running! ") ;
		return false;
	}


    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }
    AfxEnableControlContainer();
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Dediprog Inc."));

    LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CDediProDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CDediProView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);


    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    //cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
    ParseCommandLine(cmdInfo);


    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only windowis been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
    m_pMainWnd->UpdateWindow();

    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

void CDediProApp::SetTitle(const CString& vendor)
{
    CString sTitle("Dediprog Software ");
    sTitle += vendor;
    sTitle += DediVersion::GetDediproVersion(false).c_str();

    m_pMainWnd->SetWindowText(sTitle);
}

void CDediProApp::OnFileNew()
{
    CWinApp::OnFileNew();

    CDediProView* pView;
    pView = (CDediProView*)((CMainFrame*)m_pMainWnd)->GetActiveView();

    AfxGetApp()->OnIdle(-1);
}

void CDediProApp::OnFileOpen()
{
}


// App command to run the dialog
void CDediProApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CDediProApp message handlers
void CDediProApp::OnHelpUsermanual()
{
    // TODO: Add your command handler code here
    CString sPath(path_util::GetCurrentExeDir().c_str());

    sPath += L"\\Help.pdf";
    ShellExecute(NULL,L"open",sPath,NULL,NULL,SW_SHOWNORMAL);

}

void CDediProApp::OnHelpLaunchcalculater()
{
    // TODO: Add your command handler code here
    ShellExecute(NULL, L"open", L"calc.exe", NULL, NULL, SW_SHOWNORMAL);
}

bool CDediProApp::IsSingleInstance()
{
    // 用应用程序名创建信号量 
    HANDLE hSem = CreateSemaphore(NULL, 1, 1, m_pszExeName); 

    // 信号量已存在？ 
    // 信号量存在，则程序已有一个实例运行 
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    { 
        // 关闭信号量句柄 
        CloseHandle(hSem); 
        // 寻找先前实例的主窗口 
        HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD); 
        while (::IsWindow(hWndPrevious)) 
        { 
            // 检查窗口是否有预设的标记? 
            // 有，则是我们寻找的主窗 
            if (::GetProp(hWndPrevious, m_pszExeName)) 
            { 
                // 主窗口已最小化，则恢复其大小 
                if (::IsIconic(hWndPrevious)) 
                    ::ShowWindow(hWndPrevious,SW_RESTORE); 
                
                // 将主窗激活 
                ::SetForegroundWindow(hWndPrevious); 

                // 将主窗的对话框激活 
                ::SetForegroundWindow( 
                ::GetLastActivePopup(hWndPrevious)); 

                // 退出本实例 
                return FALSE; 
            } 
            // 继续寻找下一个窗口 
            hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT);
        } 
        // 前一实例已存在，但找不到其主窗 
        // 可能出错了 
        // 退出本实例 
        return FALSE; 
    } 
    return true;
}



