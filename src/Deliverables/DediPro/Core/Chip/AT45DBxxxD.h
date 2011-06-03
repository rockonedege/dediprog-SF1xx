#pragma once
#include "SerialFlash.h"

class CAT45DBxxxD :
    public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ATMEL_45DBxxxD; }
    size_t GetChipSizeInBytes() const;
    size_t GetPageSizeInBytes() const;

public:
    CAT45DBxxxD(CUSB& usb, const memory::memory_id& id);
    ~CAT45DBxxxD(void){};
private:
    enum    //list of all chip-specific instruction
    {    
        READ    = 0x03,                     // Byte Read
        FREAD   = 0x0B,                     // Fast Read
        RDIDJ   = 0x9F,                     // Read Jedec ID 
        RDSR    = 0xD7,                     // Status Register Read
        SECTOR_ERASE = 0x7C,                // Sector Erase
        BLOCK_ERASE  = 0x50,                // Sector Erase

        //ESPC    = 0x3D2A7FA9                // Enable Sector Protection Command
        //DSPC    = 0x3D2A7F9A                // Disable Sector Protection Command

        //WREN    = 0x06,                     // Write Enable
        //WRDI    = 0x04,                     // Write Disable
        //RDSR    = 0x05,                     // Read Status Register
        //WRSR    = 0x01,                     // Write Status Register
        //PP      = 0x02,                     // Page Program
        //CHIP_ERASE      = 0xC7,         //CHIP_ERASE        // Bulk (or Chip) Erase
        //DP      = 0xB9,                     // Deep Power Down
        //RDP     = 0xAB,        //RES        // Release Deep Power Down 
        //RES     = 0xAB,        //RES        // RDP and read signature
    };

public:
    tribool bulkPipeProgram(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeWrite);
    bool chipErase();
private:
    // overrides
    void waitForWIP(); 
    void waitForWEL(); 
private:
    bool EnableEraseSectorProtectionRegister();
    bool EraseSectorProtectionRegister();
    bool ProgramSectorProtectionRegister(const vector<unsigned char>& vc);
    vector<unsigned char> ReadSectorProtectionRegister();
    tribool protectBlock(tribool bProtect);
private:
    unsigned char getWriteMode();
    size_t   m_pageSize;
    size_t   m_pageSizeMask;
    size_t   m_chipSize;
    size_t   m_addrSpace;

public:
    bool ValidateAddr(size_t addr);
    void PrependPageModeInfo();
    typedef enum
    {
        AT45DB011D = 0x1F22,
        AT45DB021D = 0x1F23,
        AT45DB041D = 0x1F24,
        AT45DB081D = 0x1F25,
        AT45DB161D = 0x1F26,
        AT45DB321D = 0x1F27,
        AT45DB642D = 0x1F28
    };
};
