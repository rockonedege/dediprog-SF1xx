// DediProView.cpp : implementation of the CDediProView class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "DediPro.h"
#include "EEPROMHistory.h"

#include "3rd\File\FileIO.h"
#include "3rd\Logger\Logger.h"
#include "3rd\FolderDialog\FolderDialog.h"

#include "DediProView.h"
#include "SelMemType.h"
#include "Project.h"
#include "HexProDlg.h"
#include "ImageFileLoader.h"
#include "BatchOptions.h"
#include "ProgOptions.h"
#include "TypeConflict.h"
#include "StatusRegisterConfig.h"
#include "StandaloneSettings.h"
#include "FlashCardSettings.h"
#include "EngineeringMode.h"
#include "MiscSettings.h"
#include "DediContext.h"
#include "ManualDetection.h"
#include "Freescale.h"

#include "OptionSheet.h"
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <Dbt.h>
#include <setupapi.h>

#include <sstream>
using std::wstringstream;

const UINT EVENT_UPDATE_FIRMWARE = 0x123;
const UINT LOGGING = 0x124;

// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] = 
{
    // GUID_DEVINTERFACE_USB_DEVICE
    { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

    // GUID_DEVINTERFACE_DISK
    { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

    // GUID_DEVINTERFACE_HID, 
    { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },

    // GUID_NDIS_LAN_CLASS
    { 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }

    //// GUID_DEVINTERFACE_COMPORT
    //{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

    //// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
    //{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

    //// GUID_DEVINTERFACE_PARALLEL
    //{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

    //// GUID_DEVINTERFACE_PARCLASS
    //{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

BOOL MyRegisterDeviceNotification( CDediProView * pView)
{
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) 
    {
        NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
        hDevNotify = RegisterDeviceNotification(pView->GetSafeHwnd(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
        if( !hDevNotify ) 
        {
            //AfxMessageBox(CString("Can't register device notification: ") 
            //	+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
            return FALSE;
        }
    }

    return TRUE;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDediProView
IMPLEMENT_DYNCREATE(CDediProView, CFormView)

BEGIN_MESSAGE_MAP(CDediProView, CFormView)
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_GETMINMAXINFO()
    ON_WM_DEVICECHANGE()


    ON_COMMAND(ID_TOOLBUTTON_SELTYPE, &CDediProView::OnSelectChip)

    ON_COMMAND(ID_TOOLBUTTON_LOADFILE, &CDediProView::OnLoadFile)
    ON_COMMAND_RANGE(ID_TOOLBUTTON_BLANKCHK, ID_TOOLBUTTON_AUTO, &CDediProView::OnRun)
    //ON_COMMAND(ID_TOOLBUTTON_BLANKCHK, &CDediProView::OnBlankChk)
    //ON_COMMAND(ID_TOOLBUTTON_ERASE, &CDediProView::OnErase)
    //ON_COMMAND(ID_TOOLBUTTON_PROG, &CDediProView::OnProg)
    //ON_COMMAND(ID_TOOLBUTTON_READ, &CDediProView::OnRead)
    ON_COMMAND(ID_TOOLBUTTON_EEHISTO, &CDediProView::OnShowEEPROMHisto)
    ON_COMMAND(ID_TOOLBUTTON_DLDCARD, &CDediProView::OnDownload2Card)
    ON_COMMAND(ID_TOOLBUTTON_HEX, &CDediProView::OnReadAndHexView)
    ON_COMMAND(ID_TOOLBUTTON_ADVCFG, &CDediProView::OnAdvancedConfigurations)
    ON_COMMAND(ID_TOOLBUTTON_CARDCONFIG, &CDediProView::OnCardConfigurations)
    ON_COMMAND(ID_HELP_UPDATEFIRMWARE, &CDediProView::OnHelpUpdatefirmware)
    ON_COMMAND(ID_TOOLBUTTON_FREESCALE, &CDediProView::OnFreescale)
    //ON_COMMAND(ID_HELP_FIRMWAREAUTOUPDATE, &CDediProView::OnHelpFirmwareAutoupdate)
END_MESSAGE_MAP()

// CDediProView construction/destruction

CDediProView::CDediProView()
: CFormView(CDediProView::IDD)
, m_pProject(0/*new CProject(CProject::GUI_USERS, Context::STARTUP_APPLI_SF_1)*/)
, m_updatingfirmwre(false)
, m_updatingfirmwre_successful(false)

{
    // TODO: add construction code here
    // m_pProject.reset(new CProject(CProject::GUI_USERS)) ;

}

CDediProView::~CDediProView()
{
}

void CDediProView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_TYPE, m_chipParameterCaptions[ SHOW_CHIPINFO_TYPE]);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_MANU, m_chipParameterCaptions[ SHOW_CHIPINFO_MANUFACTURER]);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_MANUURL, m_chipParameterCaptions[ SHOW_CHIPINFO_MANUFACTURER_URL]);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_MSIZE, m_chipParameterCaptions[ SHOW_CHIPINFO_CHIPSIZE_INKBYTE]);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_MANUID, m_chipParameterCaptions[ SHOW_CHIPINFO_MANUFACTURERID]);
    DDX_Control(pDX, IDC_CHIP_PARAMETER_CAPTION_JID, m_chipParameterCaptions[ SHOW_CHIPINFO_JEDECID]);

    DDX_Control(pDX, IDC_LBLTYPE, m_chipParameterFields[ SHOW_CHIPINFO_TYPE]);
    DDX_Control(pDX, IDC_LBLMANU, m_chipParameterFields[ SHOW_CHIPINFO_MANUFACTURER]);
    DDX_Control(pDX, IDC_LBLMANUURL, m_chipParameterFields[ SHOW_CHIPINFO_MANUFACTURER_URL]);
    DDX_Control(pDX, IDC_LBLMSIZE, m_chipParameterFields[ SHOW_CHIPINFO_CHIPSIZE_INKBYTE]);
    DDX_Control(pDX, IDC_LBLMANUID, m_chipParameterFields[ SHOW_CHIPINFO_MANUFACTURERID]);
    DDX_Control(pDX, IDC_LBLJID, m_chipParameterFields[ SHOW_CHIPINFO_JEDECID]);
    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    DDX_Control(pDX, IDC_LBLBOARDTYPE, m_boardParameterCaptions[BOARD_PARA_TYPE]);
    DDX_Control(pDX, IDC_LBLBOARDTYPE2, m_boardParameterFields[BOARD_PARA_TYPE]);
    DDX_Control(pDX, IDC_LBLBOARDVCC, m_boardParameterCaptions[BOARD_PARA_VCC]);
    DDX_Control(pDX, IDC_LBLBOARDVCC2, m_boardParameterFields[BOARD_PARA_VCC]);
    DDX_Control(pDX, IDC_LBLBOARDVPP, m_boardParameterCaptions[BOARD_PARA_VPP]);
    DDX_Control(pDX, IDC_LBLBOARDVPP2, m_boardParameterFields[BOARD_PARA_VPP]);
    DDX_Control(pDX, IDC_LBLFIRMWAREVERSION, m_boardParameterCaptions[BOARD_PARA_FIRMWAREVERSION]);
    DDX_Control(pDX, IDC_LBLFIRMWAREVERSION2, m_boardParameterFields[BOARD_PARA_FIRMWAREVERSION]);

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    DDX_Control(pDX, IDC_LBLFILENAME, m_imageFileInfoCaptions[DediVersion::IMAGEFILE_ATTRIBUTE_NAME]); 
    DDX_Control(pDX, IDC_LBLFILENAME2, m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_NAME]); 
    DDX_Control(pDX, IDC_LBLFILECHKSUM, m_imageFileInfoCaptions[DediVersion::IMAGEFILE_ATTRIBUTE_CRC]);  
    DDX_Control(pDX, IDC_LBLFILECHKSUM2, m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_CRC]);  
    DDX_Control(pDX, IDC_LBLFILESIZE, m_imageFileInfoCaptions[DediVersion::IMAGEFILE_ATTRIBUTE_SIZE]);  
    DDX_Control(pDX, IDC_LBLFILESIZE2, m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_SIZE]);  

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    DDX_Control(pDX, IDC_LBLTARGET_CHIP_SIZE, m_saConfigExtraInfoCaptions[0]);
    DDX_Control(pDX, IDC_LBLTARGET_CHIP_SIZE2,m_saConfigExtraInfoFields[0]);
    DDX_Control(pDX, IDC_LBLVPPACC_ENABLE, m_saConfigExtraInfoCaptions[1]);
    DDX_Control(pDX, IDC_LBLVPPACC_ENABLE2,m_saConfigExtraInfoFields[1]);
    DDX_Control(pDX, IDC_LBLSA_SEQUENCE, m_saConfigExtraInfoCaptions[2]);
    DDX_Control(pDX, IDC_LBLSA_SEQUENCE2,m_saConfigExtraInfoFields[2]);

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    DDX_Control(pDX, IDC_LOGVIEW, m_logView);
    DDX_Control(pDX, IDC_FULL_UPDATE, m_fullUpdate);
    DDX_Control(pDX, IDC_PARTIAL_UPDATE, m_partialUpdate);
    DDX_Control(pDX, IDC_LOGO, m_logo);
}

