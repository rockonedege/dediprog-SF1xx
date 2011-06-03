#pragma once

#include "..\USB\USBDevice.h"
//#include <boost/array.hpp>
//#include <boost/assign/list_of.hpp>

#include <vector>
using std::vector;

/*!
 * \brief
 * base class for operation on EEPROMs both on the programmer and on the flash card.
 * 
 * this class provides basic USB-control-pipe compatible  read&write methods, 
 * 16 bytes per go
 * 
 * \remarks
 * Do not initiate any object of this class
 * 
 * \see
 * Derived classes CEepromOnFlashCard|CEepromOnProgrammerBoard
 */
class CEeprom
{
public:
    CEeprom(CUSB& usb, const unsigned int iAddrStart);
public:
    ~CEeprom(void);
protected:
    CUSB&           m_usb;
    unsigned int    m_iAddrStart;

    //A  Read EEPROM command is to Read the EEPROM data from the Programmer or the Flash Card.
    bool Read16Bytes(const unsigned int iOffset, vector<unsigned char>& vData) ;
    // write the data into the EEPROM on the Programmer or the Flash Card.
    bool Write16Bytes(const unsigned int iOffset, const vector<unsigned char>& vData) ;

};

/*!
 * \brief
 * class for operation on the EEPROM on the flash card.
 * 
 * this class provides all supported methods on the flash card, 
 * 
 * \remarks
 * None
 * 
 * \see
 * base classes CEeprom
 */
class CEepromOnFlashCard : public CEeprom
{
public:
//    CEepromOnFlashCard(CUSB& usb, size_t iSize = 0x80) 
//        : CEeprom(usb, /*0xA800*/0xA400) 
    CEepromOnFlashCard(CUSB& usb, size_t iSize = 0x100)    // anderson 070105 modify
        : CEeprom(usb, /*0xA800*/0xA400) 
        , m_iSize(iSize)
    {}
private:
    size_t m_iSize;
public:
    bool ResetCounter();
    bool DownloadAuthenCode(const vector<unsigned char>& vc);
    bool DownloadOperatitonInfo(const vector<unsigned char>& vc);
    bool DownloadTFIT(const vector<unsigned char>& vc);
    bool ReadAll(vector<unsigned char>& vc);
    bool ReadAuthenCode(vector<unsigned char>& vc);
    bool ReadOperatitonInfo(vector<unsigned char>& vc);
    bool ReadTFIT(vector<unsigned char>& vc);

    //========================================== + anderson 070108
    bool SetChipSize(size_t vSize);
    //========================================== - anderson 070108

private:
    //enum{
    //    OFFSET_AuthenCode = 0x00;
    //    OFFSET_Target
};

/*!
 * \brief
 * class for operation on the EEPROM on the programmer.
 * 
 * this class provides all supported methods on the flash card, 
 * 
 * \remarks
 * None
 * 
 * \see
 * base classes CEeprom
 */
class CEepromOnProgrammerBoard: public CEeprom
{
public:
    CEepromOnProgrammerBoard(CUSB& usb, size_t iSize = 50) 
        : CEeprom(usb, 0xA800) 
        , m_iSize(iSize)
    {}
private:
    size_t m_iSize;
};
