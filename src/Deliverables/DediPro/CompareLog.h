#pragma once
#include "afxwin.h"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include ".\3rd\label\label.h"

// CCompareLog dialog

class CCompareLog : public CDialog
{
    DECLARE_DYNAMIC(CCompareLog)

public:
    CCompareLog(vector<unsigned char>& vFile,
        vector<unsigned char>& vMem,
        CWnd* pParent = NULL);   // standard constructor
    virtual ~CCompareLog();

// Dialog Data
    enum { IDD = IDD_COMPARE_LOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()
private:
    void Compare(void);
    CString m_sLog;

    vector<unsigned char>& m_vFile;
    vector<unsigned char>& m_vMem;
    CListBox m_list;

    boost::scoped_ptr<boost::thread> m_thread;
protected:
    virtual void OnOK();
private:
    CLabel m_timing;
    bool m_bExiting;
};
