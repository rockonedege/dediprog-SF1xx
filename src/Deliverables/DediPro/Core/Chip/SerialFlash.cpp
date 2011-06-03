#include "stdafx.h"
#include "SerialFlash.h"
#include "AT45DBxxxD.h"

#include "..\USB\USBDevice.h"
#include "..\..\3rd\Logger\Logger.h"

#include "SerialFlashInfoEx.h"
#include "memory_id.h"

#include <set>
#include <sstream>

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

using std::set ;
using std::make_pair ;
using std::wstringstream;
//
using boost::array;



/// CChip Implementations 

// write status register , just 1 bytes
// WRSR only is effects on SRWD BP2 BP1 BP0 when WEL = 1
bool CSerialFlash::doWRSR(unsigned char cSR) 
{
    if(! m_usb.is_open())
        return false ;

    // wait until WIP = 0
    waitForWIP() ;

    // wait until WEL = 1
    waitForWEL() ;

    // send request
    vector<unsigned char> vInstruction ;

    vInstruction.push_back(m_codeWRSR) ;
    vInstruction.push_back(cSR) ;

    return m_cmd.SendCommand_OutOnlyInstruction(vInstruction);
}

bool CSerialFlash::doRDSR(unsigned char& cSR)
{
    // read status
    if(! m_usb.is_open() )
        return false ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction ;    //size 1

    // first control packet
    vInstruction.push_back(m_codeRDSR) ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction))
        return false ;

    // second control packet : fetch data
    vector<unsigned char> vBuffer(1) ;        //just read one bytes , in fact more bytes are also available
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = TRANSCEIVE ;
    rq.Value = CTRL_TIMEOUT ;
    rq.Index = NO_REGISTER ;
    rq.Length = static_cast<unsigned long>(vBuffer.size()) ;

    if(! m_usb.InCtrlRequest(rq, vBuffer))
        return false ;

    cSR = vBuffer.front() ;

    return true ;
}

// 
/**
 * \brief
 * wait for write enable with a timeout.
 * 
 * Unless the memory is malfunctioning, it should return
 * before the timer counts down to 0 from 0xFFFFFFFF.
 * 
 * \remarks
 *  SR bitmap:
 *   7       6   5   4   3   2   1   0
 *   SRWD    0   0   BP2 BP1 BP0 WEL WIP
 * 
 */
void CSerialFlash::waitForWEL()
{

    unsigned char cSR ;
    size_t i = MAX_TRIALS;
    // wait until WIP = 0 and WEL = 1
    do{
        doWREN() ;

        // read SR to check WEL until WEL = 1
        doRDSR(cSR) ;
    }while(((cSR & 0x02) == 0)  && (i-- > 0)) ;

}

bool CSerialFlash::doWREN()
{
    if(! m_usb.is_open() ) return false ;

    vector<unsigned char> v(1, m_codeWREN);

    return m_cmd.SendCommand_OutOnlyInstruction(v);

}

bool CSerialFlash::doWRDI()
{
    if(! m_usb.is_open() ) return false ;

    vector<unsigned char> v(1, m_codeWRDI);

    return m_cmd.SendCommand_OutOnlyInstruction(v);

}

/**
 * \brief
 * wait Write In Process with a timeout.
 * 
 * Unless the memory is malfunctioning, it should return
 * before the timer counts down to 0 from 0xFFFFFFFF.
 * 
 * \remarks
 *  SR bitmap:
 *   7       6   5   4   3   2   1   0
 *   SRWD    0   0   BP2 BP1 BP0 WEL WIP
 * 
 */
void CSerialFlash::waitForWIP()
{    
    unsigned char cSR ;
    size_t i = MAX_TRIALS;
    // wait until WIP = 0
    do{
        doRDSR(cSR) ;

        if( m_isCanceled) break;

    }while((cSR & 0x01) && (i-- > 0)) ;

}

tribool CSerialFlash::protectBlock(tribool bProtect)
{
    return do_nothing;
}

/**
 * \brief
 * read data, check if 0xFF
 * 
 * \returns
 * true, if blank; false otherwise
 */
