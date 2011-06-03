// DediProView.h : interface of the CDediProView class
//


#pragma once

#include "resource.h"

#include "ProgBoard.h"
#include "label.h"
#include "TCImageList.h"
#include "dedicontext.h"

#include "moduleversion.h"

#include <boost/scoped_ptr.hpp>
#include "afxwin.h"
using boost::scoped_ptr ;

class CDediProDoc ;
class CProject;

// user defined message
#define WM_TIMER_CONTROL        (WM_USER + 205)
#define START_TIMER             0
#define STOP_TIMER              1

#define WM_GUI_CONTROL          (WM_USER + 206)
#define ENABLE_BUTTON            0
#define DISABLE_BUTTON           1

#define INIT_DLGBAR             (WM_USER + 207)

class CDediProView : public CFormView
{
    enum
    {
        SHOW_CHIPINFO_TYPE,
        SHOW_CHIPINFO_MANUFACTURER,
        SHOW_CHIPINFO_MANUFACTURER_URL,
        SHOW_CHIPINFO_CHIPSIZE_INKBYTE,
        SHOW_CHIPINFO_MANUFACTURERID,
        SHOW_CHIPINFO_JEDECID,

        SHOW_CHIPINFO_COUNT
    };

    enum 
    { 
        BOARD_PARA_TYPE =0, 
        BOARD_PARA_VCC, 
        BOARD_PARA_FIRMWAREVERSION, 
        BOARD_PARA_VPP,

        BOARD_PARA_COUNT 
    };

    typedef enum
    {
        NUMONYX_LOGO = IDB_NUMONYX_LOGO,
        DEDIPROG_LOGO = IDB_SPECIAL_LOGO
    } LOGO;

protected: // create from serialization only
    CDediProView();
    DECLARE_DYNCREATE(CDediProView)

public:
    enum
    { 
        IDD = IDD_MAIN_FORM 
    };

    // Attributes
public:
    //    CDediProDoc* GetDocument() const ;

    // Operations
public:
    // reset
    void reset(Context::STARTUP_MODE target_flash);
    // inits
    void InitViewUIElements();
    bool InitInstance(Context::STARTUP_MODE target_flash);

    // updates
    void UpdateDialogBar();
    bool UpdateRightInfoPanel() ;
    void UpdateBatchConfigDisplay();
    void UpdateSAExtraInfo(bool showCaption, bool showValue);
    void AppendLog(wstring log, bool bPrefixTimeStamp = true,  bool bClear =false);

    // Overrides
protected:
    virtual     BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual     void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg     void OnSize(UINT nType, int cx, int cy);
    afx_msg     void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg     void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg     void OnTimer(UINT_PTR nIDEvent);
    afx_msg     int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg     BOOL OnDeviceChange( UINT nEventType, DWORD_PTR dwData);
	afx_msg     void OnHelpFirmwareAutoupdate();
    virtual     void OnInitialUpdate(); // first called after construct
    void        ChangeLogo(LOGO logo);
    bool        is_functions_locked();

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
private:
    afx_msg void OnCancelOperation();
    afx_msg void OnSelectChip();
    afx_msg void OnLoadFile();
    afx_msg void OnShowEEPROMHisto();
    afx_msg void OnReadAndHexView();
    afx_msg void OnDownload2Card();
    afx_msg void OnAdvancedConfigurations();
    afx_msg void OnCardConfigurations();
    afx_msg void OnHelpUpdatefirmware();
    afx_msg void OnDump2EEPROM();
    afx_msg void OnFreescale();

    bool        CheckFirmwareUpdate();
    void		thdUpdateFirmware();
    bool        CreateProjectInstance(Context::STARTUP_MODE target_flash);
    void        InitLogView();
    bool        MapToolButtonToChipOperation(UINT id);
    void        initLabelStylesInGroup(CLabel* caption, CLabel* field, int cnt);
    void		moveCheckSymbol(bool is_full);
    bool        is_FlashCard_Mode();
    CString     get_vcc_str();
    CString     get_vpp_str();
    void        FillFileInfo(bool clear);
public:
    void        setTargetFlash(Context::STARTUP_MODE startupMode);
    afx_msg     void OnRun(UINT nID);
private:
    CLabel      m_chipParameterCaptions[SHOW_CHIPINFO_COUNT];
    CLabel      m_chipParameterFields[SHOW_CHIPINFO_COUNT];
    CLabel      m_boardParameterCaptions[BOARD_PARA_COUNT];
    CLabel      m_boardParameterFields[BOARD_PARA_COUNT];
    CLabel      m_imageFileInfoCaptions[DediVersion::IMAGEFILE_ATTRBUTE_COUNT_OF_SHOWNITEMS];
    CLabel      m_imageFileInfoFields[DediVersion::IMAGEFILE_ATTRBUTE_COUNT_OF_SHOWNITEMS];
    CLabel      m_saConfigExtraInfoFields[3];
    CLabel      m_saConfigExtraInfoCaptions[3];
    CLabel      m_fullUpdate;
    CLabel      m_partialUpdate;

public:
    scoped_ptr<CProject> m_pProject ;

    // Implementation
public:
    virtual ~CDediProView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    CListCtrl m_logView;
    CTCImageList m_imageList;;
    bool m_updatingfirmwre;
    bool m_updatingfirmwre_successful;
    CStatic m_logo;
    CBitmap   m_logoicon;

private:
};

#ifndef _DEBUG  // debug version in DediProView.cpp
//inline CDediProDoc* CDediProView::GetDocument() const
//   { return reinterpret_cast<CDediProDoc*>(m_pDocument); }
#endif