BOOL CDediProView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    // optional - set initial window size
    //cs.cx = 1024;
    //cs.cy = 768;


    return CFormView::PreCreateWindow(cs);
}

void CDediProView::reset(Context::STARTUP_MODE target_flash)
{
    InitViewUIElements();

    InitInstance(target_flash);
    AfxGetApp()->m_pMainWnd->SendMessage(INIT_DLGBAR, m_pProject->get_context().StartupMode/*target_flash*/) ; // init dialog bar.

    UpdateRightInfoPanel();
    UpdateDialogBar();

}


void CDediProView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();

    reset(Context::STARTUP_PREVIOUS);
}

// CDediProView diagnostics

#ifdef _DEBUG
void CDediProView::AssertValid() const
{
    CFormView::AssertValid();
}

void CDediProView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

// CDediProDoc* CDediProView::GetDocument() const // non-debug version is inline
// {
//    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDediProDoc)));
//    return (CDediProDoc*)m_pDocument;
//}
#endif //_DEBUG

//CDediProDoc* CDediProView::GetDocument() const
//{ return (CDediProDoc*)m_pDocument; }

// CDediProView message handlers

void CDediProView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}

void CDediProView::OnSizing(UINT fwSide, LPRECT pRect)
{
    CFormView::OnSizing(fwSide, pRect);

    // TODO: Add your message handler code here
}

void CDediProView::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default
    MINMAXINFO* pmmi = (MINMAXINFO*) lpMMI;

    //pmmi->ptMinTrackSize.x = 100;
    //pmmi->ptMinTrackSize.y = 150;
    //pmmi->ptMaxTrackSize.x = 600;
    //pmmi->ptMaxTrackSize.y = 400;

    //      return 0;
    CFormView::OnGetMinMaxInfo(lpMMI);
}

int CDediProView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFormView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_imageList.CreateTC(IDB_LEGEND, RGB(0,0,0));

    // TODO:  Add your specialized creation code here
    MyRegisterDeviceNotification(this);

    SetTimer(LOGGING, 500, NULL);

    return 0;
}

