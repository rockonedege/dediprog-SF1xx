///     CFlashCommand Implementations 

#include "FlashCommand.h"
#include "macro.h"
#include "USBDevice.h"

#include <boost/array.hpp>

CFlashCommand::CFlashCommand(CUSB& usb)
    :m_usb(usb)
{}

bool CFlashCommand::TransceiveOut(const vector<unsigned char>& v, bool has_result_in)
{
    CNTRPIPE_RQ rq ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;               
    rq.Value = RFU ;
	rq.Index = has_result_in? RESULT_IN : NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(v.size()) ;

    return m_usb.OutCtrlRequest(rq, v);
}

bool CFlashCommand::TransceiveIn(vector<unsigned char>& v)
{
    CNTRPIPE_RQ rq ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = TRANSCEIVE ;
    rq.Value = CTRL_TIMEOUT ;
    rq.Index = NO_REGISTER ;
    rq.Length = static_cast<unsigned long>(v.size()) ;

    return m_usb.InCtrlRequest(rq, v);
}

bool CFlashCommand::SendCommand_OutOnlyInstruction(const vector<unsigned char>& v)
{
   return TransceiveOut(v, false);
}

bool CFlashCommand::SendCommand_OneOutOneIn(const vector<unsigned char>& vOut, vector<unsigned char>& vIn )
{
	if(!TransceiveOut(vOut, true)) return false;
	if(!TransceiveIn(vIn)) return false;
    return true;
}

bool CFlashCommand::SendCommand_SetupPacketForBulkWrite(const pair<size_t, size_t>& AddrRange, unsigned char modeWrite)
{
    vector<unsigned char> vInstruction(4) ;

    // length in terms of 256/128 bytes
    size_t divider = (PP_128BYTE == modeWrite) ?  7 : 8;

    size_t pageNum = (AddrRange.second - AddrRange.first) >> divider ;

    vInstruction[0] = static_cast<unsigned char>(pageNum & 0xff) ;                // lowest byte of length : page number
    vInstruction[1] = static_cast<unsigned char>( (pageNum >> 8) &  0xff) ;        // highest byte of length: page number
    vInstruction[2] = 0x00 ;                                                      // reserved
    vInstruction[3] = modeWrite;        // PAGE_PROGRAM, PAGE_WRITE, AAI_1_BYTE, AAI_2_BYTE, PP_128BYTE, PP_AT26DF041


    CNTRPIPE_RQ rq ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = WRITE ;
    rq.Value = static_cast<unsigned short>(AddrRange.first  & 0xffff) ;           //16 bits LSB
    rq.Index = static_cast<unsigned short>((AddrRange.first >> 16) & 0xffff) ;    //16 bits MSB
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    // send rq via control pipe
    return m_usb.OutCtrlRequest(rq, vInstruction);
}

bool CFlashCommand::SendCommand_SetupPacketForAT45DBBulkWrite(const pair<size_t, size_t>& AddrRange, unsigned char modeWrite)
{
    /*  modeWrite:
        1: page-size = 256
        2: page-size = 264
        3: page-size = 512
        4: page-size = 528
        5: page-size = 1024
        6: page-size = 1056
    */
    boost::array<size_t,7> pageSize = { 0, 256, 264, 512, 528, 1024, 1056};

    size_t pageNum = (AddrRange.second - AddrRange.first + pageSize[modeWrite] - 1) / pageSize[modeWrite] ;

    vector<unsigned char> vInstruction(4) ;
    vInstruction[0] = static_cast<unsigned char>(pageNum & 0xff) ;                // lowest byte of length : page number
    vInstruction[1] = static_cast<unsigned char>( (pageNum >> 8) &  0xff) ;        // highest byte of length: page number
    vInstruction[2] = 0x00 ;                                                      // reserved
    vInstruction[3] = modeWrite; 


    CNTRPIPE_RQ rq ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = ATMEL45_WRITE ;
    rq.Value = static_cast<unsigned short>(AddrRange.first  & 0xffff) ;           //16 bits LSB
    rq.Index = static_cast<unsigned short>((AddrRange.first >> 16) & 0xffff) ;    //16 bits MSB
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    // send rq via control pipe
    return m_usb.OutCtrlRequest(rq, vInstruction);
}


bool CFlashCommand::SendCommand_SetupPacketForBulkRead(const pair<size_t, size_t>& AddrRange, unsigned char modeRead)
{
    vector<unsigned char> vInstruction(4) ;

    // length in terms of 256 bytes
    size_t pageNum = (AddrRange.second - AddrRange.first) >> 9 ;

    vInstruction[0] = static_cast<unsigned char>(pageNum & 0xff) ;                // lowest byte of length : page number
    vInstruction[1] = static_cast<unsigned char>( (pageNum >> 8) &  0xff) ;        // highest byte of length: page number
    vInstruction[2] = 0x00 ;                                                      // reserved
    vInstruction[3] = modeRead; // BULK_NORM_READ, BULK_FAST_READ


    CNTRPIPE_RQ rq ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = DTC_READ ;
    rq.Value = static_cast<unsigned short>(AddrRange.first  & 0xffff) ;           //16 bits LSB
    rq.Index = static_cast<unsigned short>((AddrRange.first >> 16) & 0xffff) ;    //16 bits MSB
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    // send rq via control pipe
    return m_usb.OutCtrlRequest(rq, vInstruction);
}