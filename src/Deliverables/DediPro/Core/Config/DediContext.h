#pragma once

#include <string>
#include "VppVccPower.h"

namespace Context 
{
    using std::wstring;

    typedef enum 
    {   // value dedicated by the spec
        STARTUP_APPLI_SF_1 = 0,
        STARTUP_APPLI_CARD = 1,
        STARTUP_APPLI_SF_2 = 2,

        STARTUP_SPECIFY_LATER = 0xFE,
        STARTUP_PREVIOUS = 0xFF
    } STARTUP_MODE;

    typedef enum
    {
        InvokeFromProg = 1,
        InvokeFromBatch = 2,
        InvokeFromCard = 3,
    } INVOKE_FROM;


    struct CRuntimeContext
    {
        bool  is_feature_locked;
        bool  is_operation_on_going;
        bool  is_operation_successful;
        double elapsed_time_of_last_operation;

        unsigned char padding_char;

        std::wstring current_status;

        INVOKE_FROM InvokeFrom;

    public:
        CRuntimeContext()
            : is_feature_locked(false)
            , is_operation_on_going(false)
            , is_operation_successful(true)

            , elapsed_time_of_last_operation(0)

            , current_status(L"idle.")

            , InvokeFrom()

            , padding_char(0xFF)
        {}
    };

    struct CPersistentContext
    {

    };

    struct CFileContext
    {
        size_t file_buffer_crc;
        size_t file_size;

        wstring CurrentFileInBuffer;
        wstring CurrentFileInBufferFormat;
        wstring LastLoadedFile;
        wstring LastLoadedFileFormat;

    public:
        CFileContext()
            :file_buffer_crc()
            ,file_size()

            ,CurrentFileInBuffer()
            ,CurrentFileInBufferFormat()
            ,LastLoadedFile()
            ,LastLoadedFileFormat()        
        {}
    };

    struct CChipContext
    {
        bool is_S25FL128P_256KSectors;
		size_t mcf_config_register;

    public:
        CChipContext()
            :is_S25FL128P_256KSectors(false)
			,mcf_config_register(0x3F)
        {}

    };

    struct CPowerContext
    {
        power::VCC_VALUE vcc;
        power::VPP_VALUE vpp;

        bool Misc_ApplyVpp;
    public:
        CPowerContext()
            :vcc(power::vccPOWEROFF)
            ,vpp(power::vppdo_nothing)
            ,Misc_ApplyVpp(false)
        {}

    };

    struct CFlashCardContext
    {
        size_t FlashCard_Option;
        size_t FlashCard_OptionIndex;
        size_t FlashCard_TargetSizeInBytes;
        wstring FlashCard_TargetType;
        wstring FlashCard_TFITFileForV116Below;
        wstring FlashCard_TFITFile;

        size_t FlashCard_ComboSelection;
        bool FlashCard_UsingVpp;

    public:
        CFlashCardContext()
            :FlashCard_Option()
            ,FlashCard_TFITFileForV116Below()
            ,FlashCard_TFITFile()
            ,FlashCard_OptionIndex()
            ,FlashCard_UsingVpp()
            ,FlashCard_TargetSizeInBytes()
            ,FlashCard_TargetType()
            ,FlashCard_ComboSelection()
        {}
    };

    struct CDediContext
    {
        static const unsigned char NO_PADDING = 0xFF;

        wstring id;

        STARTUP_MODE StartupMode;

        unsigned char Prog_PaddingCharacterWhenProgrammingWholeChip;
        size_t Prog_CustomizedAddrFrom;
        size_t Prog_CustomizedDataLength;
        bool Prog_PartialProgram;

        size_t Batch_SelectedOption;
        bool Batch_VerifyAfterCompletion;
        bool Batch_ReloadFile;
        bool Batch_PartialProgram;
        unsigned char Batch_PaddingCharacterWhenDownloadWithBlankCheck;
        unsigned char Batch_PaddingCharacterWhenDownloadWithNoBlankCheck;
        size_t Batch_CustomizedAddrFrom;
        size_t Batch_CustomizedDataLength;
        size_t Read_CustomizedAddrFrom;
        size_t Read_CustomizedDataLength;


        wstring SavedPaths;

    public:
        CFileContext file;
        CPowerContext power;
        CRuntimeContext runtime;
        CChipContext chip;
        CFlashCardContext flashcard;
    public:     //runtime

        CDediContext()
            :StartupMode(STARTUP_APPLI_SF_1)

            ,Prog_PaddingCharacterWhenProgrammingWholeChip(0x55)
            ,Prog_PartialProgram()
            ,Prog_CustomizedAddrFrom()
            ,Prog_CustomizedDataLength()

            ,Batch_SelectedOption()
            ,Batch_PartialProgram()
            ,Batch_VerifyAfterCompletion()
            ,Batch_ReloadFile()
            ,Batch_PaddingCharacterWhenDownloadWithBlankCheck()
            ,Batch_PaddingCharacterWhenDownloadWithNoBlankCheck()
            ,Batch_CustomizedAddrFrom()
            ,Batch_CustomizedDataLength()

            ,Read_CustomizedAddrFrom()
            ,Read_CustomizedDataLength()

            ,SavedPaths()

            ,file()
            ,power()
            ,runtime()
            ,flashcard()
        {}
    };

}
