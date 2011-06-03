
#ifndef _MACRO_H
#define _MACRO_H

// new defined macros 
//programmer info RQ
#define PROGINFO_REQUEST                0x08 
#define SET_VCC                         0x09                ///< set VCC
#define SET_VPP                         0x03                ///< set VPP

//first field of RQ
#define OUT_REQUEST                     0x42 
#define IN_REQUEST                      0xC2

//second field of RQ in case of bulk transfer
//#define TRANSCEIVE                      0x01                
//#define DTC_READ                        0x20
//#define WRITE                           0x30
//#define READ_EEPROM                     0x05
//#define WRITE_EEPROM                    0x06
// values of Request Field of a setup packet
typedef enum 
{
    TRANSCEIVE                      = 0x01,

    DTC_READ                        = 0x20,
    WRITE                           = 0x30,

    ATMEL45_WRITE                   = 0x31,

    READ_EEPROM                     = 0x05,
    WRITE_EEPROM                    = 0x06
};

//third field of RQ
#define CTRL_TIMEOUT                    3000                ///< time out for control pipe or for firmwire

// fourth field of RQ
#define RESULT_IN                       0x01                ///< result in or not
#define NO_RESULT_IN                    0x00
#define GET_REGISTER                    0x01                ///< get register value or not
#define NO_REGISTER                     0x00
#define RFU                             0xFF

// bulk write mode
#define PAGE_PROGRAM                    0x01                ///< pp via bulk pipes
#define PAGE_WRITE                      0x02                ///< pw via bulk pipes
#define AAI_1_BYTE                      0x03
#define AAI_2_BYTE                      0x04
#define PP_128BYTE                      0x05
#define PP_AT26DF041                    0x06                ///< PP AT26DF041 
#define PP_SB_FPGA                      0x07                ///<  Silicon Blue FPGA 

// bulk read mode
#define BULK_NORM_READ                  0x01                ///< read via bulk pipes
#define BULK_FAST_READ                  0x02                ///< fast read via bulk pipes

//for flash card
#define POLLING                         0x02                ///< polling
#define SET_VPP                         0x03                ///< set vpp
#define VPP_OFF                         0x00                ///< vpp value    
#define VPP_9V                          0x01
#define VPP_12V                         0x02                

#define SET_TARGET_FLASH                0x04            ///< set target FLASH_TRAY
#define APPLICATION_MEMORY_1            0x00            ///< application memory chip 1
#define FLASH_CARD                      0x01            ///< flash card
#define APPLICATION_MEMORY_2            0x02            ///< application memory chip 2

//for io & LED
#define SET_IO                          0x07                ///< request
//IO number
#define IO_1                            0x01
#define IO_2                            0x02
#define IO_3                            0x04
#define IO_4                            0x08
//led number
#define LED_RED                         0x01
#define LED_GREEN                       0x02
#define LED_ORANGE                      0x04

// for usb 
#define USB_TIMEOUT                     8000                ///< time out value for usb EP2

// for SR reads
#define MAX_TRIALS                      0x80000      


#endif    //_MACRO_H