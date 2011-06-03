// HexProDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DediProView.h"
#include "HexProDlg.h"
#include "CompareLog.h"
#include ".\3rd\File\FileIO.h"
#include ".\3rd\label\label.h"

#include "project.h"
#include "ImageFileLoader.h"
#include "moduleversion.h"
#include "numeric_conversion.h"

#include <vector>
#include <sstream>
#include <string>
#include <iomanip>

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/filesystem.hpp>

//#include <boost/thread.hpp>

using std::vector;
using std::wstringstream;
using std::wstring;
// CHexProDlg dialog

IMPLEMENT_DYNAMIC(CHexProDlg, CDialog)

CHexProDlg::CHexProDlg(CProject* project /*=NULL*/)
: CDialog(CHexProDlg::IDD, NULL)
, m_project(project)
, m_nIndex(0)
, m_bShowFileContent(true)
, m_bDowloadOver(true)
, m_crcFile(L"")
, m_crcMem (L"")
, m_crcBufferedFile(L"")
, m_crcBufferedMem (L"")
, m_isDirty(false)
{
    //if(m_project)
    //{
    //       m_vFile = m_project->getBufferForLoadedFile();
    //       m_vMem = m_project->getLastUpLoadedRange();
    //}

}

CHexProDlg::~CHexProDlg()
{
}

void CHexProDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_GRIDFILE, m_hexGrid);
    //DDX_Control(pDX, IDC_GRIDMEM, m_gridMem);

    DDX_Control(pDX, IDC_CRC, m_lblCRC);
    DDX_Control(pDX, IDC_HEX, m_lblHex);
    DDX_Control(pDX, IDC_ASCII, m_lblAscii);
    DDX_Control(pDX, IDC_FILE_INFO, m_fileinfo);
    DDX_Control(pDX, IDC_CHIP_INFO, m_chipinfo);
    DDX_Control(pDX, IDC_WARNING_RELOAD_FILE, m_warning_file);
    DDX_Control(pDX, IDC_WARNING_RELOAD_CHIP, m_warning_chip);
}


BEGIN_MESSAGE_MAP(CHexProDlg, CDialog)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_OPTHEX, IDC_OPTASCII, OnShowOption)
    //ON_BN_CLICKED(ID_OK, &CHexProDlg::OnExit)
    ON_BN_CLICKED(IDC_COMPARE, &CHexProDlg::OnCompare)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_OPEN, &CHexProDlg::OnOpenFile)
    ON_BN_CLICKED(IDC_READMEM, &CHexProDlg::OnReadMem)
    ON_BN_CLICKED(IDC_JUMPTO, &CHexProDlg::OnJumpTo)

    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
    ON_BN_CLICKED(IDC_NEXTDIFF, &CHexProDlg::OnNextDiff)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_FILEBUFF2FILE, IDC_CHIPBUFF2CHIP, &CHexProDlg::OnSave)
END_MESSAGE_MAP()


// CHexProDlg message handlers
void CHexProDlg::InitGrid()
{
    m_project->getBufferForLastReadChipContents().clear();
    m_hexGrid.SetFileBuffer(m_project->getBufferForLoadedFile());
    m_hexGrid.SetChipBuffer(m_project->getBufferForLastReadChipContents());
}

BOOL CHexProDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    GetDlgItem(IDC_COMPARE)->EnableWindow(!m_project->getBufferForLastReadChipContents().empty());

    //    g_vBuffer = m_vMem;
    InitGrid();

    CheckRadioButton(IDC_OPTHEX, IDC_OPTASCII, IDC_OPTHEX);
    OnShowOption(IDC_OPTHEX);

    UpdateCRCDisplay(true, true);

    m_lblHex.SetFontBold(true).SetTextColor(colBlue);
    m_lblAscii.SetFontBold(true).SetTextColor(colBlue);

    m_fileinfo.SetText(m_project->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_PATH).c_str());
    m_chipinfo.SetText(m_project->getTypeDisplayName().c_str());

    if(!m_project->is_readyToDownload())
    {
        GetDlgItem(IDC_READMEM)->EnableWindow(FALSE);
        m_chipinfo.SetText(L"Chip not identified.");
    }

    bool b = m_project->is_readyToDownload();
    GetDlgItem(IDC_FILEBUFF2FILE )->EnableWindow(!m_project->getBufferForLoadedFile().empty());
    GetDlgItem(IDC_FILEBUFF2CHIP )->EnableWindow( b && (!m_project->getBufferForLoadedFile().empty()));
    GetDlgItem(IDC_CHIPBUFF2FILE )->EnableWindow( !m_project->getBufferForLastReadChipContents().empty());
    GetDlgItem(IDC_CHIPBUFF2CHIP )->EnableWindow( b && (!m_project->getBufferForLastReadChipContents().empty()));

    m_warning_file.SetTextColor(colRed);
    m_warning_chip.SetTextColor(colRed);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CHexProDlg::OnShowOption(UINT nID)
{
    m_hexGrid.m_ShowASCII =  (IDC_OPTASCII == nID);
    //m_hexGrid.SetData(m_project->getBufferForLoadedFile(), m_project->getBufferForLastReadChipContents());     
    m_hexGrid.Invalidate();
}