void CDediProView::InitLogView(void)
{
    m_logView.DeleteAllItems();
    m_logView.DeleteItem(0);

    m_logView.SetImageList(&m_imageList, LVSIL_SMALL);

    CRect rect;
    m_logView.GetClientRect(&rect);
    m_logView.InsertColumn(0, L"DediLogs", 0, rect.Width()/* - 5*/);
}

void CDediProView::initLabelStylesInGroup(CLabel* caption, CLabel* field, int cnt)
{
    for(int i = 0; i < cnt; i++)
    {
        caption[i].SetFontBold(true);
        field[i].SetTextColor(colBlue);
        field[i].SetText(L"");
    }
}

void CDediProView::InitViewUIElements(void)
{
    InitLogView();

    initLabelStylesInGroup(&m_chipParameterCaptions[0], &m_chipParameterFields[0], SHOW_CHIPINFO_COUNT);
    initLabelStylesInGroup(&m_boardParameterCaptions[0], &m_boardParameterFields[0], BOARD_PARA_COUNT);
    initLabelStylesInGroup(&m_imageFileInfoCaptions[0], &m_imageFileInfoFields[0], DediVersion::IMAGEFILE_ATTRBUTE_COUNT_OF_SHOWNITEMS);
    initLabelStylesInGroup(&m_saConfigExtraInfoCaptions[0], &m_saConfigExtraInfoFields[0], 3);

    UpdateBatchConfigDisplay();
}


void CDediProView::moveCheckSymbol(bool is_full)
{
    CRect rectArrow, rectRef;

    GetDlgItem(IDC_UPDATE_OPTION_CHECK)->GetWindowRect(&rectArrow);
    GetDlgItem(is_full ? IDC_FULL_UPDATE : IDC_PARTIAL_UPDATE )->GetWindowRect(&rectRef);

    rectArrow.MoveToY(rectRef.top);
    ScreenToClient(&rectArrow);

    GetDlgItem(IDC_UPDATE_OPTION_CHECK)->MoveWindow(rectArrow);

}

void CDediProView::UpdateBatchConfigDisplay()
{
    if(!m_pProject) return;

    Context::CDediContext& context = m_pProject->get_context();

    size_t option = context.Batch_SelectedOption;
    bool is_fullchip = option < 3;

    UINT bShow = (Context::STARTUP_APPLI_CARD != context.StartupMode) ? SW_SHOW : SW_HIDE;

    GetDlgItem(IDC_GRPBATCHCONFIG)->ShowWindow(bShow );
    GetDlgItem(IDC_FULL_UPDATE)->ShowWindow(bShow);
    GetDlgItem(IDC_PARTIAL_UPDATE)->ShowWindow(bShow);

    if(SW_SHOW == bShow) moveCheckSymbol(is_fullchip);
    //GetDlgItem(IDC_PARTIAL_UPDATE_CHECK)->ShowWindow((bShow && !is_fullchip) ? SW_SHOW : SW_HIDE);
    //GetDlgItem(IDC_UPDATE_OPTION_CHECK)->ShowWindow((bShow && is_fullchip) ? SW_SHOW : SW_HIDE);

    m_fullUpdate.SetTextColor(is_fullchip ?colOrangeRed : colGray);
    m_partialUpdate.SetTextColor(is_fullchip ?  colGray : colOrangeRed);

    m_partialUpdate.SetText(
        (L"Partial Update and starting from:" + 
        numeric_conversion::to_hex_string<2>(m_pProject->get_context().Batch_CustomizedAddrFrom, false)).c_str());
}

void CDediProView::UpdateSAExtraInfo(bool showCaption, bool showValue)
{
    BOOST_FOREACH(CLabel &label, m_saConfigExtraInfoCaptions) 
    {
        label.ShowWindow(showCaption ? SW_SHOW : SW_HIDE);
    }

    BOOST_FOREACH(CLabel &label, m_saConfigExtraInfoFields) 
    {
        label.ShowWindow(showValue ? SW_SHOW : SW_HIDE);
    }

    if(showValue)
    {
        boost::array<CString, 3> opt = 
        {
            L"EPV",
            L"BEPV",
            L"PV"
        };

        //std::wstringstream wss;
        //size_t target_size = m_pProject->get_context().flashcard.FlashCard_TargetType;
        //if(0 == target_size)
        //{
        //    wss << L"512 Kbit";
        //}
        //else
        //{
        //    wss << (target_size>>17) << L" Mbit";

        //    size_t addend = target_size & 0x1FFFF;
        //    if(addend) wss <<L" + "<< (addend >> 7) << L" Kbit";
        //}

        m_saConfigExtraInfoFields[0].SetText(m_pProject->get_context().flashcard.FlashCard_TargetType.c_str());
        m_saConfigExtraInfoFields[1].SetText( m_pProject->get_context().flashcard.FlashCard_UsingVpp ? L"Enabled" : L"Disabled" );
        m_saConfigExtraInfoFields[2].SetText(opt[m_pProject->get_context().flashcard.FlashCard_OptionIndex]);
    }
}


void CDediProView::UpdateDialogBar()
{
    if(0 != m_pProject.get())
    {
        if(m_pProject->is_BoardVersionGreaterThan_1_1_6())
        {
            // old board w/ flash memory 2 support
            ((CMainFrame*)GetParentFrame())->EnableModeBarOption(IDC_OPTMEM2,true);
        }
        else
        {
            ((CMainFrame*)GetParentFrame())->EnableModeBarOption(IDC_OPTMEM2,false);
        }

        if(m_pProject->is_SF300()/* || true*/)
        {
            // old board w/ flash memory 2 support
            ((CMainFrame*)GetParentFrame())->EnableModeBarOption(IDC_OPTCARD,true);
        }
        else
        {
            ((CMainFrame*)GetParentFrame())->EnableModeBarOption(IDC_OPTCARD,false);
        }

    }
}

