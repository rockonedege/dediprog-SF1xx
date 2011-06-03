// ImageFileLoader.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ImageFileLoader.h"
#include "PreferenceEx.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/Classification.hpp>
#include <boost/foreach.hpp>

#include "dedicontext.h"

#include <shlwapi.h>
#include <boost/filesystem.hpp>


using std::fill_n;

// CImageFileLoader dialog

IMPLEMENT_DYNAMIC(CImageFileLoader, CDialog)

CImageFileLoader::CImageFileLoader(Context::CDediContext& context /*=NULL*/)
	: CDialog(CImageFileLoader::IDD)
    , m_PathName(_T(""))
    , m_fileFormat(CFileIO::UNKNOWN)
    , m_context(context)
{
    m_PathName = m_context.file.LastLoadedFile.c_str();
}

CImageFileLoader::~CImageFileLoader()
{
}

void CImageFileLoader::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, CBO_FILEFAPTH, m_PathName);
    DDX_Control(pDX, CBO_FILEFAPTH, m_ctrlPath);
}


BEGIN_MESSAGE_MAP(CImageFileLoader, CDialog)
    ON_BN_CLICKED(IDC_FIND, &CImageFileLoader::OnFindFile)
	ON_CBN_EDITUPDATE(CBO_FILEFAPTH, &CImageFileLoader::OnUpdateFilePath)
    //ON_CBN_CLOSEUP(CBO_FILEFAPTH, &CImageFileLoader::OnUpdateFilePath)
    ON_CBN_KILLFOCUS(CBO_FILEFAPTH, &CImageFileLoader::OnUpdateFilePath)
    ON_CBN_SELCHANGE(CBO_FILEFAPTH, &CImageFileLoader::OnCbnSelchangeFilefapth)
END_MESSAGE_MAP()


// CImageFileLoader message handlers

void CImageFileLoader::OnFindFile()
{
    CString sFormat = m_context.file.LastLoadedFileFormat.c_str();
    sFormat.MakeLower();

    CFileDialog dlg(true,NULL, L"*." + sFormat.Right(3)/*L"*.bin"*/, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
        L"Binary File (*.bin)|*.bin|"
        L"Intel Hex Files(16-bit only) (*.hex)|*.hex|"
        L"Motorola Hex Files (*.s19)|*.s19|"
        L"ROM Files (*.rom)|*.rom|"
        L"All Files (*.*)|*.*||", this);
    if ( dlg.DoModal() == IDOK )
    {
        m_PathName = dlg.GetPathName();
        UpdateData(false);

		selectFileFormatsByExt(m_PathName.MakeLower().GetString());
    };

}

void CImageFileLoader::DisableOptions(UINT lower, UINT upper)
{
    //for(UINT i = lower; i <= upper; ++i)
    //{
    //    GetDlgItem(i)->EnableWindow(BST_CHECKED == IsDlgButtonChecked(i));
    //}

}


void CImageFileLoader::SaveOptions()
{
    UpdateData(true);

    /// LastLoadedFile
    m_context.file.LastLoadedFile = m_PathName.GetString();

    /// LastLoadedFileFormat
    CString sExt;
    switch(GetCheckedRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM))
    {
        case IDC_FORMAT_RAWBIN:
            m_fileFormat = CFileIO::BIN;
            sExt = L"RawBin";
            break;
        case IDC_FORMAT_MOTOS19:
            m_fileFormat = CFileIO::S19;
            sExt = L"MotorolaS19";
            break;
        case IDC_FORMAT_INTELHEX:
            m_fileFormat = CFileIO::HEX;
            sExt = L"IntelHex";
            break;
        case IDC_FORMAT_ROM:
            m_fileFormat = CFileIO::BIN;
            sExt = L"ROM";
            break;
        default:
            m_fileFormat = CFileIO::BIN;
            sExt = L"RawBin";
    }
    m_context.file.LastLoadedFileFormat = sExt.GetString();
    
    std::deque<std::wstring>::iterator itr =  std::find(m_saved_paths.begin(), m_saved_paths.end(), m_PathName.GetString()); 
    if(m_saved_paths.end() == itr)
    {
        m_saved_paths.resize(9);
    }
    else
    {
        m_saved_paths.erase(itr);      
    }
	
	m_saved_paths.push_front(m_PathName.GetString());

    std::wstring paths(boost::join(m_saved_paths, L"|"));
    //BOOST_FOREACH(std::wstring& s, m_saved_paths)
    //{
    //    if(!s.empty()) paths += L"|" + s;
    //}
    m_context.SavedPaths = paths;
}

void CImageFileLoader::OnOK()
{
    SaveOptions();
    CDialog::OnOK();
}

void CImageFileLoader::initFileFormats()
{
	selectFileFormatsByType(m_context.file.LastLoadedFileFormat.c_str());
}
void CImageFileLoader::selectFileFormatsByType(const wstring& filetype)
{
    if(L"IntelHex" == filetype )
        CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_INTELHEX );
    else if(L"MotorolaS19" == filetype)
        CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_MOTOS19 );
    else if(L"ROM" == filetype)
        CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_ROM );
    else      
        CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_RAWBIN );

    DisableOptions(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM);

}


void CImageFileLoader::initFilePaths()
{
    SHAutoComplete(m_ctrlPath.GetDlgItem(1001)->m_hWnd, SHACF_FILESYS_ONLY);

    std::wstring sFormat(m_context.SavedPaths);

    boost::algorithm::split(m_saved_paths, sFormat, boost::algorithm::is_any_of(L"|"), boost::algorithm::token_compress_on);

    BOOST_FOREACH(std::wstring& s, m_saved_paths)
    {
        if(!s.empty()) m_ctrlPath.AddString(s.c_str());
    }

    m_ctrlPath.SetCurSel(0);
}

BOOL CImageFileLoader::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    UpdateData(false);

    initFilePaths();
    initFileFormats();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

std::pair<wstring,              ///< file path
    CFileIO::FILE_FORMAT   ///< file format
> CImageFileLoader::GetFileInfo()
{

    return std::make_pair<std::wstring, 
    CFileIO::FILE_FORMAT
    >
    ( m_PathName.GetString(), m_fileFormat);

}

void CImageFileLoader::selectFileFormatsByExt(const wstring& filename)
{
	boost::filesystem::path p(filename);
	wstring ext = p.extension().wstring();	
	
	if(L".hex" == ext)   CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_INTELHEX );
	else if(L".s19" == ext)   CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_MOTOS19 );
	else if(L".rom" == ext)   CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_ROM );
	else      CheckRadioButton(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM,IDC_FORMAT_RAWBIN );

	DisableOptions(IDC_FORMAT_RAWBIN, IDC_FORMAT_ROM);
}

void CImageFileLoader::OnUpdateFilePath()
{
	// TODO: Add your control notification handler code here
    CString s;

	m_ctrlPath.GetWindowTextW(s);
	selectFileFormatsByExt(s.Trim().GetString());

}
void CImageFileLoader::OnCbnSelchangeFilefapth()
{
    // TODO: Add your control notification handler code here
    CString s;
    m_ctrlPath.GetLBText(m_ctrlPath.GetCurSel(), s);
	selectFileFormatsByExt(s.Trim().GetString());
}
