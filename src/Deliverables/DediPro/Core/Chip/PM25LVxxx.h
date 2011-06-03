#pragma once 

/************************************************************************/
/* PMC PM25LVxxx Serial Flashes               
    ID table :
                            Jedec            ID        ES
        Pm25LV512           0x9D7B7F        
        Pm25LV010           0x9D7C7F         0x7C            VPP
        Pm25LV020           0x9D7D7F         0x7D    DP
        Pm25LV040           0x9D7E7F         0x7E    DP        VPP
*/
/************************************************************************/

#include "SerialFlash.h"

class CPM25LVxxx : public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_PMC_PM25LVxxx; }
private:

    enum    //list of all chip-specific instruction, for ST serial flash
    {    
        WREN    = 0x06,                     // Write Enable
        WRDI    = 0x04,                     // Write Disable
        RDSR    = 0x05,                     // Read Status Register
        WRSR    = 0x01,                     // Write Status Register
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read 25MHz for(2.7-3.6V), 33MHz(3.3-3.6)
        RDID    = 0xAB,                     // Read Manufacturer and Product ID
        READJEDEC=0x9F,         // Pm25LV512 not suppported         
        PP      = 0x02,                     // Page Program
        RDCR    = 0xA1,         // Pm25LV512 not suppported      
        WRCR    = 0xF1,         // Pm25LV512 not suppported      
        SE      = 0xD7,                     // Sector Erase
        BE      = 0xD8,         //BE        // Block Erase
        CHIP_ERASE      = 0xC7,         //CHIP_ERASE        // Chip Erase
    };

private :
    // protect block for erase/program or not 
    tribool protectBlock(tribool bProtect) ;
public:
    CPM25LVxxx(CUSB& usb, const memory::memory_id& id) ;
};
