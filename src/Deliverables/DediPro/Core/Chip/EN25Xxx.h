#pragma once 

/************************************************************************/
/* Eon EN25Pxx Serial Flashes               
*/
/************************************************************************/

#include "SerialFlash.h"


class CEN25Xxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_EON_EN25Xxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN    = 0x06,                     // Write Enable
        WRDI    = 0x04,                     // Write Disable
        RDIDJ   = 0x90,        //RDIDJ      // Read Jedec ID , except 80
        RDSR    = 0x05,                     // Read Status Register
        WRSR    = 0x01,                     // Write Status Register
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read
        PP      = 0x02,                     // Page Program
        SE      = 0xD8,                     // Sector Erase
        CHIP_ERASE      = 0xC7,             // CHIP_ERASE        // Bulk (or Chip) Erase
        DP      = 0xB9,                     // Deep Power Down
        RDP     = 0xAB,        //RES        // Release Deep Power Down 
        RES     = 0xAB,        //RES        // RDP and read signature
    };

private :
    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    CEN25Xxx(CUSB& usb, const memory::memory_id& id) ;

};
