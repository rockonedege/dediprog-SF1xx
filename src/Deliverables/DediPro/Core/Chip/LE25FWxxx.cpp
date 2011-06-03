/// AMIC
#include "stdafx.h"
#include "LE25FWxxx.h"
#include "..\USB\USBDevice.h"

CLE25FWxxx::CLE25FWxxx(CUSB& usb, const memory::memory_id& id)
    : CSerialFlash(usb, id)
{
    m_codeRDSR = RDSR; 
    m_codeWRSR = WRSR; 
    m_codeChipErase = CHIP_ERASE;
    m_codeProgram = PAGE_PROGRAM;
    m_codeRead    = BULK_FAST_READ;
    m_code64KErase= SE;
}
 