/// - instantiate CProject
/// - readIds()
/// - Create Instance
bool CDediProView::CreateProjectInstance(Context::STARTUP_MODE target_flash)
{
    m_pProject.reset(new CProject(CProject::GUI_USERS, target_flash)) ;

    if(m_pProject->is_usbWorking())
    {
        AppendLog(L"USB OK.");  

        //CheckFirmwareUpdate();

        if(Context::STARTUP_APPLI_CARD == m_pProject->get_context().StartupMode)
        {
            m_pProject->Init();     //auto
        }
        else
        {
            boost::timer t;

            vector<memory::memory_id> ids;
            m_pProject->readIds(ids);

            wstringstream ss;
            ss << t.elapsed() << L"s elapsed to identify chip.";
            AppendLog(ss.str().c_str());

            if( ids.size() > 1)
            {
                CTypeConflict tc(ids);
                tc.DoModal();

                m_pProject->Init(tc.GetChipId());     
                AppendLog((L"Current Type: " + m_pProject->getTypeDisplayName()).c_str());

            }
            else if(1 == ids.size())
            {
                m_pProject->Init(*ids.begin());   
                AppendLog((L"Current Type: " + m_pProject->getTypeDisplayName()).c_str());
            }
            else
            {
                AppendLog(L"Warning: device unrecognizable or not found.");
                return false;
            }
        }

        return true;
    }

    AppendLog(L"Error: Hardware initialization failed.");
    AppendLog(L"Hint: USB connection might not be working.");
    AppendLog(L"Hint: Programmer might not be present..");

    return false;
}

bool CDediProView::InitInstance(Context::STARTUP_MODE target_flash)
{
    if(!DediVersion::isFinalRelease()) 
        AppendLog(L"                        <<Warning: THIS IS AN UNSTABLE INTERNAL BUILD ONLY.  DO NOT RELEASE>>", false);
    AppendLog((L"Welcome to Dedipro " + DediVersion::GetDediproVersion(false)).c_str());
    AppendLog(L"Start logging ...");    
    AppendLog(L"Checking USB connnection ....");  

    return CreateProjectInstance(target_flash);
}
CString  CDediProView::get_vcc_str()
{
    using namespace power;

    CString setting;
    switch(m_pProject->get_context().power.vcc)
    {
    case vcc3_5V: setting = L"3.5V / "; break;
    case vcc2_5V: setting = L"2.5V / "; break;
    case vcc1_8V: setting = L"1.8V / "; break;
    default:  setting = L"unspecified / ";
    }

    return setting + ((power::vccPOWEROFF == CVppVccPower::m_vcc) ? L"OFF" : L"ON");
}

CString CDediProView::get_vpp_str()
{
    const Context::CDediContext& context = m_pProject->get_context();

    if(
        (Context::STARTUP_APPLI_CARD == context.StartupMode && !context.flashcard.FlashCard_UsingVpp)||
        (Context::STARTUP_APPLI_CARD != context.StartupMode && !context.power.Misc_ApplyVpp))
        return L"Not Applicable";

    CString setting;
    switch(context.power.vpp)
    {
    case power::vpp9V:  setting = L"9V / "; break;
    case power::vpp12V: setting = L"12V / "; break;
    default:  setting = L"VCC / ";
    }

    return setting + (( power::CVppVccPower::m_vpp != power::vppPOWEROFF) ? L"ON" : L"OFF") ;
}

