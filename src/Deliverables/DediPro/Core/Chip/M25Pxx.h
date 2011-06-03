#pragma once 

/************************************************************************/
/* ST M25Pxx Serial Flashes               
    Supported Operations :
    Common Instructions :
                            not supported           support
        WREN                
        WRDI
        RDSR
        WRSR
        READ
        FAST_READ
        PP
        SE
        BE
        DP                  M25P64                  M25P128
        RDP                 M25P64                  M25P128
        RDID                M25P05                  M25P80
        RES                 M25P128
        VPP                 M25P64                  M25P32

    ID table :
                            Jedec           ID        ES
        M25P128             0x202018        
        M25P64              0x202017        0x16            VPP
        M25P40              0x202013        0x12    DP
        M25P32              0x202016        0x15    DP        VPP
        M25P20              0x202012        0x11    DP
        M25P16              0x202015        0x14    DP
        M25P10A             0x202011        0x10    DP
        M25P05A             0x202010        0x05    DP
        M25P05                              0x10    DP
        M25P80                              0x13    DP

    Identification method : 
        (a) read Jedec ID
        if succeeded, recognize types ;
        if failed, the chip should be either M25P05 or M25P80 .
        (b) read ES for further identification. 
        Though M25P05is the same ES as that of M25P10A, only M25P05 requires reading ES for further identification.
*/
/************************************************************************/

#include "SerialFlash.h"


class CM25Pxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ST_M25Pxx; }
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
    CM25Pxx(CUSB& usb, const memory::memory_id& id) ;

};
