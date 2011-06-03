
#include "stdafx.h"
#include "AT26xxx.h"
#include <utility>
#include "..\USB\USBDevice.h"

CAT26xxx::CAT26xxx(CUSB& usb, const memory::memory_id& id) 
    : CSerialFlash(usb, id)
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = WRSR; 
    m_codeChipErase = CHIP_ERASE;
    m_codeRead    = BULK_FAST_READ;
    m_code64KErase= BE_64K;

    m_chipSize =  m_uniqueId.ChipSizeInByte;

    if(AT26DF041 == m_uniqueId.UniqueID)
    {
        m_codeProgram = PP_AT26DF041;
    }
    else if(AT26DF004 == m_uniqueId.UniqueID)
    {
        m_codeProgram = AAI_1_BYTE;
    }
    else
    {
        m_codeProgram = PAGE_PROGRAM;
    }
} 

//// set BP2 BP1 BP0 to 0 for SE,BE,PE,PP ...
tribool CAT26xxx::protectBlock(tribool bProtect)
{
    // SR :
    //    7       6 5 4    3    2    1     0
    // SRWD 0 0 BP2 BP1 BP0 WEL WIP
    // SR , only one byte

    if((is_protectbits_set() == bProtect) || boost::logic::indeterminate(bProtect)) return do_nothing;

    if(AT26DF041 == m_uniqueId.UniqueID) return true; // feature is not supported on this chip 

    bool result = false ;

    int numOfRetry = 1000 ;

    result = doWREN();
    result = doWRSR(0) ;

    unsigned char   tmpSRVal;
    do 
    {
        result = doRDSR(tmpSRVal) ;
        numOfRetry -- ;

    }while( (tmpSRVal & 0x01) &&  numOfRetry > 0 && result); 


    if (tmpSRVal & 0x80 ) return false;   ///< enable SPRL


    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(4) ;

    vInstruction[0] = bProtect ? PROTECTSECTOR : UNPROTECTSECTOR ;

    size_t iUniformSectorSize = 0x10000;    // always regarded as 64K
    size_t cnt = m_chipSize / iUniformSectorSize; 
    for(size_t i = 0; i < cnt; ++i)
    {
        doWREN();

        vInstruction[1] = static_cast<unsigned char>(i);

        rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
        rq.Direction = VENDOR_DIRECTION_OUT ;
        rq.Request = TRANSCEIVE ;
        rq.Value = RFU ;
        rq.Index = NO_RESULT_IN ;
        rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

        if(! m_usb.OutCtrlRequest(rq, vInstruction) )
            return false ;
    }

    if(AT26DF081A == m_uniqueId.UniqueID || AT26DF004 == m_uniqueId.UniqueID) // 8K each for the last 64K
    {
        vInstruction[1] = static_cast<unsigned char>(cnt - 1);
        for(size_t i = 1; i < 8; ++i)
        {
            doWREN();

            vInstruction[2] = static_cast<unsigned char>(i<<5);

            rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
            rq.Direction = VENDOR_DIRECTION_OUT ;
            rq.Request = TRANSCEIVE ;
            rq.Value = RFU ;
            rq.Index = NO_RESULT_IN ;
            rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

            if(! m_usb.OutCtrlRequest(rq, vInstruction) )
                return false ;
        }   
    }

    return true;
}

bool CAT26xxx::DoReadSPR(unsigned char& vSPR, size_t Addr)
{
    // read status
    if(! m_usb.is_open() )
        return false ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction ;    //size 1

    // first control packet
    vInstruction.push_back(READPROTECTIONREGISTER) ;
    vInstruction.push_back(static_cast<unsigned char>(Addr>>16)) ;
    vInstruction.push_back(static_cast<unsigned char>(Addr>>8)) ;
    vInstruction.push_back(static_cast<unsigned char>(Addr)) ;

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

    vSPR = vBuffer.front() ;

    return true ;

}

  // Deep Power Down
tribool CAT26xxx::BlockErase_4K(pair<unsigned int, unsigned int> addrRange)
{
    const unsigned char BLOCKER_4K = 0x20;
    return rangeErase(BLOCKER_4K, 0x01000,addrRange);
}

tribool CAT26xxx::BlockErase_32K(pair<unsigned int, unsigned int> addrRange)
{
    if(AT26DF041 == m_uniqueId.UniqueID) return do_nothing;

    const unsigned char BLOCKER_32K = 0x52;
    return rangeErase(BLOCKER_32K, 0x08000,addrRange);
}

tribool CAT26xxx::BlockErase_64K(pair<unsigned int, unsigned int> addrRange)
{
    if(AT26DF041 == m_uniqueId.UniqueID) return do_nothing;

    const unsigned char BLOCKER_64K = 0xD8;
    return rangeErase(BLOCKER_64K, 0x10000,addrRange);
}

void CAT26xxx::waitForWEL()
{
    if(AT26DF041 == m_uniqueId.UniqueID) return;

    CSerialFlash::waitForWEL();

    return;
}


// chip erase 
bool  CAT26xxx::chipErase()
{
    if(! m_usb.is_open())
        return false ;

    // AT26DF041 does not support Chip Erase
    if(AT26DF041 == m_uniqueId.UniqueID) 
        return BlockErase_4K(std::make_pair(0, m_uniqueId.ChipSizeInByte));
    else
        return CSerialFlash::chipErase();

}

