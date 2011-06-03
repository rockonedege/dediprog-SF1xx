#include "StdAfx.h"

#include "VppVccPower.h"
#include "macro.h"
#include "USBDevice.h"
#include "logger.h"

#include <string>


namespace power
{
    namespace helper
    {
        power::VPP_VALUE convert_vpp_to_enum(size_t v)
        {
            switch(v)
            {
            case 9:  return power::vpp9V;
            case 12: return power::vpp12V;
            default: return power::vppdo_nothing;
            }
        }

        std::wstring convert_vpp_to_str(size_t v)
        {
            switch(v)
            {
            case 9:  return L"9V";
            case 12: return L"12V";
            default: return L"VCC";
            }
        }

    }

    bool CVppVccPower::RefreshVccValue(power::VCC_VALUE v)
    {
        bool b;
        b = SetVppORVccVoltage(vccPOWEROFF, VOLTAGE_VCC);
        Sleep(1000);
        b&= SetVppORVccVoltage(v, VOLTAGE_VCC);
        return b;
    }


    CVppVccPower::CVppVccPower(CUSB& usb)
        :m_usb(usb)
    {}

    bool CVppVccPower::SetVppORVccVoltage(unsigned short v, VOLTAGE_MODE mode)
    {
        // read status
        if(! m_usb.is_open() )
            return false;

        if(0 == v) Sleep(200);

        if( CVppVccPower::VOLTAGE_VCC == mode) m_vcc = static_cast<VCC_VALUE>(v);
        else /*( VOLTAGE_VPP == mode)*/ m_vpp = static_cast<VPP_VALUE>(v);

        // send request
        CNTRPIPE_RQ rq ;
        vector<unsigned char> vBuffer(0) ;
        rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
        rq.Direction = VENDOR_DIRECTION_OUT ;
        rq.Request = (VOLTAGE_VCC == mode) ? SET_VCC : SET_VPP;
        rq.Value = v ;
        rq.Index = RFU ;
        rq.Length = 0 ;

        if(! m_usb.OutCtrlRequest(rq, vBuffer))
        {
            return  false;
        }

        if(0 != v) Sleep(200);
        return true;
    }

    VCC_VALUE CVppVccPower::m_vcc = vccPOWEROFF;
    VPP_VALUE CVppVccPower::m_vpp   = vppPOWEROFF;


    // CAutoVccPower
    CAutoVccPower::CAutoVccPower(CUSB& usb, VCC_VALUE v)
        : CVppVccPower(usb)
    {
        //m_vcc = v;
        SetVccValue(v);
    }

    CAutoVccPower::~CAutoVccPower()
    {
        SetVccValue(vccPOWEROFF);
    }

    bool CAutoVccPower::SetVccValue(VCC_VALUE vcc)
    {
        return SetVppORVccVoltage(vcc, VOLTAGE_VCC);
    }


    // auto vpp
    CAutoVppPower::CAutoVppPower(CUSB& usb, VPP_VALUE v)
        : CVppVccPower(usb)
    {
        if(v != vppdo_nothing) 
        {
            SetVppValue(v);
            Logger::LogIt(L"VPP is turned  On");
        }
		else
		{
			m_vpp = vppdo_nothing;
		}
    }

    CAutoVppPower::~CAutoVppPower()
    {
        if(m_vpp != vppdo_nothing) 
        {
            Logger::LogIt(L"VPP is turned Off");
            SetVppValue(vppPOWEROFF);
        }
    }

    bool CAutoVppPower::SetVppValue(VPP_VALUE vpp)
    {
        return SetVppORVccVoltage(vpp, VOLTAGE_VPP);
    }
}

