
#include "stdafx.h"
#include "M25PExx.h"
#include "..\USB\USBDevice.h"

CM25PExx::CM25PExx(CUSB& usb, const memory::memory_id& id)
    : CSerialFlash(usb, id) 
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = WRSR; 
    m_codeChipErase = CHIP_ERASE;
    m_codeProgram = PAGE_PROGRAM;
    m_codeRead    = BULK_FAST_READ;
    m_code64KErase= SE;
} 
  // Deep Power Down
//read chip lock register
bool CM25PExx::DoRDLR(vector<unsigned char>& vLR)
{
    // read status
    if(! m_usb.is_open() )
        return false ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(1) ;    //size 1

    // first control packet
    vInstruction[0] = RDLR ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction))
        return false ;

    // second control packet : fetch data
    vector<unsigned char> vBuffer(1) ;            //make sure 1 bytes of signature
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = TRANSCEIVE ;
    rq.Value = CTRL_TIMEOUT ;
    rq.Index = NO_REGISTER ;
    rq.Length = static_cast<unsigned long>(vBuffer.size()) ;

    if(! m_usb.InCtrlRequest(rq, vBuffer))
        return false ;

    vLR = vBuffer ;

    return true ;
}

//read locked register
bool CM25PExx::DoWRLR(const vector<unsigned char>& vLR)
{
    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(1) ;

    vInstruction[0] = WRLR ;
    copy(vLR.begin(), vLR.end(), back_inserter(vInstruction)) ;

    vInstruction[0] = WRLR ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction) )
        return false ;

    return true ;
}