wstring CHexProDlg::OnSaveAs2File(bool fromFile)
{
    // TODO: Add your control notification handler code here

    CFileDialog dlg(false,NULL, L"*.bin", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
        L"Binary File (*.bin)|*.bin|Intel Hex Files(16-bit only) (*.hex)|*.hex|All Files (*.*)|*.*||", this);

    if ( dlg.DoModal() == IDOK )
    {
        boost::filesystem::path filename(dlg.GetPathName().GetString());

        if(filename.extension().empty())
		{
            switch(dlg.m_ofn .nFilterIndex )
            {
            case 1:
                filename.replace_extension(L".bin");
                break;
            case 2:
                filename.replace_extension(L".hex");
                break;
            default:
                filename.replace_extension(L".bin");
            }

        }


		std::vector<unsigned char>& v = fromFile ? 
			m_project->getBufferForLoadedFile() : m_project->getBufferForLastReadChipContents();

		if(v.size() > (1<<20) && boost::algorithm::iequals(L".hex", filename.extension().wstring()))
		{
			MessageBox(L"Intel 16-bit hex file format does not support file size exceeding 1 Megabyte.\n\nOperation cancelled.",
				L"Dedipro",
				MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			CFileIO f ;
			f.WriteByFileExt(v, filename.wstring().c_str()) ;

			return filename.wstring().c_str();
		}

    }
    return L"";
}

BOOL CHexProDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    return CDialog::PreTranslateMessage(pMsg);
}

void CHexProDlg::OnExit()
{
    // TODO: Add your control notification handler code here
    CDialog::OnOK();
}

void CHexProDlg::OnCompare()
{
    // TODO: Add your control notification handler code here
    CCompareLog dlg(m_project->getBufferForLoadedFile(), m_project->getBufferForLastReadChipContents());
    dlg.DoModal();
}

void CHexProDlg::OnOpenFile()
{
    CImageFileLoader dlg(m_project->get_context());
    if(IDOK == dlg.DoModal())
    {
        m_project->LoadFile(dlg.GetFileInfo());
        SetTimer(IDC_FILE_PROGRESS, 500, NULL);
        GetDlgItem(IDC_FILE_PROGRESS)->ShowWindow(SW_SHOW);
        EnableInteractive(FALSE);
    }
}

void CHexProDlg::OnReadMem()
{
    // TODO: Add your control notification handler code here

    m_project->Run(CProject::READ_WHOLE_CHIP);
    SetTimer(IDC_CHIP_PROGRESS, 500, NULL);
    GetDlgItem(IDC_CHIP_PROGRESS)->ShowWindow(SW_SHOW);
    EnableInteractive(FALSE);
}

void CHexProDlg::UpdateSourceCRC()
{
    m_crcFile = crc::CRC32InStringFormat(m_project->getBufferForLoadedFile());
    m_crcMem  = crc::CRC32InStringFormat(m_project->getBufferForLastReadChipContents());
}

void CHexProDlg::UpdateBufferCRC()
{
    boost::tuples::tie(m_crcBufferedFile, m_crcBufferedMem) = m_hexGrid.CalculateCRCs();
}

void CHexProDlg::UpdateCRCDisplay(bool source, bool buffer)
{
    if(source) UpdateSourceCRC();
    if(buffer) UpdateBufferCRC();

    wstringstream wss;

    wss << L"File = " << m_crcFile 
        << L"   "
        << L"Memory = " << m_crcMem
        << L"   "
        << L"Buffered File = " << (m_crcBufferedFile.empty() ? L"<empty>" : m_crcBufferedFile)
        << L"   "
        << L"Buffered Memory = " <<(m_crcBufferedMem.empty() ? L"<empty>" : m_crcBufferedMem);

    m_lblCRC.SetText(wss.str().c_str());
    m_lblCRC.SetFontBold(true);
}


