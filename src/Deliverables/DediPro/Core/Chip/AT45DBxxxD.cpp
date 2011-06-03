#include "StdAfx.h"
#include "AT45DBxxxD.h"
#include "..\USB\USBDevice.h"

#include <iterator>

#include <boost/assign.hpp>
#include <boost/array.hpp>
#include <boost/format.hpp>

#include "..\..\3rd\Logger\Logger.h"

using namespace boost::assign;


CAT45DBxxxD::CAT45DBxxxD(CUSB& usb, const memory::memory_id& id)
            :CSerialFlash(usb, id)           
{

    m_codeRDSR = RDSR; 
    m_codeWRSR = 0;
    m_codeRead    = BULK_FAST_READ;

    //m_codeChipErase = CHIP_ERASE;

    unsigned char writeMode;
    writeMode     = getWriteMode();
    m_codeProgram =  writeMode;

    boost::array<size_t, 7> pageSize = {0, 256, 264, 512, 528, 1024, 1056};
    boost::array<size_t, 7> pageSizeMask = {0, (1<<8)-1, (1<<9)-1, (1<<9)-1, (1<<10)-1, (1<<10)-1, (1<<11)-1};
    m_pageSize = pageSize[writeMode];
    m_pageSizeMask = pageSizeMask[writeMode];
    
    m_chipSize = id.ChipSizeInByte;
    m_addrSpace = m_chipSize;
    if(! (writeMode & 0x1) )            // for AT45DB:0x1F2200 - 0x1F2800
    {
        m_chipSize += m_chipSize / 256 * 8;    
        m_addrSpace = m_chipSize<<1;
    }

}

size_t CAT45DBxxxD::GetChipSizeInBytes() const
{
    return m_chipSize;
}
size_t CAT45DBxxxD::GetPageSizeInBytes() const
{
    return m_pageSize;
}

void CAT45DBxxxD::waitForWEL()  // override
{

    unsigned char cSR ;
    size_t i = MAX_TRIALS;
    // wait until WIP = 0 and WEL = 1
    do{
        doWREN() ;

        // read SR to check WEL until WEL = 1
        doRDSR(cSR) ;
    }while((cSR & 0x02)  && (i-- > 0)) ;

}

unsigned char CAT45DBxxxD::getWriteMode()
{
    unsigned char cMode;
    unsigned char cSR ;
    doRDSR(cSR) ;

    bool powerOfTwo = (1 == (cSR & 0x1));

    switch(m_uniqueId.UniqueID)
    {
        case AT45DB011D:
        case AT45DB021D:
        case AT45DB041D:
        case AT45DB081D:
            cMode = powerOfTwo ? 1 : 2; //256 : 264;
            break;
        case AT45DB161D:
        case AT45DB321D:
            cMode = powerOfTwo ? 3 : 4; //512 : 528;
            break;
        case AT45DB642D:
            cMode = powerOfTwo ? 5 : 6; //1024 : 1056;
            break;
        default:
            cMode = powerOfTwo ? 1 : 2; //256 : 264;
            ;
    }
    return cMode;

}

