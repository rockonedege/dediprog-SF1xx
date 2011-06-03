/*! \mainpage DediProg Pro Source Code Manual
 *
 *  \version 3.0.xx
 *  \date    2005 - 2007
 *  \section intro_sec Introduction
 *
 *  This is the documentation file for dedipro.
 *  - This project heavily relies on Boost Library(www.boost.org). 
 *    You need to download and compile it and config your environment properly.
 *
 * - the current project configurations are based on teh following directory architecture <pre>
 *   Src.
 *
 * </pre>You need to change some configurations if the directory architecture differs.
 *  
 */


#ifndef _CHIP_H
#define _CHIP_H

#include "..\..\stdafx.h"

#include "Macro.h"
#include "ProgBoard.h"
#include "SerialFlashInfoEx.h"
#include "FlashCommand.h"
//#include "..\..\3rd\Markup\Markup.h"

#include "memory_id.h"

#include <utility>
#include <vector>

#include <boost/tuple/tuple.hpp>

#include <boost/logic/tribool.hpp>
BOOST_TRIBOOL_THIRD_STATE(do_nothing)
using boost::logic::tribool;
using boost::logic::indeterminate;

using std::pair ;
using std::vector ;

class CUSB;


/**
 * \brief
 * Base class for all families of serial flash memories.
 * 
 * any serial flash must derive from this class.
 * This class declares a few common interfaces 
 * and also provides a few default implementations which can be overriden.
 * 
 * \remarks
 * Write remarks for CSerialFlash here.
 * 
 * \see
 * Separate items with the '|' character.
 */
class CSerialFlash:boost::noncopyable
{
public:
    CSerialFlash(CUSB& usb, const memory::memory_id& uniqueId) :
        m_usb(usb),
        m_info(),
        m_uniqueId(uniqueId),
        m_cmd(usb),
        m_isCanceled(false),
        m_bProtectAfterWritenErase(do_nothing),
        m_codeRDSR(0x05),
        m_codeWRSR(0x01), 
        m_codeChipErase (0),
        m_codeProgram (0),
        m_codeRead (0),
        m_codeWREN(0x06),
        m_codeWRDI(0x04),
        m_code64KErase(0)
        {

        }

    // virtual deconstructor
    virtual ~CSerialFlash()=0 {} 
public:
    bool is_good();

    bool Is64KEraseSupported()                           const  { return (0 != m_code64KErase);}
    bool IsProtectedAfterWritenErase()                   const  { return m_bProtectAfterWritenErase;}

    void SetBProtectAfterWritenErase(bool bProtect)             { m_bProtectAfterWritenErase = bProtect;}
    void SetCancelOperationFlag();
    void ClearCancelOperationFlag();
protected :
    bool is_protectbits_set();
    
public:
    bool readSR(unsigned char& cSR);
    bool writeSR(unsigned char cSR);
    bool rangeRead      (const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData);
    bool rangeProgram   (const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData);
    bool rangeBlankCheck(pair<unsigned int, unsigned int> Range);
    tribool batchErase(const vector<size_t>& vAddrs);

    // virtual publics
    virtual bool chipErase();


protected :
    bool         DoPolling() ;  
    bool         doRDSR(unsigned char& cSR);

    tribool rangeErase(unsigned char cmd, size_t sectionSize, const pair<size_t, size_t>& AddrRange);
    tribool bulkPipeRead(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeRead);

    // virtuals
    virtual bool doWREN();
    virtual bool doWRDI();
    virtual bool doWRSR(unsigned char cSR); 
    virtual void waitForWEL();
    virtual void waitForWIP();  

    virtual tribool protectBlock(tribool);
    virtual tribool bulkPipeProgram(const pair<size_t, size_t>& AddrRange, vector<unsigned char>& vData, unsigned char modeWrite);

protected:
    CUSB&           m_usb;  ///< one and only CUSB instance
    ChipInfo::CChipInfo       m_info;
    CFlashCommand   m_cmd;
    tribool         m_bProtectAfterWritenErase;
    bool            m_isCanceled;
protected:    
    const memory::memory_id&    m_uniqueId;
    unsigned char   m_codeRDSR;     ///< RDSR code
    unsigned char   m_codeWRSR;     ///< WRSR code
    unsigned char   m_codeChipErase;
    unsigned char   m_codeProgram;
    unsigned char   m_codeRead;
    unsigned char   m_codeWREN;
    unsigned char   m_codeWRDI;
    unsigned char   m_code64KErase;
};

#endif    //_CHIP_H