/************************************************************************/
/* ST M25PExx Serial Flashes               
    Supported Operations :
        Common Instructions :            
        WREN                
        WRDI
        RDSR
        RDLR                    only for M25PE80
        WRLR                    only for M25PE80
        RDID
        READ
        F_READ
        PP
        PW
        PE
        SE
        BE                        only for M25PE80
        DP                    
        RDP                                
        RES                        

    ID table :
                        Jedec    ID    
        M25PE80         0x208014
        M25PE40         0x208013
        M25PE20         0x208012
        M25PE10         0x208011

    Identification method : read Jedec ID

*/
/************************************************************************/
#pragma once 

#include "SerialFlash.h"


class CM25PExx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ST_M25PExx; }
private:


    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN    = 0x06,                           // Write Enable
        WRDI    = 0x04,                           // Write Disable
        RDIDJ   = 0x9F,        //RDIDJ            // Read Jedec ID , except 80
        RDSR    = 0x05,                           // Read Status Register
        WRSR    = 0x01,                     // Write Status Register
        READ    = 0x03,                           // Byte Read
        FREAD   = 0x0B,                           // Fast Read
        PW      = 0x0A,                           // Page Write
        PP      = 0x02,                           // Page Program
        PE      = 0xDB,                           // Page Erase
        SE      = 0xD8,                           // Sector Erase
        CHIP_ERASE      = 0xC7,           //CHIP_ERASE            // Bulk (or Chip) Erase
        DP      = 0xB9,                           // Deep Power Down
        RDP     = 0xAB,           //RES           // Release Deep Power Down 
        WRLR    = 0xE5,                           // Write Lock Register
        RDLR    = 0xE8,                           // Read Lock Register
    };

private :

private :
    // methods 
    // bulk operations : hide from the clients , called by bulkRead/write 
    
    
    

public:
    CM25PExx(CUSB& usb, const memory::memory_id& id) ;

public:
   
    bool DoWRLR(const vector<unsigned char>& vLR) ;
    bool DoRDLR(vector<unsigned char>& vLR) ;
private:
    tribool protectBlock(tribool bProtect){ return do_nothing;} // operation not supported
};