void CAT45DBxxxD::waitForWIP()  // override
{    
    unsigned char cSR ;
    size_t i = MAX_TRIALS;
    // wait until WIP = 0
    do{
        doRDSR(cSR) ;

        if( m_isCanceled) break;

    }while((!(cSR & 0x80)) && (i-- > 0)) ;  // poll bit 7

}
//// set BP2 BP1 BP0 to 0 for SE,BE,PE,PP ...
tribool CAT45DBxxxD::protectBlock(tribool bProtect)
{
    // SR :
    //  7   6 5  4   3   2   1   0
    // SRWD 0 0 BP2 BP1 BP0 WEL WIP
    // SR , only one byte

    if((is_protectbits_set() == bProtect) || boost::logic::indeterminate(bProtect)) return do_nothing;


    bool result = false ;
    unsigned char  tmpSRVal;
    unsigned char dstSRVal ;

    // un-protect block ,set [BP2] BP1 BP2 to 0
    dstSRVal = 0x00 ;
    // protect block ,set [BP2] BP1 BP2 to 1
    if(bProtect)
    {
        dstSRVal += 0x9C ;    // 9C : 1001 1100 
    }

    // WRSR only  s effects on SRWD [BP2] BP1 BP0
    int numOfRetry = 1000 ;
    result = doWRSR(dstSRVal) ;
    result = doRDSR(tmpSRVal) ;
    while((!(tmpSRVal & 0x80)) &&  numOfRetry > 0) // WIP = TRUE;
    {
        // read SR
        result = doRDSR(tmpSRVal) ;

        if(! result) return false;

        numOfRetry -- ;

    }; 

    //// to implemented be 
    return true;////((tmpSRVal ^ dstSRVal)& 0x02 ) ? false:true;
}


bool CAT45DBxxxD::EnableEraseSectorProtectionRegister()
{
    vector<unsigned char> v;
    v +=  0x3D,0x2A,0x7F,0xA9;
    return m_cmd.SendCommand_OutOnlyInstruction(v);
}

bool CAT45DBxxxD::EraseSectorProtectionRegister()
{
    vector<unsigned char> v;
    v +=  0x3D,0x2A,0x7F,0xCF;
    return m_cmd.SendCommand_OutOnlyInstruction(v);
}

bool CAT45DBxxxD::ProgramSectorProtectionRegister(const vector<unsigned char>& vc)
{
    vector<unsigned char> v;
    v +=  0x3D,0x2A,0x7F,0xFC;

    std::copy(vc.begin(), vc.end(), std::back_inserter<vector<unsigned char> >(v));

    return m_cmd.SendCommand_OutOnlyInstruction(v);
}

vector<unsigned char> CAT45DBxxxD::ReadSectorProtectionRegister()
{
    vector<unsigned char> vIn, vOut;
    vIn +=  0x32,0x00,0x00,0x00;            // out

    // to be done

    return  vOut;
}

bool CAT45DBxxxD::ValidateAddr(size_t addr)
{
    return (m_pageSizeMask  &  addr) <=  (m_pageSizeMask &  m_pageSize);
}


tribool CAT45DBxxxD::bulkPipeProgram(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeWrite)
{
    /*  modeWrite:
        1: page-size = 256
        2: page-size = 264
        3: page-size = 512
        4: page-size = 528
        5: page-size = 1024
        6: page-size = 1056
    */

    if(! m_usb.is_open())
        return false ;

    if(! protectBlock(false) )
        return false ;

    PrependPageModeInfo();

    waitForWEL() ;

    m_cmd.SendCommand_SetupPacketForAT45DBBulkWrite(AddrRange, modeWrite);

    size_t packageNum = (AddrRange.second - AddrRange.first + m_pageSize - 1) / m_pageSize ;
    size_t oldSize = vData.size();
    // vData.resize(packageNum * m_pageSize);
    vData.insert(vData.end(), packageNum * m_pageSize - oldSize, 0xFF); 

    boost::array<size_t,2> pageSize = { 264, 256};
    unsigned char idx = modeWrite & 0x1;

    size_t itrCnt =  packageNum *  m_pageSize / pageSize[idx];

    vector<unsigned char>::const_iterator itr_begin;
    itr_begin = vData.begin();

    for(size_t i = 0; i < itrCnt; ++ i)
    {
        bool b =  m_usb.BulkPipeWrite(vector<unsigned char>(itr_begin + (i * pageSize[idx]), itr_begin + ((i+1) * pageSize[idx])), USB_TIMEOUT);
        if((!b) || m_isCanceled) return false ;
    }
    vData.resize(oldSize);

    // wait wipis been done in F/W , here only needs Sleep 10 ms
    // notice : waiting for 10 ms only are only used on the occasion of bulk write / bulk AAI
    /*waitForWIP() ;*/
    ::Sleep(10) ;

    if(! protectBlock(m_bProtectAfterWritenErase) )      return false ;

    return true ;
}

