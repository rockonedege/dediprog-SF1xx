// EngineeringMode.cpp : implementation file
//

#include "stdafx.h"
#include "DediPro.h"
#include "EngineeringMode.h"
#include "serialflash.h"
#include "progboard.h"
#include "project.h"
#include "vppvccpower.h"

#include <iomanip>

#include "numeric_conversion.h"
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>


// CEngineeringMode dialog

IMPLEMENT_DYNAMIC(CEngineeringMode, CPropertyPage)

CEngineeringMode::CEngineeringMode(CProject* p, CWnd* pParent /*=NULL*/)
	: CPropertyPage(CEngineeringMode::IDD/*, pParent*/),
	//: CDialog(CEngineeringMode::IDD, pParent),
    m_p(p)
	, m_out_stream(_T(""))
{

}

CEngineeringMode::~CEngineeringMode()
{
    power::CVppVccPower p(m_p->getUsbInstance());
    p.SetVppORVccVoltage(power::vccPOWEROFF, power::CVppVccPower::VOLTAGE_VCC);
}

void CEngineeringMode::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IN, m_in);
    DDX_Text(pDX, IDC_OUT, m_out_stream);
    DDX_Control(pDX, IDC_SR, m_srValue);
    DDX_Control(pDX, IDC_ENGMODE_WARNING, m_warning);
}


BEGIN_MESSAGE_MAP(CEngineeringMode, CPropertyPage)
    ON_BN_CLICKED(IDC_SEND, &CEngineeringMode::OnSend)
	ON_BN_CLICKED(IDC_NEEDRETURN, &CEngineeringMode::OnNeedReturn)
    ON_BN_CLICKED(IDC_NEEDMONITOR, &CEngineeringMode::OnNeedMonitor)
END_MESSAGE_MAP()


// CEngineeringMode message handlers

void CEngineeringMode::OnSend()
{
    // TODO: Add your control notification handler code here
	using namespace boost::algorithm;

    CString sValue;
    GetDlgItemTextW(IDC_OUT, sValue);

	vector<wstring> out;
	wstring sIn( sValue.GetString());
	split(out, sIn,is_any_of(L" ,;-"), token_compress_on);

    vector<unsigned char> vOut;
	BOOST_FOREACH(wstring& val, out)
	{
        unsigned char c = static_cast<unsigned char>(numeric_conversion::hexstring_to_size_t(val));
		vOut.push_back(c);
	}

	vector<unsigned char> vIn(GetDlgItemInt(IDC_BYTECOUNT));

    {
        power::CVppVccPower p(m_p->getUsbInstance());
        p.SetVppORVccVoltage(m_p->get_context().power.vcc, power::CVppVccPower::VOLTAGE_VCC);

        CFlashCommand cmd(m_p->getUsbInstance());

        //m_board.SetVccValue(CVppVccPower::vcc3_5V);   // power ON(3.5V)
        (TRUE == IsDlgButtonChecked(IDC_NEEDRETURN) && !vIn.empty()) ? 
            cmd.SendCommand_OneOutOneIn(vOut,vIn)
            :
            cmd.SendCommand_OutOnlyInstruction(vOut)
        ;
        //m_board.SetVccValue(CVppVccPower::POWEROFF);   // power ON(3.5V)
    }
	std::wostringstream wss;
	wss.setf(std::ios::hex, std::ios::basefield);
	BOOST_FOREACH(unsigned char val, vIn)
	{
		wss << std::setfill(L'0') << std::setw(2) << val <<L" ";
	}
	
	SetDlgItemTextW(IDC_IN, wss.str().c_str());
    RefreshSR();
}

void CEngineeringMode::OnNeedReturn()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BYTECOUNT)->EnableWindow(IsDlgButtonChecked(IDC_NEEDRETURN));
}

void CEngineeringMode::OnNeedMonitor()
{
    // TODO: Add your control notification handler code here
	GetDlgItem(IDC_SECONDS)->EnableWindow(IsDlgButtonChecked(IDC_NEEDMONITOR));
}

BOOL CEngineeringMode::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  Add extra initialization here
    m_warning
        .SetTransparent(TRUE)
        .SetText(L"Warning: any on-going operation(e.g. erase) will be terminated immediately after closing this page.")
        .SetTextColor(colRed);
    m_srValue
        .SetText(L"")
       // .SetTransparent(TRUE)
        .SetTextColor(colBlue);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CEngineeringMode::RefreshSR()
{
    // TODO: Add your control notification handler code here

    boost::timer t;
    size_t timeout = GetDlgItemInt(IDC_SECONDS); 

    do{
        unsigned char c;
        if(m_p->readSR(c))
        {
            std::wstringstream wss;
            wss <<"SR = " << std::setfill(L'0')<<std::uppercase<<std::setw(2)<<std::hex << c
                <<L"\t" << t.elapsed() << "seconds elapsed.";
            m_srValue.SetText(wss.str().c_str());
        }
        else
        {
            m_srValue.SetText(L"SR unavailable");
            break;
        }
    }while(t.elapsed() < timeout);
}