bool CSerialFlash::rangeBlankCheck(pair<unsigned int, unsigned int> Range)   
{
    //if((m_uniqueId.first > CAT45DBxxxD::AT45DB011D - 1) && (m_uniqueId.first < CAT45DBxxxD::AT45DB642D + 1))
    //{
    //    vector<unsigned char> vBuffer ;

    //    if(! Read(Range, vBuffer))
    //        return false ;

    //    vector<unsigned char>::iterator itr, itr_end;
    //    itr_end = vBuffer.end();
    //    for(itr = vBuffer.begin(); itr != itr_end; ++itr)
    //    {
    //        if(0xFF != *itr) return false;
    //    }
    //
    //}
    //else
    //{
    //    vector<unsigned char> vBuffer ;
    //    vBuffer.reserve(1<<16);

    //    size_t pageNum = (Range.second - Range.first) >> 16 ;
    //    vector<unsigned char>::iterator itr, itr_end;
    //    for(size_t i = 0; i < pageNum; ++ i)
    //    {
    //        if(! Read(std::make_pair(Range.first + (i << 16), Range.first + ((i+1) << 16)), vBuffer)) return false ;

    //        itr = vBuffer.begin();
    //        itr_end = vBuffer.end();
    //        for(; itr != itr_end; ++itr)
    //        {
    //            if(0xFF != *itr) return false;
    //        }

    //    }
    //}

    vector<unsigned char> vBuffer ;

    if(! rangeRead(Range, vBuffer))
        return false ;

    vector<unsigned char>::iterator itr, itr_end;
    itr_end = vBuffer.end();
    for(itr = vBuffer.begin(); itr != itr_end; ++itr)
    {
        if(0xFF != *itr) return false;
    }
    
    return true ;
}

/**
 * \brief
 * Write brief comment for Download here.
 * 
 * it first check the data size and then delegates the actual operation to BulkPipeWrite() 
 * 
 * \param AddrRange
 * the flash memory starting(inclusive) and end address(exlusive)
 * 
 * \param vData
 * data to be written into the flash memory.
 * 
 * \returns
 * true, if successfull
 * false, if data size larger than memory size, or operation fails
 */
bool CSerialFlash::rangeProgram(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData)
{
      //return BulkPipeWrite(AddrRange, vData);
    return bulkPipeProgram(AddrRange, vData, m_codeProgram);
};

bool CSerialFlash::rangeRead(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData)
{
        //return BulkPipeRead(AddrRange, vData);
    return bulkPipeRead(AddrRange, vData, m_codeRead);
};


/**
 * \brief
 * poll the status register after Erase operation.
 * 
 * \returns
 * To be defined
 * 
 * (not fully implemented or used now)
 * 
 * \remarks
 * A polling command can be used to poll the status register after Erase operation
 * 
 */
bool CSerialFlash::DoPolling()
{
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vDataPack(4) ;      

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = POLLING ;
    rq.Value = (CTRL_TIMEOUT >> 16) & 0xffff ;
    rq.Index = CTRL_TIMEOUT & 0xffff ;
    rq.Length = static_cast<unsigned long>(vDataPack.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vDataPack))
        return false ;

    return true ;

}

/**
 * \brief
 * retrieves object wellness flag
 * 
 * before calling any other method, be sure to call is_good() to check the wellness  
 *  which indicates whether the instance is constructed successfully or not
 *
 * \returns
 * true only if all objects of chip info, chip itself, USB  are correctly constructed
 * false otherwise
 * 
 */
bool CSerialFlash::is_good()  
{
    return ( m_info.is_good() && m_usb.is_open() ) ;
}

tribool CSerialFlash::batchErase(const vector<size_t>& vAddrs)
{
    if(0 == m_code64KErase) return do_nothing;      //  chipErase code not initialised or not supported, please check chip class ctor.

    if(! m_usb.is_open())  return false ;

    if(! protectBlock(false) )  return false ;

    // send request
    CNTRPIPE_RQ rq ;                 
    vector<unsigned char> vInstruction(4, m_code64KErase) ;

    // instrcution format
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    BOOST_FOREACH(size_t addr, vAddrs)
    {
        waitForWEL() ;

        // MSB~ LSB (23...0)
        vInstruction[1] = static_cast<unsigned char>((addr >> 16) & 0xff) ;     //MSB
        vInstruction[2] = static_cast<unsigned char>((addr >> 8) & 0xff) ;      //M
        vInstruction[3] = static_cast<unsigned char>(addr & 0xff) ;             //LSB

        bool b = m_usb.OutCtrlRequest(rq, vInstruction);
        if((!b)|| m_isCanceled)  return false ;

        waitForWIP() ;
    }

    return true ;
}

