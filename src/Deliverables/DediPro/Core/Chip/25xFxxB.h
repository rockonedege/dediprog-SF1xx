#pragma once 
/************************************************************************/
/* SST 25xFxxB Serial Flashes               
    ID
                        Jedec ID        ES
        25VF040B      0xBF258D    0x8D         AAI W
        25VF080B      0xBF258E    0x8E        AAI W
        25VF016B      0xBF2541    0x41        AAI W

Identification method : read Jedec ID or ES

*/
/************************************************************************/



#include "SerialFlash.h"


class C25xFxxB : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_SST_25xFxxB; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN = 0x06,                            //Write Enable
        WRDI = 0x04,                            //Write Disable
        RDSR = 0x05,                            //Read Status Register
        WRSR = 0x01,                            //Write Status Register
        EWSR = 0x50,                            //enable write status register
        RDIDJ = 0x9F,        //RDIDJ            //Read Jedec ID , (only for 40B/80B/16B)
        AAI = 0xAD,        //Word            //auto address increment program
        READ = 0x03,                            //Byte Read
        FREAD = 0x0B,                            //Fast Read        (10A/512A/40A/10A/20A)
        PP = 0x02,                                //Page Program
        SE = 0x20,                                //Sector Erase
        SE_64K = 0xD8,                                //Sector Erase
        BE = 0x52,            //32K                //block Erase
        CHIP_ERASE = 0x60,                                //chip erase
        RDID = 0xAB,        //RES                //release from deep power down and read signature
        EBSY = 0x70,                            //Enable SO
        DBSY = 0x80                            //Disable SO
    };

public :
private :
    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;

public:
    C25xFxxB(CUSB& usb, const memory::memory_id& id) ;

public:
    // list of all single instruction implementation, all are ctrl pipe
    
    bool DoEWSR() ;
    bool DoEBSY() ;
    bool DoDBSY() ;
    bool doWRSR(unsigned char cSR); //  virtual to override

};
