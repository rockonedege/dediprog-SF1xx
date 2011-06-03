/** 
  * @brief  SST 25xFxx Serial Flashes               
  *
  *      25VF512            0xBF    0x48    20        AAI S
  *      25VF040            0xBF    0x44    20        AAI S
  *      25VF020            0xBF    0x43    20        AAI S
  *      25VF010            0xBF    0x49    20        AAI S
  *
  *
  */
#pragma once 
#include "SerialFlash.h"

/** 
  *@brief The class to support 25xFxx memories
  *
  */
class C25xFxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_SST_25xFxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN = 0x06,                            //Write Enable
        WRDI = 0x04,                            //Write Disable
        RDSR = 0x05,                            //Read Status Register
        WRSR = 0x01,                            //Write Status Register
        EWSR = 0x50,                            //enable write status register
        AAI  = 0xAF,                                //auto address increment program
        READ = 0x03,                            //Byte Read
        PP   = 0x02,                                //Page Program
        SE   = 0x20,                                //Sector Erase
        BE   = 0x52,                                //block Erase
        CHIP_ERASE   = 0x60,                                //chip erase
        RDID = 0xAB,        //RES                //release from deep power down and read signature
    };

public :

private :

    // protect block for erase/program or not 
    virtual tribool protectBlock(tribool bProtect) ;

public:
    C25xFxx(CUSB& usb, const memory::memory_id& id) ;

public:
    
    bool DoEWSR() ;
    bool doWRSR(unsigned char cSR); //  override 

};
