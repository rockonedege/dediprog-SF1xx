#pragma once 

/************************************************************************/
/* ATMEL AT25Fxxx Serial Flashes               
*/
/************************************************************************/

#include "SerialFlash.h"

class CAT25Fxxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ATMEL_AT25Fxxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {
        // Read Commands
        READ                        = 0x03,                     // Byte Read

        // Program and Erase Commands 
        SE                          = 0x52,                     // Sector Erase
        CHIP_ERASE                  = 0x62,         //CHIP_ERASE        // Bulk (or Chip) Erase
        PP                          = 0x02,                     // Page Program

        WREN                        = 0x06,                     // Write Enable
        WRDI                        = 0x04,                     // Write Disable

        // Status Register Commands
        RDSR                        = 0x05,                     // Read Status Register
        WRSR                        = 0x01,                     // Write Status Register

        // Miscellaneous Commands
        RDIDJ                        = 0x15,        //RDIDJ      // Read Jedec ID , except 80
    };

private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;
public:
    CAT25Fxxx(CUSB& usb, const memory::memory_id& id) ;
};
