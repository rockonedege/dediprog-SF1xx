
#include "stdafx.h"
#include "AT25FSxxx.h"
#include "..\USB\USBDevice.h"

CAT25FSxxx::CAT25FSxxx(CUSB& usb, const memory::memory_id& id) 
    : CSerialFlash(usb, id)
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = WRSR; 
    m_codeChipErase = CHIP_ERASE;
    m_codeProgram = PP_128BYTE;
    m_codeRead    = BULK_NORM_READ;
    m_code64KErase  = BE_64K;
} 

//// set BP2 BP1 BP0 to 0 for SE,BE,PE,PP ...
tribool CAT25FSxxx::protectBlock(tribool bProtect)
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
        dstSRVal += 0xFF ;    // 9C : 9001 1100 
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
