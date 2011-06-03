
#include "stdafx.h"
#include "M45PExx.h"
#include "..\USB\USBDevice.h"

CM45PExx::CM45PExx(CUSB& usb, const memory::memory_id& id)
    : CSerialFlash(usb, id) 
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = 0; 
    m_codeProgram = PAGE_PROGRAM;
    m_codeRead    = BULK_FAST_READ;
    m_code64KErase= SE;
} 

 
bool CM45PExx::chipErase()
{
    if(! m_usb.is_open())
        return false ;
   
    // erase whole chip
    pair<size_t, size_t> addrRange ;
    addrRange.first = 0 ;
    addrRange.second =  m_uniqueId.ChipSizeInByte;

//    return rangeErase(SE, Decwstring_to_Size_t(m_info.GetAttribValueByUniqueID(m_uniqueId, CChipInfo::SECTORSIZE_INBYTE)), addrRange);
    //return rangeErase(SE, Decwstring_to_Size_t(m_info.GetAttrib(m_uniqueId, CChipInfo::SECTORSIZE_INBYTE)), addrRange);
    return rangeErase(SE, m_uniqueId.SectorSizeInByte, addrRange);

}