tribool CSerialFlash::rangeErase(unsigned char cmd, size_t sectionSize, const pair<size_t, size_t>& AddrRange)
{
    if(! m_usb.is_open())  return false ;

    if(! protectBlock(false) )  return false ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(4, cmd) ;

    // instrcution format
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    size_t sectorNum = (AddrRange.second - AddrRange.first + sectionSize - 1) /  sectionSize ;

    for(size_t i = 0; i < sectorNum; ++ i)
    {
        waitForWEL() ;

        // MSB~ LSB (23...0)
        size_t addr = (AddrRange.first + i *  sectionSize) ;
        vInstruction[1] = static_cast<unsigned char>((addr >> 16) & 0xff) ;    //MSB
        vInstruction[2] = static_cast<unsigned char>((addr >> 8) & 0xff) ;    //M
        vInstruction[3] = static_cast<unsigned char>(addr & 0xff) ;    //LSB

        bool b = m_usb.OutCtrlRequest(rq, vInstruction);
        if((!b)|| m_isCanceled)  return false ;

        waitForWIP() ;
    }

    return true ;
}

/// chip erase
bool CSerialFlash::chipErase()
{
//    if(0 == m_codeChipErase) return do_nothing;      //  chipErase code not initialised or not supported, please check chip class ctor.

    if(! m_usb.is_open())   return false ;

    if(! protectBlock(false) )  return false ;

    waitForWEL() ;

    vector<unsigned char> v(1, m_codeChipErase);
    m_cmd.SendCommand_OutOnlyInstruction(v);

    waitForWIP() ;

   if(! protectBlock(m_bProtectAfterWritenErase) ) return false ;

   return true ;
}

tribool CSerialFlash::bulkPipeProgram(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeWrite)
{
    if(! m_usb.is_open())
        return false ;

    if(! protectBlock(false) )
        return false ;

    waitForWEL() ;

    m_cmd.SendCommand_SetupPacketForBulkWrite(AddrRange, modeWrite);


    vector<unsigned char>::const_iterator itr_begin;
    itr_begin = vData.begin();
    size_t divider = (PP_128BYTE == modeWrite) ?  7 : 8;
    size_t packageNum = (AddrRange.second - AddrRange.first) >> divider ;
    for(size_t i = 0; i < packageNum; ++ i)
    {
        bool b =  m_usb.BulkPipeWrite(vector<unsigned char>(itr_begin + (i << divider), itr_begin + ((i+1) << divider)), USB_TIMEOUT);
        if((!b) || m_isCanceled) return false ;
    }

    // wait wipis been done in F/W , here only needs Sleep 10 ms
    // notice : waiting for 10 ms only are only used on the occasion of bulk write / bulk AAI
    /*waitForWIP() ;*/
    ::Sleep(10) ;

    if(! protectBlock(m_bProtectAfterWritenErase) )      return false ;

    return true ;
}
tribool CSerialFlash::bulkPipeRead(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeRead)
{
    if(! m_usb.is_open())
        return false ;

    m_cmd.SendCommand_SetupPacketForBulkRead(AddrRange, modeRead);

    vData.clear();
    vData.reserve(AddrRange.second - AddrRange.first);
    vector<unsigned char> v(512);
    size_t pageNum = (AddrRange.second - AddrRange.first) >> 9 ;
    for(size_t i = 0; i < pageNum; ++ i)
    {
        bool b = m_usb.BulkPipeRead(v, USB_TIMEOUT);
        if((!b) || m_isCanceled) return false ;

        copy(v.begin(), v.begin() + 512 , back_inserter(vData)) ;
    }

    return true ;
}

void CSerialFlash::SetCancelOperationFlag()
{
    m_isCanceled = true;
}

void CSerialFlash::ClearCancelOperationFlag()
{
    m_isCanceled = false;
}

bool CSerialFlash::readSR(unsigned char& cSR)
{ 
    //doWREN(); 
    return doRDSR(cSR);
}

bool CSerialFlash::writeSR(unsigned char cSR)
{ 
    doWREN(); 
    return doWRSR(cSR);
}

bool CSerialFlash::is_protectbits_set()
{
    unsigned char sr;
    doRDSR(sr) ;

    return (0 != (sr & 0x0C) );
}



