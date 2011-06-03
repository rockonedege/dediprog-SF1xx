#pragma once 

/************************************************************************/
/* Winbond W25Xxx Serial Flashes               
    ID
                    Jedec ID        ES
        W25X10    0xEF3011    0x10
        W25X20    0xEF3012    0x11
        W25X40    0xEF3013    0x12
        W25X80    0xEF3014    0x13
        W25X16    0xEF3015    0x14
        W25X32    0xEF3016    0x15

    Identification method : read Jedec ID or ES

*/
/************************************************************************/

#include "SerialFlash.h"

class CW25Xxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_WINBOND_W25Xxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN = 0x06,                            //Write Enable
        WRDI = 0x04,                            //Write Disable
        RDIDJ = 0x9F,        //RDIDJ            //Read Jedec ID , except 80
        RDSR = 0x05,                            //Read Status Register
        WRSR = 0x01,                            //Write Status Register
        READ = 0x03,                            //Byte Read
        FREAD = 0x0B,                            //Fast Read
        PP = 0x02,                                //Page Program
        SE = 0x20,                                //Sector Erase
        BE = 0xD8,                                //block erase
        CHIP_ERASE = 0xC7,            //CHIP_ERASE                //Bulk (or Chip) Erase
        DP = 0xB9,                                //Deep Power Down
        RDP = 0xAB,        //RES                //Release Deep Power Down 
        RDID = 0x90,        //RES                //release from deep power down and read signature
    };

public :

// overides
private :
    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;
public:
    CW25Xxx(CUSB& usb, const memory::memory_id& id) ;

public:
    bool doWRSR(unsigned char cSR); //  not virtual because not suppoeted  
};
