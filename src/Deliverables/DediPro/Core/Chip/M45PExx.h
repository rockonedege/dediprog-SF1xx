#pragma once 
/************************************************************************/
/* ST M45PExx Serial Flashes               
Supported Operations :
    Common Instructions :            
        WREN                
        WRDI
        RDSR
        RDID
        READ
        FAST_READ
        PP
        PW
        PE
        SE
        DP                    
        RDP                                                    

        ID table :
                        Jedec
        M45PE80        0x204014
        M45PE40        0x204013
        M45PE20        0x204012
        M45PE10        0x204011

Identification method : read Jedec ID

*/
/************************************************************************/
#include "SerialFlash.h"


class CM45PExx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ST_M45PExx; }
private:
    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN    = 0x06,                      // Write Enable
        WRDI    = 0x04,                      // Write Disable
        RDIDJ   = 0x9F,        //RDIDJ       // Read Jedec ID , except 80
        RDSR    = 0x05,                      // Read Status Register
        READ    = 0x03,                      // Byte Read
        FREAD   = 0x0B,                      // Fast Read
        PW      = 0x0A,                      // Page Write
        PP      = 0x02,                      // Page Program
        PE      = 0xDB,                      // Page Erase
        SE      = 0xD8,                      // Sector Erase
        DP      = 0xB9,                      // Deep Power Down
        RDP     = 0xAB,        //RES         // Release Deep Power Down 
    };

public :

private :

private :
    // methods 
    // bulk operations : hide from the clients , called by bulkRead/write 
    
    
    

public:
    CM45PExx(CUSB& usb, const memory::memory_id& id) ;

public:
    tribool protectBlock(tribool bProtect){ return do_nothing;} // operation not supported
    virtual bool chipErase();

};
