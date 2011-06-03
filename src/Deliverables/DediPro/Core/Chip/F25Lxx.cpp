
#include "stdafx.h"
#include "F25Lxx.h"
#include "..\USB\USBDevice.h"

CF25Lxx::CF25Lxx(CUSB& usb, const memory::memory_id& id)
    : CSerialFlash(usb, id)
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = WRSR; 
    m_codeChipErase = CHIP_ERASE;
    m_code64KErase= BE;

    if(F25L04UA == m_uniqueId.UniqueID)
    {
        m_codeProgram = AAI_1_BYTE;
    }
    else
    {
        m_codeProgram = AAI_2_BYTE;//PAGE_PROGRAM;
    }
    m_codeRead    = BULK_FAST_READ;
}

//// set BP2 BP1 BP0 to 0 for SE,BE,PE,PP ...
tribool CF25Lxx::protectBlock(tribool bProtect)
{
    // SR :
    //    7       6 5 4    3    2    1     0
    // SRWD 0 0 BP2 BP1 BP0 WEL WIP
    // SR , only one byte

    if((is_protectbits_set() == bProtect) || boost::logic::indeterminate(bProtect)) return do_nothing;

    bool result = false ;
    unsigned char  tmpSRVal;
    unsigned char dstSRVal ;
    //int numOfRetry = 3 ;

    // un-protect block ,set BP2 BP1 BP2 to 0
    dstSRVal = 0x00 ;
    // protect block ,set BP2 BP1 BP2 to 1
    if(bProtect){
        dstSRVal += 0x9C ;    // 9C : 9001 1100
    }

    int numOfRetry = 1000 ;
    result = doWRSR(dstSRVal) ;
    result = doRDSR(tmpSRVal) ;
    while( (tmpSRVal & 0x01) &&  numOfRetry > 0) // WIP = TRUE;
    {
        // read SR
        result = doRDSR(tmpSRVal) ;

        if(! result) return false;

        numOfRetry -- ;

    };

    return ((tmpSRVal ^ dstSRVal)& 0x0C ) ? false:true;
}

 // conjunct with WRSR command to make both effect
bool CF25Lxx::DoEWSR()
{
    if(! m_usb.is_open())
        return false ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(1) ;

    vInstruction[0] = EWSR ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction))
        return false ;

    return true ;
}

bool CF25Lxx::doWRSR(unsigned char cSR) //  virtual to override
{
    if(! m_usb.is_open())
        return false ;

    // wait until WIP = 0
    waitForWIP() ;

    // wait until WEL = 1
    DoEWSR() ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction ;

    vInstruction.push_back(m_codeWRSR) ;
    vInstruction.push_back(cSR) ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = NO_RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction))
        return false ;

    return true ;
}
