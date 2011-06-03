// CompareLog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CompareLog.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/timer.hpp>

#include <sstream>
#include <string>

using boost::bind;
using boost::thread;
using std::wstringstream;

using namespace std;

// CCompareLog dialog

IMPLEMENT_DYNAMIC(CCompareLog, CDialog)

CCompareLog::CCompareLog(vector<unsigned char>& vFile,
                         vector<unsigned char>& vMem, CWnd* pParent /*=NULL*/)
                         : CDialog(CCompareLog::IDD, pParent)
                         , m_vFile(vFile)
                         , m_vMem(vMem)
                         , m_thread(0)
                         , m_bExiting(false)
{

}

CCompareLog::~CCompareLog()
{
}

void CCompareLog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
    DDX_Control(pDX, IDC_TIMING, m_timing);
}


BEGIN_MESSAGE_MAP(CCompareLog, CDialog)
    ON_WM_TIMER()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCompareLog message handlers

BOOL CCompareLog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    if(m_vMem.empty() && m_vFile.size())
    {
        m_sLog = L"Memory or File Buffer is empty. No comparing necessary.";
        return TRUE;
    }

    //SetTimer(1, 1000, NULL);
    m_thread.reset(new boost::thread(boost::bind(&CCompareLog::Compare, this)));

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CCompareLog::Compare(void)
{
    boost::timer t;
    //this->GetDlgItem(IDOK)->EnableWindow(FALSE);

    boost::wformat fmt(L"%1$6s %2$12s %3$16s %4$18s");
    fmt%(L"No.")%(L"Addr.")%(L"File")%(L"Memory");
    m_list.AddString(fmt.str().c_str());

    size_t sizeFile = m_vFile.size();
    size_t sizeMem = m_vMem.size();
    size_t sizeMax = max(sizeFile,sizeMem);

    boost::wformat   fmtPercentage(L"Comparing(%1$-d bytes of %2$-d bytes in total(%3$3.2f%%) ...");
    boost::wformat   fmtNormal(L"%1$=6d       %2$#=06x        %3$#=04x('%4$c')        %5$#=04x('%6$s')");
    boost::wformat   fmtMemLarger(L"%1$=6d        %2$#=06x        %3$=4s        %4$#=04x('%5$c')");
    boost::wformat   fmtFileLarger(L"%1$=6d        %2$#=06x       %3$=#04x('%4$c')       %5$=4s");
    size_t idxTotal = 1;
    for(size_t i = 0; i < sizeMax; ++i)
    {
        if(m_bExiting) return;

        fmtPercentage%idxTotal%sizeMax%(idxTotal*100.0/sizeMax);
        m_timing.SetTextColor(colRed).SetText(fmtPercentage.str().c_str());

        if( i >=sizeFile)
        {
            fmtMemLarger%(idxTotal++)%(i)%(L"End of File")%m_vMem[i]%(char)m_vMem[i];
            m_list.AddString(fmtMemLarger.str().c_str());
        }
        else if( i >=sizeMem)
        {
            fmtFileLarger%(idxTotal++)%(i)%m_vFile[i]%(char)m_vFile[i]%(L"End of Memory");
            m_list.AddString(fmtFileLarger.str().c_str());
        }
        else
        {
            if(m_vMem[i] == m_vFile[i]) continue; 
            fmtNormal%(idxTotal++)%(i)%m_vFile[i]%(char)m_vFile[i]%m_vMem[i]%(char)m_vMem[i];
            m_list.AddString(fmtNormal.str().c_str());
        }
    }

    GetDlgItem(IDOK)->EnableWindow(TRUE);

    m_list.AddString(L"");
    m_list.AddString(L"= End of Comparison =");

    wstringstream wss;
    wss << L"Comparing completes.\t\t" << t.elapsed() << L"s elapsed";
    m_timing.SetTextColor(colRed).SetText(wss.str().c_str());
}

void CCompareLog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
}

void CCompareLog::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    m_bExiting = true; 
    m_thread->yield();
    CDialog::OnOK();
}
