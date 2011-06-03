#pragma once 

/************************************************************************/
/* Winbond W25Pxx Serial Flashes               
    ID
                    Jedec ID        ES
        W25P80    0xEF2014    0x13
        W25P16    0xEF2015    0x14
        W25P32    0xEF2016    0x15
        W25P10                    EF    10
        W25P20                    EF    11
        W25P40                    EF    12

Identification method : read Jedec ID or ES

*/
/************************************************************************/

#include "SerialFlash.h"

class CW25Pxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_WINBOND_W25Pxx; }
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
        //SE = 0x20,                                //Sector Erase
        SE = 0xD8,                                //Sector Erase
        CHIP_ERASE = 0xC7,                                //bulk erase
        DP = 0xB9,                                //Deep Power Down
        RDP = 0xAB,        //RES                //Release Deep Power Down 
        RDID = 0x90,        //RES                //release from deep power down and read signature
    };

public :
private :

    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;
public:
    CW25Pxx(CUSB& usb, const memory::memory_id& id) ;
};
