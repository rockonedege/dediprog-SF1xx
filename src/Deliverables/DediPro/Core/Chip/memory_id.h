#pragma once

#include <string>
using std::wstring;

namespace memory
{

    struct memory_id
    {
    public: //persist
        wstring TypeName;
        size_t UniqueID;
        wstring Class;
        wstring Description;

        wstring Manufacturer;
        wstring ManufactureUrl;
        wstring Voltage;
        wstring Clock;

        size_t ManufactureID;
        size_t JedecDeviceID;
        size_t AlternativeID;

        size_t ChipSizeInByte;
        size_t SectorSizeInByte;
        size_t PageSizeInByte;
        size_t BlockSizeInByte;

        size_t AddrWidth;

        bool PageEraseSupport;
        bool PageWriteSupport;
        bool PageProgramSupport;
        bool SectorEraseSupport;
        bool BlockEraseSupport;
        bool AAISingleByteProgramSupport;
        bool AAIWordProgramSupport;
        bool DeepPowerDown;
        bool DualID;
        size_t VppSupport;

    public:
        bool is_empty() const { return (-1 == UniqueID) && TypeName.empty(); }

    public:
        memory_id()
            : TypeName()
            , UniqueID(-1)
            , Class()
            , Description()

            , Manufacturer()
            , ManufactureUrl()
            , Voltage()
            , Clock()

            , ManufactureID()
            , JedecDeviceID()
            , AlternativeID()

            , ChipSizeInByte()
            , SectorSizeInByte()
            , PageSizeInByte()
            , BlockSizeInByte()

            , AddrWidth()

            , PageEraseSupport()
            , PageWriteSupport()
            , PageProgramSupport()
            , SectorEraseSupport()
            , BlockEraseSupport()
            , AAISingleByteProgramSupport()
            , AAIWordProgramSupport()
            , DeepPowerDown()
            , DualID(false)
            , VppSupport(0xFF)
        {}    
    };

}// end of namespace