bool CDediProView::UpdateRightInfoPanel()
{
    if(!m_pProject.get()) return false;

    CString sVendor((L"for " + m_pProject->getBoardInfo().supported_chip_vendor + L" ").c_str());

    bool is_special_build(!m_pProject->getBoardInfo().supported_chip_vendor.empty());
    theApp.SetTitle(is_special_build ? sVendor : L"");
    ChangeLogo(is_special_build ? NUMONYX_LOGO : DEDIPROG_LOGO);


    ChipInfo::CChipInfo ci;
    memory::memory_id chipID = m_pProject->getChipId();

    /// set chip parameters
    if(Context::STARTUP_APPLI_CARD == m_pProject->get_context().StartupMode)
    {
        bool is_greater_than_3_0_0 = m_pProject->is_BoardVersionGreaterThan_3_0_0();

        SetDlgItemTextW(IDC_GRPINFO, 
            is_greater_than_3_0_0 ? L"SA Board Info" : L"Card Info");
        m_chipParameterCaptions[SHOW_CHIPINFO_CHIPSIZE_INKBYTE].SetText(
            is_greater_than_3_0_0 ? L"Max Programming Density" : L"Size(KB):");

        SetDlgItemTextW(IDC_GRPPARAM2, L"Standalone Config Info");

        CString cardName(m_pProject->getTypeDisplayName().c_str());
        if(!cardName.IsEmpty())
        {
            m_chipParameterFields[ SHOW_CHIPINFO_TYPE].SetText(cardName) ;  
            m_chipParameterFields[ SHOW_CHIPINFO_MANUFACTURER].SetText(L"Dediprog Inc.") ;    
            m_chipParameterFields[ SHOW_CHIPINFO_MANUFACTURER_URL].SetText(L"www.dediprog.com") ;  
            m_chipParameterFields[SHOW_CHIPINFO_CHIPSIZE_INKBYTE].SetText(
                is_greater_than_3_0_0 ? L"128Mb" : numeric_conversion::to_hex_string<6>(m_pProject->getChipId().ChipSizeInByte>>10, true).c_str());

        }

        /// SA extra info
        UpdateSAExtraInfo(true, !cardName.IsEmpty());

        GetDlgItem(IDC_GRPINFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CHIP_PARAMETER_CAPTION_TYPE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CHIP_PARAMETER_CAPTION_MANU)->ShowWindow(SW_HIDE);

        m_chipParameterCaptions[SHOW_CHIPINFO_CHIPSIZE_INKBYTE].ShowWindow(SW_HIDE);

        m_chipParameterFields[SHOW_CHIPINFO_TYPE].ShowWindow(SW_HIDE);
        m_chipParameterFields[SHOW_CHIPINFO_MANUFACTURER].ShowWindow(SW_HIDE);
        m_chipParameterFields[SHOW_CHIPINFO_MANUFACTURER_URL].ShowWindow(SW_HIDE);

        m_chipParameterCaptions[SHOW_CHIPINFO_MANUFACTURERID].ShowWindow(SW_HIDE);
        m_chipParameterCaptions[SHOW_CHIPINFO_JEDECID].ShowWindow(SW_HIDE);
    }
    else
    {
        SetDlgItemTextW(IDC_GRPINFO, L"Memory Info");
        m_chipParameterCaptions[SHOW_CHIPINFO_CHIPSIZE_INKBYTE].SetText(L"Size(KB):");
        SetDlgItemTextW(IDC_GRPPARAM2, L"File Info");

        GetDlgItem(IDC_GRPINFO)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_CHIP_PARAMETER_CAPTION_TYPE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_CHIP_PARAMETER_CAPTION_MANU)->ShowWindow(SW_SHOW);
        m_chipParameterCaptions[SHOW_CHIPINFO_CHIPSIZE_INKBYTE].ShowWindow(SW_SHOW);
        m_chipParameterFields[SHOW_CHIPINFO_TYPE].ShowWindow(SW_SHOW);
        m_chipParameterFields[SHOW_CHIPINFO_MANUFACTURER].ShowWindow(SW_SHOW);
        //m_chipParameterFields[SHOW_CHIPINFO_MANUFACTURER_URL].ShowWindow(SW_SHOW);

        m_chipParameterCaptions[SHOW_CHIPINFO_MANUFACTURERID].ShowWindow(SW_SHOW);
        m_chipParameterCaptions[SHOW_CHIPINFO_JEDECID].ShowWindow(SW_SHOW);


        boost::array<wstring, 6> arChip = 
        {
            L"TypeName",
            L"Manufacturer",
            L"ManufactureUrl",
            L"ChipSizeInKByte",
            L"ManufactureID",
            L"JedecDeviceID",
        } ;

        m_chipParameterFields[0].SetText(chipID.TypeName.c_str());
        m_chipParameterFields[1].SetText(chipID.Manufacturer.c_str());
        m_chipParameterFields[2].SetText(chipID.ManufactureUrl.c_str());
        m_chipParameterFields[3].SetText((0 == chipID.ChipSizeInByte) ? L"" : boost::lexical_cast<std::wstring>(chipID.ChipSizeInByte>>10).c_str());
        m_chipParameterFields[4].SetText(numeric_conversion::to_hex_string<2>(chipID.ManufactureID, true).c_str());
        m_chipParameterFields[5].SetText(numeric_conversion::to_hex_string<6>(chipID.JedecDeviceID, true).c_str());


        /// SA extra info
        UpdateSAExtraInfo(false, false);
    }

    /// set board parameters

    const programmer_board::board_info& bi = m_pProject->getBoardInfo();
    m_boardParameterFields[BOARD_PARA_TYPE].SetText(bi.board_type_display.c_str());
    m_boardParameterFields[BOARD_PARA_FIRMWAREVERSION].SetText(bi.firmware_version.c_str());
    m_boardParameterFields[BOARD_PARA_VCC].SetText(get_vcc_str());
    m_boardParameterFields[BOARD_PARA_VPP].SetText(get_vpp_str());

    /// file info
    if(m_pProject->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_NAME).empty())
    {
        FillFileInfo(true);
    }


    UpdateBatchConfigDisplay();

    return true;
}

void CDediProView::AppendLog(wstring log, bool bPrefixTimeStamp,  bool bClear)
{
    if(log.empty()) return;
    if(bClear)  m_logView.DeleteAllItems();

    int curPos= 0;

    int imgIdx;
    if(-1 != log.find(L"Error:"))       imgIdx = 2;
    else if(-1 != log.find(L"restart"))imgIdx = 2;
    else if(-1 != log.find(L"Warning:"))imgIdx = 0;
    else if(-1 != log.find(L"Operation completed"))imgIdx = 5;
    else if(-1 != log.find(L"seconds elapsed"))imgIdx = 5;
    //else if(-1 != log.find(L"checksum"))imgIdx = 5;
    else if(-1 != log.find(L"OK"))imgIdx = 6;
    else if(-1 != log.find(L"Identical"))imgIdx = 6;
    else if(-1 != log.find(L"Loaded"))imgIdx = 6;
    else                                imgIdx = 1;

    if(is_FlashCard_Mode())    boost::algorithm::ireplace_all(log, L"chip", L"card");

    m_logView.InsertItem(
        LVIF_IMAGE|LVIF_TEXT, m_logView.GetItemCount(), 
        bPrefixTimeStamp ? PrefixTimeStamp(log).c_str() : log.c_str(),
        LVIS_SELECTED, 
        LVIS_SELECTED,  
        imgIdx, 
        0);

    m_logView.Scroll(CSize(0,50));
}

