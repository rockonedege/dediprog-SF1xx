#pragma once 

/************************************************************************/
/* Spansion M25Pxx Serial Flashes               
    ID table :
                            Jedec           ID        ES
        S25FL004A           0x010212        
        S25FL008A           0x010213        0x13            VPP
        S25FL016A           0x010214        0x14    DP
        S25FL032A           0x010215        0x15    DP        VPP
        S25FL064A           0x010216        0x16    DP
*/
/************************************************************************/

#include "SerialFlash.h"

class CS25FLxxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_SPANSION_S25FLxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN    = 0x06,                     // Write Enable
        WRDI    = 0x04,                     // Write Disable
        RDIDJ   = 0x9F,        //RDIDJ      // Read Jedec ID , except 80
        RDSR    = 0x05,                     // Read Status Register
        WRSR    = 0x01,                     // Write Status Register
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read
        PP      = 0x02,                     // Page Program
        SE      = 0xD8,                     // Sector Erase
        CHIP_ERASE      = 0xC7,         //CHIP_ERASE        // Bulk (or Chip) Erase
        DP      = 0xB9,                     // Deep Power Down
        RDP     = 0xAB,        //RES        // Release Deep Power Down 
        RES     = 0xAB,        //RES        // RDP and read signature
    };

private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    CS25FLxxx(CUSB& usb, const memory::memory_id& id) ;
};