bool  CAT45DBxxxD::chipErase()
{
    using std::make_pair;

    switch(m_chipSize)
    {
        case (1<<17):           // 1Mbit
            rangeErase(SECTOR_ERASE, 1<<11, make_pair(0, 1<<12));
            rangeErase(SECTOR_ERASE, 1<<15, make_pair(1<<15, m_addrSpace));
            break;
        case (1<<17)*264/256:   // 1Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<16, make_pair(1<<16, m_addrSpace));
            break;
        case (1<<18):           // 2Mbit
            rangeErase(SECTOR_ERASE, 1<<11, make_pair(0, 1<<12));
            rangeErase(SECTOR_ERASE, 1<<15, make_pair(1<<15, m_addrSpace));
            break;
        case (1<<18)*264/256:   // 2Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<16, make_pair(1<<16, m_addrSpace));
            break;
        case (1<<19):           // 4Mbit
            rangeErase(SECTOR_ERASE, 1<<11, make_pair(0, 1<<12));
            rangeErase(SECTOR_ERASE, 1<<16, make_pair(1<<16, m_addrSpace));
            break;
        case (1<<19)*264/256:   // 4Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<17, make_pair(1<<17, m_addrSpace));
            break;
        case (1<<20):           // 8Mbit
            rangeErase(SECTOR_ERASE, 1<<11, make_pair(0, 1<<12));
            rangeErase(SECTOR_ERASE, 1<<16, make_pair(1<<16, m_addrSpace));
            break;
        case (1<<20)*264/256:   // 8Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<17, make_pair(1<<17, m_addrSpace));
            break;
        case (1<<21):           // 16Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<17, make_pair(1<<17, m_addrSpace));
            break;
        case (1<<21)*264/256:   // 16Mbit
            //rangeErase(SECTOR_ERASE, 0x1000, make_pair(0, 0x1001));
            //rangeErase(SECTOR_ERASE, 0x40000, make_pair(0x20000, 0x400000/*m_chipSize*/));
            rangeErase(SECTOR_ERASE, 1<<13, make_pair(0, 1<<14));
            rangeErase(SECTOR_ERASE, 1<<18, make_pair(1<<18, m_addrSpace));
            break;
        case (1<<22):           // 32Mbit
            rangeErase(SECTOR_ERASE, 1<<12, make_pair(0, 1<<13));
            rangeErase(SECTOR_ERASE, 1<<18, make_pair(1<<18, m_addrSpace));
            break;
        case (1<<22)/256*264:   // 32Mbit
            rangeErase(SECTOR_ERASE, 1<<13, make_pair(0, 1<<14));
            rangeErase(SECTOR_ERASE, 1<<17, make_pair(1<<17, m_addrSpace));
            break;
        case (1<<23):           // 64Mbit
            rangeErase(SECTOR_ERASE, 1<<13, make_pair(0, 1<<14));
            rangeErase(SECTOR_ERASE, 1<<18, make_pair(1<<18, m_addrSpace));
            break;
        case (1<<23)/256*264:   // 64Mbit
            rangeErase(SECTOR_ERASE, 1<<14, make_pair(0, 1<<15));
            rangeErase(SECTOR_ERASE, 1<<19, make_pair(1<<19, m_addrSpace));
            break;
        default:
            ; //CSerialFlash::chipErase();
    };
    return true;
}

void CAT45DBxxxD::PrependPageModeInfo()
{
    boost::wformat fmt(L"Current working context: chip size = %1$#x bytes, page size = %2% bytes");
    fmt%GetChipSizeInBytes()%GetPageSizeInBytes() ;
    Logger::LogIt(fmt.str());
}