void CDediProView::OnSelectChip()   // memory only, not for card
{
    if(m_pProject.get() == 0) return;
    vector<memory::memory_id> autoIds;

    m_pProject->readIds(autoIds);
    if(autoIds.empty())
    {
        AppendLog(L"Warning: Failed to detect type.");
        AppendLog(m_pProject->getAllChipIdentifyingTrials().c_str());
    }

    CManualDetection md(autoIds);

    if(IDOK == md.DoModal())
    {
        const memory::memory_id& mid = md.get_memory_id();    
        m_pProject->Init(mid);      // by type name

        if(!mid.is_empty()) 
        {
            AppendLog((L"Type " + mid.TypeName + L" is applied").c_str());
        }
        UpdateRightInfoPanel() ;
    }

    return;
}

void CDediProView::OnLoadFile()
{
    if(m_pProject.get())
    {
        CImageFileLoader dlg(m_pProject->get_context());
        if(IDOK == dlg.DoModal())
        {
            FillFileInfo(true);
            m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_NAME].SetText(L"Loading...").SetFontItalic(true);

            m_pProject->LoadFile(dlg.GetFileInfo());

            AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, DISABLE_BUTTON) ;
            // enable progress bar
            AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, START_TIMER, 50) ;

            SetTimer(ID_TOOLBUTTON_LOADFILE, 600, NULL);
        }
    }
    else
    {
        AppendLog(L"Error: Operation Canceled");
        AppendLog(L"Device  not detected !") ;
    }
}

void CDediProView::OnRun(UINT nID)
{
    if(m_pProject.get())
    {
        tribool tri = m_pProject->is_readyToDownload();
        if( tri )
        {
            // disable buttons on GUI
            AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, DISABLE_BUTTON) ;
            // enable progress bar
            AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, START_TIMER, 50) ;

            MapToolButtonToChipOperation(nID);

            SetTimer(nID, 600, NULL);
        }
        else if(!tri)
        {
            InitViewUIElements();
            AppendLog(L"Error: Operation Aborted.");
            AppendLog(L"Hint: device type might not be supported yet.");
            AppendLog(L"Hint: memory device might not be present.");
            AppendLog(L"Hint: Programmer might not be present..");
        }
        else
        {
            AppendLog(L"This operation is locked in this version.");
            AppendLog(L"Please contact Dediprog Inc. for more info.");
        }
    };
}

void CDediProView::OnShowEEPROMHisto()
{
    CEEPROMHistory eh(this);
    eh.DoModal();
}

void CDediProView::OnDownload2Card()
{
    OnRun(ID_TOOLBUTTON_DLDCARD);
}

bool CDediProView::is_functions_locked()
{
    if(m_pProject->is_functions_locked())
    {
        AppendLog(L"This operation is locked in this version.");
        AppendLog(L"Please contact Dediprog Inc. for more info.");

        return true;
    }
    return false;
}

void CDediProView::OnReadAndHexView()
{
    if(is_functions_locked()) return;

    if(is_FlashCard_Mode()) return;

    CHexProDlg dlg(m_pProject.get());
    //CHexProDlg dlg(m_pProject->getBufferForLoadedFile(),m_pProject->getBufferForLastReadChipContents(), this);
    dlg.DoModal();
    if(dlg.is_dirty())
    {
        FillFileInfo(false);
    }
    //OnRun(ID_TOOLBUTTON_READ);
}

void CDediProView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    //wstring sProjectStatus(L"Ide..."), sNewStatus;

    if(EVENT_UPDATE_FIRMWARE == nIDEvent)
    {
        if(!m_updatingfirmwre)
        {
            KillTimer(EVENT_UPDATE_FIRMWARE);

            AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, STOP_TIMER);
            if(m_updatingfirmwre_successful)
            {
                AfxMessageBox(L"Update OK, Please Re-Start this application to apply changes.", MB_ICONINFORMATION);
                //this->UpdateRightInfoPanel();
            }
            else
            {
                AfxMessageBox(L"Update failed.", MB_ICONSTOP);
                AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, ENABLE_BUTTON) ;
            }
        }
    }
    else if( LOGGING ==nIDEvent)
    {
        AppendLog( Logger::GetLog().c_str(), false);
    }
    else
    {

        if(!m_pProject->is_operationOnGoing())
        {
            static CString imageFile, imageCrc, imageSize;
            Context::CDediContext& context = m_pProject->get_context();
            if(m_pProject->is_operationSuccessful())
            {
                switch(nIDEvent)
                {
                case ID_TOOLBUTTON_LOADFILE:
                case ID_TOOLBUTTON_AUTO: 
                    FillFileInfo(false);
                    break;
                case ID_TOOLBUTTON_PROG:    break;
                case ID_TOOLBUTTON_VERIFY:  break;
                default:
                    ;
                }
            }

            FillFileInfo(false);

            KillTimer(nIDEvent);

            wstring sStatus;
            while( !(sStatus = Logger::GetLog()).empty())   // clear logs if any
            {
                AppendLog(sStatus.c_str(), false);
            }

            AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, STOP_TIMER);
            AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, ENABLE_BUTTON) ;

            // pop up edit dialog
            if( ID_TOOLBUTTON_READ == nIDEvent)
            {
                CHexProDlg dlg(m_pProject.get());
                dlg.DoModal();
            }
        }

        m_boardParameterFields[BOARD_PARA_VCC].SetText( get_vcc_str());
        m_boardParameterFields[BOARD_PARA_VPP].SetText( get_vpp_str());
    }	



    CFormView::OnTimer(nIDEvent);
}

