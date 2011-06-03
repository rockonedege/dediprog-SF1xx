#pragma once 

/************************************************************************/
/* ESMT F25Lxx Serial Flashes
 */
/************************************************************************/

#include "SerialFlash.h"


class CF25Lxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ESMT_F25Lxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {
        WREN    = 0x06,                     // Write Enable
        WRDI    = 0x04,                     // Write Disable
        RDIDJ   = 0x9F,        //RDIDJ      // Read Jedec ID , except 80
        RDSR    = 0x05,                     // Read Status Register
        WRSR    = 0x01,                     // Write Status Register
        EWSR    = 0x50,                            //enable write status register
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read
        PP      = 0x02,                     // Page Program
        AAI     = 0xAF,                            //enable write status register
        SE      = 0x20,                     // Sector Erase
        BE      = 0xD8,                     // BLOCK Erase
        CHIP_ERASE      = 0x60,         //CHIP_ERASE        // Bulk (or Chip) Erase
        RES     = 0xAB,
   };

private :

    // protect block for erase/program or not
    tribool protectBlock(tribool bProtect) ;

 public:
    CF25Lxx(CUSB& usb, const memory::memory_id& id) ;

public:
    // list of all single instruction implementation, all are ctrl pipe
    //#1.no input parameter , no return value
    bool DoEWSR() ;
    
    bool doWRSR(unsigned char cSR); //  virtual to override
     
private:
    static const unsigned int F25L04UA =  0x8C8C8C;

};
