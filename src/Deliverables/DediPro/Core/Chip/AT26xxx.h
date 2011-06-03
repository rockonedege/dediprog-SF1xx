#pragma once 

/************************************************************************/
/* ATMEL AT26xxx Serial Flashes               
*/
/************************************************************************/

#include "SerialFlash.h"

class CAT26xxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ATMEL_AT26xxx; }
private:

    size_t m_chipSize;

    enum    //list of all chip-specific instruction, for ST serial flash
    {
        // Read Commands
        READ                        = 0x03,                     // Byte Read
        FREAD                       = 0x0B,                     // Fast Read

        // Program and Erase Commands 
        Page_Erase                    = 0x81,                        // Page   Erase
        CHIP_ERASE                    = 0xC7,          // Bulk (or Chip) Erase
        BYTEP                       = 0x02,                     // Page Program
        PP                          = 0xAF,                     // Page Program
        PP2                         = 0x11,                     // Page Program
        PP3                         = 0x82,                     // Page Program

        WREN                        = 0x06,                     // Write Enable
        WRDI                        = 0x04,                     // Write Disable

        // Protection Commands
        PROTECTSECTOR               = 0x36,                     // Write Enable
        UNPROTECTSECTOR             = 0x39,                     // Write Disable
        READPROTECTIONREGISTER      = 0x3C,                     // Write Disable

        // Status Register Commands
        RDSR                        = 0x05,                     // Read Status Register
        WRSR                        = 0x01,                     // Write Status Register

        // Miscellaneous Commands
        DP                          = 0xB9,                     // Deep Power Down
        RDP                         = 0xAB,        //RES        // Release Deep Power Down 

        BE_64K      = 0xD8,                     // Sector Erase
    };

private :

    tribool protectBlock(tribool bProtect) ;

public:
    CAT26xxx(CUSB& usb, const memory::memory_id& id) ;

public:
    // list of all single instruction implementation, all are ctrl pipe

    bool CAT26xxx::DoReadSPR(unsigned char& vSPR, size_t Addr);                   //read Sector protect Register

private:
    tribool BlockErase_2K(pair<unsigned int, unsigned int> addrRange);
    tribool BlockErase_4K(pair<unsigned int, unsigned int> addrRange);
    tribool BlockErase_32K(pair<unsigned int, unsigned int> addrRange);
    tribool BlockErase_64K(pair<unsigned int, unsigned int> addrRange);
private:
    void waitForWEL();          // override
    bool  chipErase();    // override
private:
    static const unsigned int AT26DF041 = 0x1F4400;
    static const unsigned int AT26DF004 = 0x1F0400;
    static const unsigned int AT26DF081A = 0x1F4501;
};
