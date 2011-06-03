#pragma once 

#include "SerialFlash.h"

class CxxxS33x : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_INTEL_S33; }
private:

    enum    //list of all chip-specific instruction, for INTEL-Blanshard serial flash
    {    
        WSS     = 0x01,                     // Write SPI SR
        PP        = 0x02,                     // Page Program
        RDB        = 0x03,                        // Read Data Bytes
        WD        = 0x04,                     // Write Disable
        RSS        = 0x05,                     // Read SPI SR
        WE        = 0x06,                     // Write Enable
        FRDB    = 0x0B,                     // Fast Read Data Bytes
        CSFF    = 0x30,                     // Clear SR Fail Flags
        PBE     = 0x40,                     // Parameter Block Erase
        OTPP    = 0x42,                        // OTP Program
        RODB    = 0x4B,                     // Read OTP Data Bytes
        RID     = 0x9F,                        // Read ID
        RFDO    = 0xAB,                        // Release from DPD only
        DPD     = 0xB9,                        // Deep Power down
        CHIP_ERASE      = 0xC7,                        // Bulk Erase
        SE      = 0xD8,                        // Sector Erase
    };
   
private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    CxxxS33x(CUSB& usb, const memory::memory_id& id) ;

};
