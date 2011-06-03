#pragma once 
/************************************************************************/
/* TSI TS25Lxxx Serial Flashes
/************************************************************************/

#include "M25PExx.h"

class CTS25Lxxx : public CM25PExx
{
public:
    static wstring getClassName(){ return SUPPORT_TSI_TS25Lxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {
        WREN    = 0x06,                     // Write Enable
        WRDI    = 0x04,                     // Write Disable
        RDIDJ   = 0x9F,        //RDIDJ      // Read Jedec ID , except 80
        RDSR    = 0x05,                     // Read Status Register
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read
        PP      = 0x02,                     // Page Program
		PW		= 0x0A,						// Page Write
    	PE		= 0xDB,						// Page erase
		SE      = 0xD8,                     // Sector Erase
        CHIP_ERASE      = 0xC7,         //CHIP_ERASE        // Bulk (or Chip) Erase
        DP      = 0xB9,                     // Deep Power Down
        RDP     = 0xAB,        //RES        // Release Deep Power Down
        RES     = 0xAB,        //RES        // RDP and read signature
    };

public:
    CTS25Lxxx(CUSB& usb, const memory::memory_id& id) ;
};
