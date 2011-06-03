#pragma once

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>
#include "FirmwareFile.h"
#include "VppVccPower.h"

using std::vector;

class CUSB;
class CProject;

namespace Context { struct CDediContext;}

namespace programmer_board
{
    using std::wstring;
    struct board_info
    {
        //wstring     vcc;
        //wstring     vpp;
        wstring     board_type;
        wstring     firmware_version;
		wstring	    supported_chip_vendor;
        wstring     board_type_display;
    };

    /**
    * \brief  operations on the programmer board
    *
    *
    */
    class CProgBoard:boost::noncopyable
    {
    public:
        CProgBoard(CProject* project);
        ~CProgBoard(void);

    public:
        bool DownloadTFIT(vector<unsigned char>& vc);
        bool ReadAllFromEEPROMOnCard(vector<unsigned char>& vc);
        bool ResetCntrEEPROMOnCard();
        bool ReadTFITFromEEPROMOnCard(vector<unsigned char>& vc);


        //bool SetTargetFlash(TARGET_MODE mode, bool SelfVccControl = true);
        bool SetTargetFlash(bool SelfVccControl = true);
        bool SetIO(unsigned char ioState, unsigned char ledColors);


        bool isUpdatable(std::pair<wstring,wstring>& type_version);
        bool UpdateFirmware();
        bool UpdateFirmware(const wstring& sFolder);

        const board_info& GetBoardInfo();
    private:
        CUSB&           m_usb;
        wstring         m_fpathPrefix;
        board_info      m_info;

        power::VCC_VALUE m_vcc;

    private:
        wstring latestVersion(const boost::filesystem::path& p);
        void QueryBoard();
    private:
        /**
        * \brief Erase ST7 Sector 1 or 2
        * \param bSectr1 bool, true sector 1, false sector 2
        * \return
        */
        bool SetVpp4IAP(bool bOn);
        bool UnlockRASS();
        bool EraseST7Sectors(bool bSect1);
        bool ProgramSectors(const wstring& sFolder, bool bSect1);
        //bool ProgramSectors_2(const wstring& sFolder, bool bSect1, vector<unsigned char> & vBuffer2);
        bool UpdateChkSum();
        bool ReadMemOnST7(unsigned int iAddr);
    private:
        Context::CDediContext& m_context;
    };

} //namespace programmer_board
