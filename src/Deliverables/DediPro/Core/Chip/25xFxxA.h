#pragma once 
/************************************************************************/
/* SST 25xFxxA Serial Flashes               
        ID          ES        Clock
        25VF512A    0xBF    0x48    33        AAI S
        25LF040A    0xBF    0x44    33        AAI S
        25LF020A    0xBF    0x43    33        AAI S
        25VF010A    0xBF    0x49    33        AAI S

        Identification method : read ES ID

*/
/************************************************************************/

#include "SerialFlash.h"


class C25xFxxA : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_SST_25xFxxA; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN = 0x06,                            //Write Enable
        WRDI = 0x04,                            //Write Disable
        RDSR = 0x05,                            //Read Status Register
        WRSR = 0x01,                            //Write Status Register
        EWSR = 0x50,                            //enable write status register
        AAI = 0xAF,                             //auto address increment program
        READ = 0x03,                            //Byte Read
        FREAD = 0x0B,                           //Fast Read        (10A/512A/40A/10A/20A)
        PP = 0x02,                              //Page Program
        SE = 0x20,                              //Sector Erase
        BE = 0x52,                              //block Erase
        CHIP_ERASE = 0x60,                              //chip erase
        RDID = 0xAB,        //RES               //release from deep power down and read signature
    };

private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    C25xFxxA(CUSB& usb, const memory::memory_id& id) ;

public:
    //#1.no input parameter , no return value
    
    
    bool DoEWSR() ;
    bool doWRSR(unsigned char cSR); //  virtual to override
    
};
