#pragma once 

#include "SerialFlash.h"

class CMCF : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_FREESCALE_MCF; }
private:

    enum    //list of all chip-specific instruction, for Freescale MCU
    {    
        RDSR        = 0x05,                     // Read SPI SR
        WRCR     = 0x01,                     // Write Config Register
        RDB        = 0x03,                        // Read Data Bytes
        FRDB    = 0x0B,                     // Fast Read Data Bytes
        PP        = 0x02,                     // Page Program
        SE      = 0xD8,                        // Sector Erase
        CHIP_ERASE      = 0xC7,                        // Bulk Erase
        WD        = 0x04,                     // Write Disable
        WE        = 0x06,                     // Write Enable
        CSFF    = 0x30,                     // Clear SR Fail Flags
        PBE     = 0x40,                     // Parameter Block Erase
        OTPP    = 0x42,                        // OTP Program
        RODB    = 0x4B,                     // Read OTP Data Bytes
        RID     = 0x9F,                        // Read ID
        RFDO    = 0xAB,                        // Release from DPD only
        DPD     = 0xB9,                        // Deep Power down
    };
   
private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    CMCF(CUSB& usb, const memory::memory_id& id) ;

};