void CDediProView::OnAdvancedConfigurations()
{
    if(is_functions_locked()) return;

    if(is_FlashCard_Mode()) return;

    CEngineeringMode em(m_pProject.get());
    CProgOptions po(m_pProject->get_context());
    CAutoSequence bo(m_pProject->get_context());
    CStatusRegisterConfig sr(m_pProject.get());
    CMiscSettings ms(m_pProject->get_context());
    CFreescale fs(m_pProject->get_context());

    {
        COptionSheet os(L"Advanced Settings");

        os.AddPage(&bo);
        os.AddPage(&po);
        os.AddPage(&em);
        os.AddPage(&sr);
        os.AddPage(&ms);
        //os.AddPage(&fs);

        if(IDOK == os.DoModal())
		{
			m_pProject->setCrIfMCF();
		}

        UpdateRightInfoPanel();
    }
}

void CDediProView::OnFreescale()
{
    MessageBox(L"To be implemented");
}


void CDediProView::OnCardConfigurations()
{
    if(!is_FlashCard_Mode()) return;

    // TODO: Add your command handler code here
    if(m_pProject->is_BoardVersionGreaterThan_3_0_0() || (!m_pProject->is_usbWorking()))
    {
        CStandaloneSettings dlg(0xFFFFFFFF, m_pProject->get_context());
        dlg.DoModal();
    }
    else
    {
        CFlashCardSettings dlg(m_pProject.get());
        dlg.DoModal();
    }

    UpdateSAExtraInfo(true, true);
}

void CDediProView::OnDump2EEPROM()
{
    // TODO: Add your control notification handler code here

    unsigned  char  kbuf[256];  
    GetKeyboardState(kbuf);  

    if((kbuf[VK_CONTROL] & 128) && (kbuf[VK_SHIFT] & 128))  
    {
        CFileDialog dlg(true,NULL, L"*.bin", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
            L"Binary File (*.bin)|*.bin|All Files (*.*)|*.*||", this);
        if ( dlg.DoModal() == IDOK )
        {
            CFileIO f;
            vector<unsigned char> vc;
            if(f.ReadByFileExt(dlg.GetFileName().GetString(), vc))
            {
                if(m_pProject->DownloadTFIT(vc))
                {
                    AfxMessageBox(L"Update completed", MB_ICONINFORMATION);
                }
                else
                {
                    AfxMessageBox(L"Update Failed");
                }
            }
        }
    }
}

bool CDediProView::MapToolButtonToChipOperation(UINT id)
{

    Context::STARTUP_MODE target = m_pProject->get_context().StartupMode;

    if((is_FlashCard_Mode()) && (ID_TOOLBUTTON_DLDCARD != id)) return false; // accelerate keys

    switch(id)
    {
    case ID_TOOLBUTTON_BLANKCHK:
        m_pProject->Run(CProject::BLANKCHECK_WHOLE_CHIP);
        break;
    case ID_TOOLBUTTON_ERASE:
        m_pProject->Run(CProject::ERASE_WHOLE_CHIP);
        break;
    case ID_TOOLBUTTON_READ:
        //            m_pProject->Init();
        //            UpdateRightInfoPanel() ;

        //m_pProject->Run(CProject::READ_WHOLE_CHIP);
        //CHexProDlg dlg(m_pProject.get());
        ////CHexProDlg dlg(m_pProject->getBufferForLoadedFile(),m_pProject->getBufferForLastReadChipContents(), this);
        //dlg.DoModal();
        break;
    case ID_TOOLBUTTON_PROG:
        m_pProject->get_context().runtime.InvokeFrom = Context::InvokeFromProg;
        m_pProject->Run(CProject::PROGRAM_CHIP_WITH_OR_WITHOUT_PADDINGS);
        break;
    case ID_TOOLBUTTON_VERIFY:          
        m_pProject->Run(CProject::VERIFY);
        break;
    case ID_TOOLBUTTON_AUTO:
        m_pProject->get_context().runtime.InvokeFrom = Context::InvokeFromBatch;
        m_pProject->Run(CProject::AUTO);
        break;
    case ID_TOOLBUTTON_DLDCARD:
        if(is_FlashCard_Mode())
        {
            m_pProject->get_context().runtime.InvokeFrom = Context::InvokeFromCard;
            m_pProject->Run(CProject::Download2Card);
        }

        break;

    default:
        ;
    }
    return true;
}


