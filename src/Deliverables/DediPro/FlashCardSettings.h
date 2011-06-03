#pragma once
//#include "afxwin.h"
#include "resource.h"


// CFlashCardSettings dialog

class CProject;
class CFlashCardSettings : public CDialog
{
    DECLARE_DYNAMIC(CFlashCardSettings)

public:
    CFlashCardSettings(CProject* project);   // standard constructor
    virtual ~CFlashCardSettings();

// Dialog Data
    enum { IDD = IDD_CONFIG_FLASHCARD_SETTING };


protected:
    virtual void    DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void    OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    void FillSizeLst();
    void CheckButtons();

private:
    unsigned char   m_cSequenceCfg;
    size_t          m_iMaxMemSize;
    size_t          m_iSelSize;
    CString         m_scboSizeSelected;
    CComboBox       m_cboSizeLst;
    BOOL            m_bVppSupported;

    CProject*       m_project;

public:
    size_t              GetSelSize() const;
    unsigned char       GetSequenceCfg() const;
    bool                isAT45() const;
    bool IsVppSupported() const
    {
        return (TRUE == m_bVppSupported);
    }
private:
};
