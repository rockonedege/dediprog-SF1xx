#pragma once
#include "SerialFlash.h"

class CAT45DBxxxB :
    public CSerialFlash
{
public:
    static wstring getClassName(){ return SUPPORT_ATMEL_45DBxxxB; }
    size_t GetChipSizeInBytes() const;
    size_t GetPageSizeInBytes() const;

public:
    CAT45DBxxxB(CUSB& usb, const memory::memory_id& id);
    ~CAT45DBxxxB(void){};
private:
    enum    //list of all chip-specific instruction
    {    
        READ    = 0xE8,                     // Byte Read
 
        RDSR    = 0xD7,                     // Status Register Read
        SECTOR_ERASE = 0x50,                // Sector Erase
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
    //typedef enum
    //{
    //    AT45DB011D = 0x1F22,
    //    AT45DB021D = 0x1F23,
    //    AT45DB041D = 0x1F24,
    //    AT45DB081D = 0x1F25,
    //    AT45DB161D = 0x1F26,
    //    AT45DB321D = 0x1F27,
    //    AT45DB642D = 0x1F28
    //};
};