void CHexProDlg::OnTimer(UINT_PTR nIDEvent)
{
    if(!m_project->is_operationOnGoing()) 
    {
        KillTimer(nIDEvent);
        EnableInteractive(TRUE);

        if(IDC_FILEBUFF2CHIP == nIDEvent)
        {
            m_project->getBufferForLoadedFile().swap(m_project->getBufferForLastReadChipContents());
        }

        if(IDC_FILE_PROGRESS == nIDEvent) m_hexGrid.SetFileBuffer(m_project->getBufferForLoadedFile());
        if(IDC_CHIP_PROGRESS == nIDEvent) m_hexGrid.SetChipBuffer(m_project->getBufferForLastReadChipContents());

        if(IDC_CHIP_PROGRESS == nIDEvent || IDC_FILE_PROGRESS == nIDEvent)
        {
            //GetDlgItem(IDC_CHIP_PROGRESS)->ShowWindow(SW_SHOW);
            GetDlgItem(nIDEvent)->ShowWindow(SW_HIDE);
            m_hexGrid.Invalidate();

            m_fileinfo.SetText(m_project->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_PATH).c_str());
            m_chipinfo.SetText(m_project->getTypeDisplayName().c_str());
            UpdateCRCDisplay(true, true);

            if(IDC_FILE_PROGRESS == nIDEvent) m_warning_file.ShowWindow(SW_HIDE);
            if(IDC_CHIP_PROGRESS == nIDEvent) m_warning_chip.ShowWindow(SW_HIDE);

            m_isDirty = true;
        }
        //else if
        //{
        //}
        else
        {
        }

    }

    ((CProgressCtrl*)GetDlgItem(nIDEvent))->StepIt();
}

void CHexProDlg::EnableInteractive(BOOL bEnable)
{
    bool b(m_project->is_readyToDownload());

    GetDlgItem(IDC_OPEN)->EnableWindow(bEnable);
    GetDlgItem(IDC_READMEM)->EnableWindow(bEnable && b);

    GetDlgItem(IDC_COMPARE)->EnableWindow(bEnable);

    GetDlgItem(IDC_OPTHEX )->EnableWindow(bEnable);
    GetDlgItem(IDC_OPTASCII )->EnableWindow(bEnable);

    GetDlgItem(IDC_JUMPTO )->EnableWindow(bEnable);
    GetDlgItem(IDC_NEXTDIFF )->EnableWindow(bEnable);

    GetDlgItem(IDC_FILEBUFF2FILE )->EnableWindow(bEnable && !m_project->getBufferForLoadedFile().empty());
    GetDlgItem(IDC_FILEBUFF2CHIP )->EnableWindow(bEnable && b && (!m_project->getBufferForLoadedFile().empty()));
    GetDlgItem(IDC_CHIPBUFF2FILE )->EnableWindow(bEnable && !m_project->getBufferForLastReadChipContents().empty());
    GetDlgItem(IDC_CHIPBUFF2CHIP )->EnableWindow(bEnable && b && (!m_project->getBufferForLastReadChipContents().empty()));
}

void CHexProDlg::OnCancel()
{ 
    if(!m_project->is_operationOnGoing()) CDialog::OnCancel(); 
};

void CHexProDlg::OnJumpTo()
{
    // TODO: Add your control notification handler code here
    CString s;
    GetDlgItemText(IDC_ROWJUMP, s);
    size_t addr = numeric_conversion::hexstring_to_size_t(s.GetString());
    m_hexGrid.JumpTo(( addr>> 4) + 1, addr & 0x0f);
}

void CHexProDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    //NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //    Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
    //CString s = m_grid.GetItemText(pItem->iRow, pItem->iColumn);
    //m_grid.SetItemBkColour(pItem->iRow, pItem->iColumn, colRed);

    UpdateCRCDisplay(false, true);

}
void CHexProDlg::OnNextDiff()
{
    // TODO: Add your control notification handler code here
    m_hexGrid.GoToNextDiff();
}

bool CHexProDlg::is_dirty()
{
    return m_isDirty;
}
void CHexProDlg::OnSave( UINT id)
{
    // TODO: Add your control notification handler code here

    //CFileIO f ;

    m_hexGrid.GetData(m_project->getBufferForLoadedFile(), m_project->getBufferForLastReadChipContents());


    switch(id)
    {
    case IDC_FILEBUFF2FILE:
        //f.WriteByFileExt(m_project->getBufferForLoadedFile(), m_project->getImageFileInfo(IMAGEFILE_ATTRIBUTE_PATH).c_str()) ;
        OnSaveAs2File(true);
        break;
    case IDC_CHIPBUFF2FILE:
        //f.WriteByFileExt(m_project->getBufferForLastReadChipContents(), m_project->getImageFileInfo(IMAGEFILE_ATTRIBUTE_PATH).c_str()) ;
        if(boost::algorithm::iequals(OnSaveAs2File(false), 
            m_project->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_PATH)))  m_warning_file.ShowWindow(SW_SHOW);
        break;
    case IDC_FILEBUFF2CHIP:
        m_project->getBufferForLoadedFile().swap(m_project->getBufferForLastReadChipContents());
        m_project->batchUpdate();
        m_warning_chip.ShowWindow(SW_SHOW);
        break;
    case IDC_CHIPBUFF2CHIP:
        m_project->batchUpdate();
        break;
    default: ;   
    }
    SetTimer(id, 500, NULL);
}
