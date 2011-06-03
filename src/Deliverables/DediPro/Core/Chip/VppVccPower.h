#pragma once 

#include <boost/noncopyable.hpp>

class CUSB;

namespace power
{
    typedef enum
    {
        vccPOWEROFF = 0x00,

        vcc3_5V     = 0x10,
        vcc2_5V     = 0x11,
        vcc1_8V     = 0x12,

        vccdo_nothing = 0xFF,

    } VCC_VALUE;

    typedef enum
    {
        vppPOWEROFF = 0x00,

        vpp9V       = 0x01,
        vpp12V      = 0x02,

        vppdo_nothing = 0xFF,

    } VPP_VALUE;

    class CVppVccPower:boost::noncopyable
    {
    public:
        typedef enum
        {
            VOLTAGE_VPP,
            VOLTAGE_VCC
        }VOLTAGE_MODE;

    public:
        CVppVccPower(CUSB& usb);
    public:
        bool SetVppORVccVoltage(unsigned short vcc, VOLTAGE_MODE mode);
        bool RefreshVccValue(power::VCC_VALUE v);
    public:
        static VCC_VALUE       m_vcc;
        static VPP_VALUE       m_vpp;


    protected:
        CUSB&                  m_usb;
    };


    namespace helper
    {
        power::VPP_VALUE convert_vpp_to_enum(size_t v);
        std::wstring convert_vpp_to_str(size_t v);

    }


    // vcc control
    class CAutoVccPower: public CVppVccPower
    {
    public:
        CAutoVccPower(CUSB& usb, VCC_VALUE v);
        virtual ~CAutoVccPower();
    private:
        bool SetVccValue(VCC_VALUE v);

    };


    // vpp control
    class CAutoVppPower: public CVppVccPower
    {
    public:
        CAutoVppPower(CUSB& usb, VPP_VALUE v);
        ~CAutoVppPower();
    private:
        bool SetVppValue(VPP_VALUE v);
    };


} // namespace power
