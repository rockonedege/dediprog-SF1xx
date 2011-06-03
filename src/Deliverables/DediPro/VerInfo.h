#pragma once

/// VERSION INFO aa
//#define VERSION_MAJOR               L"3"
//#define VERSION_MINOR               L"0"
//#define VERSION_BUILD               L"64"
//#define VERSION_SPECIAL             L"Alpha"

/// select supported modes
//#define WORKING_WITH_APPLICATION_MEMORIES_ONLY
#define WORKING_WITH_CARD_AND_MEMORIES
//#define WORKING_WITH_FLASH_CARD_ONLY

/// Misc. features
#define FWUPDATE_ENABLED

#define SUPPORT_ST
#define SUPPORT_SST
#define SUPPORT_WINBOND
#define SUPPORT_PMC
#define SUPPORT_SPANSION
#define SUPPORT_MACRONIX
#define SUPPORT_EON
#define SUPPORT_ATMEL
#define SUPPORT_AMIC
#define SUPPORT_ESMT
#define SUPPORT_INTEL
#define SUPPORT_SANYO
#define SUPPORT_TSI       
#define SUPPORT_FREESCALE

// memory support list
#ifdef SUPPORT_ST
    #define SUPPORT_ST_M25PExx          L"M25PExx"
    #define SUPPORT_ST_M25Pxx           L"M25Pxx"
    #define SUPPORT_ST_M45PExx          L"M45PExx"
#endif

#ifdef SUPPORT_SST
    #define SUPPORT_SST_25xFxxA         L"25xFxxA"
    #define SUPPORT_SST_25xFxxB         L"25xFxxB"
    #define SUPPORT_SST_25xFxx          L"25xFxx"
#endif

#ifdef SUPPORT_WINBOND
    #define SUPPORT_WINBOND_W25Bxx      L"W25Bxx"
    #define SUPPORT_WINBOND_W25Pxx      L"W25Pxx"
    #define SUPPORT_WINBOND_W25Xxx      L"W25Xxx"
#endif

#ifdef SUPPORT_PMC
    #define SUPPORT_PMC_PM25LVxxx       L"PM25LVxxx"
#endif

#ifdef SUPPORT_SPANSION
    #define SUPPORT_SPANSION_S25FLxx    L"S25FLxx"
#endif

#ifdef SUPPORT_MACRONIX
    #define SUPPORT_MACRONIX_MX25Lxxx  L"MX25Lxxx"
#endif

#ifdef SUPPORT_EON
    #define SUPPORT_EON_EN25Xxx          L"EN25Xxx"
#endif

#ifdef SUPPORT_ATMEL
    #define SUPPORT_ATMEL_AT26xxx       L"AT26xxx"
    #define SUPPORT_ATMEL_AT25Fxxx      L"AT25Fxxx"
    #define SUPPORT_ATMEL_AT25FSxxx     L"AT25FSxxx"
    #define SUPPORT_ATMEL_45DBxxxD      L"AT45DBxxxD"
    #define SUPPORT_ATMEL_45DBxxxB      L"AT45DBxxxB"
#endif

#ifdef SUPPORT_AMIC
    #define SUPPORT_AMIC_A25Lxxx        L"A25Lxxx"
#endif

#ifdef SUPPORT_ESMT
    #define SUPPORT_ESMT_F25Lxx         L"F25Lxx"
#endif

#ifdef SUPPORT_INTEL
    #define SUPPORT_INTEL_S33           L"S33"  
#endif

#ifdef SUPPORT_FREESCALE
    #define SUPPORT_FREESCALE_MCF           L"MCF"  
#endif

#ifdef SUPPORT_SANYO
    #define SUPPORT_SANYO_LE25FWxxx     L"LE25FWxxx"
#endif

#ifdef SUPPORT_TSI       
    #define SUPPORT_TSI_TS25Lxx         L"TS25Lxx"
#endif



/// --- do not edit below ---
#define MODE_NAME_FLASH_CARD            L"Flash Card"
#define MODE_NAME_APPLICATION_MEMORY_1  L"Application Memory Chip 1"
#define MODE_NAME_APPLICATION_MEMORY_2  L"Application Memory Chip 2"

#ifdef  WORKING_WITH_APPLICATION_MEMORIES_ONLY
    #define INCLUDE_WORKING_WITH_APPLICATION_MEMORIES
    #define DEFAULT_WORKING_WITH_APPLICATION_MEMORIES
    #define VERSION_VERBOSE         L"Lite.ApplicationMemory";
#endif

#ifdef  WORKING_WITH_FLASH_CARD_ONLY
    #define INCLUDE_WORKING_WITH_FLASH_CARD
    #define DEFAULT_WORKING_WITH_FLASH_CARD
    #define VERSION_VERBOSE         L"Lite.ReferenceCard";
#endif

#ifdef  WORKING_WITH_CARD_AND_MEMORIES
    #define INCLUDE_WORKING_WITH_APPLICATION_MEMORIES
    #define INCLUDE_WORKING_WITH_FLASH_CARD
    #define VERSION_VERBOSE         L"Deluxe";

    #define DEFAULT_WORKING_WITH_APPLICATION_MEMORIES
    #ifdef DEFAULT_WORKING_WITH_APPLICATION_MEMORIES
        #define DEFAULT_MODE "\r\nStarup Mode : Application Memory"
    #endif

    #ifdef DEFAULT_WORKING_WITH_FLASH_CARD
        #define DEFAULT_MODE "\r\nStarup Mode : Reference Card"
    #endif
#endif