BOOL CDediProView::OnDeviceChange( UINT nEventType, DWORD_PTR dwData)
{
#if 1
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
    PDEV_BROADCAST_DEVICEINTERFACE pDevInf= (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;

    wstring ws(pDevInf->dbcc_name);

    if(!boost::algorithm::icontains(ws, L"Vid_0483&Pid_dada")) // "\\?\USB#Vid_0483&Pid_dada#20060104-0001#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
    {
        return FALSE;
    }


    bool bClose = false;
    if(DBT_DEVTYP_DEVICEINTERFACE == pHdr->dbch_devicetype)
    {
        switch( nEventType ) 
        {
        case DBT_DEVICEARRIVAL:
            AppendLog(L"A Dediprog Programmer is plugged in, please wait while initilising ... ");
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            AppendLog(L"Error: Programmer plugged OUT\n"
                L"-   USB communication is lost, Did you just un-plug it? ");
            bClose = true;
            break;
        default:
            ;
        }

        if(bClose)
        {
            //AfxMessageBox(L"USB communication is lost, Did you just un-plug the programmer?\n\n Dediprog will exit .");
            PostQuitMessage(0);
        }
        else
        {
            reset(Context::STARTUP_APPLI_SF_1);
        }
    }
#endif
    return TRUE;
}

void CDediProView::OnCancelOperation()
{
    // TODO: Add your control notification handler code here
    m_pProject->CancelOperation();
}

void CDediProView::OnHelpUpdatefirmware()
{
    // TODO: Add your command handler code here
    CFolderDialog dlg;
    if ( dlg.DoModal() == IDOK )
    {
        if(IDYES == AfxMessageBox(L"Please only use the firmware authorized by Dediprog.\n"
            L" Updating the firmware with any random file will cause fatal error to the programmer.\n"
            L" Click Yes to continue.", MB_ICONINFORMATION|MB_YESNO)
            )
        {
            if(IDYES == AfxMessageBox(L"Firmware Upgrade: \nPlease upgrade with firmware image from Dediprog only!\n\n"
                L"It takes about 23 seconds to update,  click Yes to continue.", MB_ICONINFORMATION|MB_YESNO)
                )
            {
                if(m_pProject->UpdateFirmware((dlg.GetPathName() + L"/").GetString()))
                {
                    AfxMessageBox(L"Update completed, Please Re-Start this application to apply changes.", MB_ICONINFORMATION);
                }
                else
                {
                    AfxMessageBox(L"Update failed", MB_ICONSTOP);
                }
            }
        }
    };
}

bool CDediProView::CheckFirmwareUpdate()
{

    // TODO: Add your control notification handler code here
    std::pair<wstring,wstring> type_version;
    if(m_pProject->isFWUpdatable(type_version))
    {
        //wstringstream wss;
        //wss << L"An firmware update is available.\n\n"
        //    << L"Programmer type: " << type_version.first << L"\n"
        //    << L"Firmware Version: " << type_version.second << L"\n\n"
        //    << L"Please upgrade with firmware image from Dediprog only!\n\n"
        //    << L"It takes about 23 seconds to update\n\n"
        //    << L"Click Yes to update now, click No to skip and continue.";

        AppendLog(L"An newer firmware version("+ type_version.second + L") for " + type_version.first + L" is available" );
        AppendLog(L"Please go to \"Help->Firmware AutoUpdate\" to update.");
        AppendLog(L"please DO NOT unplug USB while updating firmware. Unplug USB while updating firmware will cause serious programmer malfunction.");
        //if(IDYES == AfxMessageBox(wss.str().c_str(), MB_ICONINFORMATION|MB_YESNO)
        //    )
        //{
        //    boost::thread thd(boost::bind(&CDediProView::thdUpdateFirmware, this));
        //    m_updatingfirmwre = true;
        //    SetTimer(EVENT_UPDATE_FIRMWARE, 500, NULL);

        //    // disable buttons on GUI and enable progress bar
        //    AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, DISABLE_BUTTON) ;
        //    AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, START_TIMER, 50) ;

        //    return true;

        //}

    }

    return false;
}

void CDediProView::thdUpdateFirmware()
{
    AppendLog(L"Update firmware started ....");
	AppendLog(L"Warning: Please do not un-plug USB while updating.");

    boost::timer t;

    //::Sleep(8000);
    //m_updatingfirmwre_successful = false;
    m_updatingfirmwre_successful = m_pProject->UpdateFirmware();
    m_updatingfirmwre = false;

    wstringstream wss;
    wss <<t.elapsed() << L"s elapsed to complete updating firmware\n";
    AppendLog(wss.str().c_str());
    AppendLog(L"Please restart this program.");
}

void CDediProView::OnHelpFirmwareAutoupdate()
{
	if(IDYES == AfxMessageBox(L"Warning: Please do not un-plug USB while updating.\n\n"
		L"Click Yes to continue.", MB_ICONINFORMATION|MB_YESNO)
		)
	{
		std::pair<wstring,wstring> type_version;
		if(m_pProject->isFWUpdatable(type_version))
		{
			boost::thread thd(boost::bind(&CDediProView::thdUpdateFirmware, this));
			m_updatingfirmwre = true;
			SetTimer(EVENT_UPDATE_FIRMWARE, 500, NULL);

			// disable buttons on GUI and enable progress bar
			AfxGetApp()->m_pMainWnd->SendMessage(WM_GUI_CONTROL, DISABLE_BUTTON) ;
			AfxGetApp()->m_pMainWnd->SendMessage(WM_TIMER_CONTROL, START_TIMER, 50) ;

		}
		else
		{
			MessageBox(L"No update is available.");
		}
	}


}


bool CDediProView::is_FlashCard_Mode()
{
    if(m_pProject)
    {
        return Context::STARTUP_APPLI_CARD == m_pProject->get_context().StartupMode;
    }
    else
    {
        return false;
    }
}

void CDediProView::setTargetFlash(Context::STARTUP_MODE startupMode)
{
    InitViewUIElements();
    m_pProject->setTargetFlash(startupMode);
}

void CDediProView::FillFileInfo(bool clear)
{
    Context::CDediContext& context = m_pProject->get_context();

    m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_NAME].SetText(
        clear ? L"" : boost::filesystem::path(context.file.CurrentFileInBuffer).filename().c_str()).SetFontItalic(false);
    m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_CRC].SetText(
        clear ? L"" : numeric_conversion::to_hex_string<2>(context.file.file_buffer_crc, true).c_str());
    m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRIBUTE_SIZE].SetText(
        clear ? L"" : numeric_conversion::to_hex_string<2>(context.file.file_size, true).c_str());
}

void CDediProView::ChangeLogo(LOGO logo)
{
    m_logoicon.DeleteObject();
    m_logoicon.LoadBitmapW(logo);
    m_logo.SetBitmap(m_logoicon);
    m_logo.Invalidate();
}
