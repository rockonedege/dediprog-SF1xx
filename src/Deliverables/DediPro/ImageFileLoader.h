#pragma once

#include "3rd\File\FileIO.h"
#include "afxwin.h"
#include <deque>
//
//using std::vector;

namespace Context{struct CDediContext;}

// CImageFileLoader dialog

class CImageFileLoader : public CDialog
{
	DECLARE_DYNAMIC(CImageFileLoader)

public:
	CImageFileLoader(Context::CDediContext& context);   // standard constructor
	virtual ~CImageFileLoader();

// Dialog Data
	enum { IDD = IDD_CONFIG_LOAD_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    void OnOK();
private:
    afx_msg void DisableOptions(UINT lower, UINT upper);
    afx_msg void OnFindFile();
    afx_msg void OnFillWith(UINT id);
    afx_msg void initFilePaths();
    afx_msg void initFileFormats();
    afx_msg void SaveOptions();
private:
	void selectFileFormatsByType(const wstring& filename);
	void selectFileFormatsByExt(const wstring& ext);
private:
    bool m_fileValid;
    CString m_PathName;
    CFileIO::FILE_FORMAT m_fileFormat;

    CComboBox m_ctrlPath;
    std::deque<std::wstring> m_saved_paths;
    Context::CDediContext& m_context;
public:
    virtual BOOL OnInitDialog();

    CString GetGetFileName()  const;
    CFileIO::FILE_FORMAT GetFileFormat()  const;

    std::pair<wstring,              ///< file path
        CFileIO::FILE_FORMAT   ///< file format
    > GetFileInfo();
private:
public:
	afx_msg void OnUpdateFilePath();
    afx_msg void OnCbnSelchangeFilefapth();
};
