#include "StdAfx.h"
#include "Eeprom.h"
#include "Macro.h"

#include <boost/utility.hpp>

CEeprom::CEeprom(CUSB& usb, const unsigned int iAddrStart)
        :m_usb(usb)
        ,m_iAddrStart(iAddrStart)
{
}

CEeprom::~CEeprom(void)
{
}

bool CEeprom::Read16Bytes(const unsigned int iOffset, vector<unsigned char>& vData)
{
    // send request
    CNTRPIPE_RQ rq ;
    
    vData.resize(16);

    // first control packet
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = READ_EEPROM ;
    rq.Value = iOffset + m_iAddrStart ;
    rq.Index = RFU ;
    rq.Length = 16 ;

    if(! m_usb.InCtrlRequest(rq, vData))
        return false ;

    return true ;
}

//// write data to EEPROM 
// Each time 16 bytes (one page of EEPROM) data will be written to the starting address
// The address shall be page aligned
bool CEeprom::Write16Bytes(const unsigned int iOffset, const std::vector<unsigned char> &vData)
{
     // send request
    CNTRPIPE_RQ rq ;

    if (vData.size() != 16) return false;

    // first control packet
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = WRITE_EEPROM ;
    rq.Value = iOffset + m_iAddrStart;
    rq.Index = RFU ;
    rq.Length = static_cast<unsigned long>(vData.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vData))
        return false ;

   return true ;
}

///  CEepromOnFlashCard implementations
bool CEepromOnFlashCard::ResetCounter()
{
    vector<unsigned char> vc(16, 0);
    return Write16Bytes(0x20,vc);
}

bool CEepromOnFlashCard::DownloadAuthenCode(const vector<unsigned char>& vc)
{
    return Write16Bytes(0,vc);
}
bool CEepromOnFlashCard::DownloadOperatitonInfo(const vector<unsigned char>& vc)
{
    return Write16Bytes(0x10,vc);
}

bool CEepromOnFlashCard::SetChipSize(size_t vSize)
{

    m_iSize = vSize;

    return true;
}

bool CEepromOnFlashCard::DownloadTFIT(const vector<unsigned char>& vc)
{
 
    if ((vc.size() & 0x0F) || (vc.size() < 0x0F)) return false;     /// must be mutilply of 16

    vector<unsigned char>::const_iterator itr1, itr2;
    itr1 = vc.begin(); 

    size_t iRound = (vc.size() >> 4);
 
    for(size_t i = 0; i< iRound; ++i)
    {
        itr2 = boost::next(itr1, 16);
        if (!Write16Bytes((i<<4) + 0x30, vector<unsigned char>(itr1, itr2))) return false;
        itr1 = itr2;

        Sleep(20);       
    }

     return true;
}
bool CEepromOnFlashCard::ReadAll(vector<unsigned char>& vc)
{
    vector<unsigned char> vData(16);

    vc.clear();
    vc.reserve(1024);

    unsigned int iRound = m_iSize >> 4;
    for(unsigned int i = 0; i< iRound; ++i)
    {
        if (!Read16Bytes(i<<4,vData)) return false;

        vc.insert(vc.end(),vData.begin(),vData.end());
    }

    return true;
}

bool CEepromOnFlashCard::ReadAuthenCode(vector<unsigned char>& vc)
{
    return Read16Bytes(0, vc);
}
bool CEepromOnFlashCard::ReadOperatitonInfo(vector<unsigned char>& vc)
{
    return Read16Bytes(0x10, vc);
}
bool CEepromOnFlashCard::ReadTFIT(vector<unsigned char>& vc)
{
    const int TFIT_OFFSET = 0x30;
    vector<unsigned char> vData(16);

    vc.clear();

    unsigned int iRound = (m_iSize >> 4) - 3 ;
    for(unsigned int i = 0; i< iRound; ++i)
    {
        if (!Read16Bytes((i<<4) + TFIT_OFFSET, vData)) return false;

        vc.insert(vc.end(),vData.begin(),vData.end());
    }

    return true;
}

