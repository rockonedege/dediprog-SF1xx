#include "StdAfx.h"
#include "ProgBoard.h"
#include "Eeprom.h"
#include "Macro.h"

#include "project.h"

#include <boost\utility.hpp>
#include <boost/algorithm/string/trim.hpp>

//#include "..\..\Preferences.h"
#include "..\..\VerInfo.h"
#include "..\..\3rd\File\FileIO.h"

#include <fstream>

namespace programmer_board
{

	CProgBoard::CProgBoard(CProject* project)
		: m_usb(project->getUsbInstance())
		, m_context(project->get_context())
		, m_vcc(power::vccPOWEROFF)
		,m_fpathPrefix(L"")
	{
		QueryBoard();
	}

	CProgBoard::~CProgBoard(void)
	{
	}

	void CProgBoard::QueryBoard()
	{
		// read status
		if(! m_usb.is_open() )
			return ;

		// send request
		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(16) ;

		rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
		rq.Direction = VENDOR_DIRECTION_IN ;
		rq.Request = PROGINFO_REQUEST ;
		rq.Value = RFU ;
		rq.Index = RFU ;
		rq.Length = static_cast<unsigned long>(vBuffer.size()) ;

		if(! m_usb.InCtrlRequest(rq, vBuffer))
			return ;

		wchar_t ss[16] = {0};
		copy(vBuffer.begin(), vBuffer.end(), ss);

        boost::trim(m_info.board_type.assign(&ss[0],6));
		m_info.firmware_version.assign(&ss[10],5);

        if('N' == ss[5])
        {
            m_info.supported_chip_vendor = L"Numonyx";
            m_info.board_type_display = m_info.board_type.substr(0, m_info.board_type.size()-1) + L" - " + m_info.supported_chip_vendor;
        }
        else
        {
            m_info.supported_chip_vendor = L"";
            m_info.board_type_display = m_info.board_type;
        }
	}

	const board_info&  CProgBoard::GetBoardInfo() 
	{
		return m_info;
	}

	/**
	* \brief
	* Set I/O ioState and LED light colors
	* 
	* I/O bitmap: 
	*  Bit  7  6  5  4  3  2  1  0
	*  I/O  x  x  x  x  4  3  2  1    (old h/w) 
	*  I/O  x  x  x  x  4  3  1  2    (new h/w) 
	* 
	* \param ioState
	* I/O ioState
	* 
	* \param ledColors
	* LED colors
	* 
	* \returns
	* true if instruction successfully sent.
	* 
	* \remarks
	* Write remarks for SetIO here.
	* 
	* \see
	* Separate items with the '|' character.
	*/
	bool CProgBoard::SetIO(unsigned char ioState, unsigned char ledColors)
	{

		CNTRPIPE_RQ rq ;
		vector<unsigned char> vDataPack(1) ;        //1 bytes, in fact no needs

		rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
		rq.Direction = VENDOR_DIRECTION_OUT ;
		rq.Request = SET_IO ;
		rq.Value = ioState ;
		rq.Index = ledColors ;
		rq.Length = 0 ;

		if(! m_usb.OutCtrlRequest(rq, vDataPack)){
			return false ;
		}

		return true ;
	}

	bool CProgBoard::SetTargetFlash(bool SelfVccControl/* = true*/)
	{
		// send request
		CNTRPIPE_RQ rq ;
		vector<unsigned char> vInstruction(0) ;

		rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
		rq.Direction = VENDOR_DIRECTION_OUT ;
		rq.Request = SET_TARGET_FLASH;
		rq.Value = m_context.StartupMode;
		rq.Index = NO_RESULT_IN ;
		rq.Length = 0 ;

		if(! m_usb.OutCtrlRequest(rq, vInstruction) )
		{
			return false ;
		}

		return true;
	}

	bool CProgBoard::isUpdatable(std::pair<wstring,wstring>& type_version)
	{
		BoardInfo bi;
		bi.board_type = m_info.board_type;
		bi.version = m_info.firmware_version;

		CFirmwareFile fw(bi);

		m_fpathPrefix = fw.getLatestVersionPathPrefix(type_version);

		return !m_fpathPrefix.empty();

	}

	bool isFileCorrect(const wstring& sFolder)
	{
		CFileIO f;
		vector<unsigned char> v1, v2;


		f.ReadByFileExt((sFolder + L"1.bin").c_str(), v1);
		f.ReadByFileExt((sFolder + L"2.bin").c_str(), v2);

		if(v1.empty() || v2.empty()) 
			return false;
		else
			return true;
	}

	/**
	* \brief set UpdateFirmware
	* \param
	* \returns
	*/
	bool CProgBoard::UpdateFirmware()
	{
		return UpdateFirmware(m_fpathPrefix);
	}
	bool CProgBoard::UpdateFirmware(const wstring& sFolder)
	{
		bool bResult = true;
		// read status
		if(! m_usb.is_open() )
			return false;

		if( !isFileCorrect(sFolder)) return false;

		power::CAutoVccPower ap(m_usb, m_context.power.vcc);

		bResult &= SetVpp4IAP(true);
		bResult &= UnlockRASS();

		// erase & program sector 1
		bResult &= EraseST7Sectors(true);
		bResult &= ProgramSectors(sFolder, true);

		// erase & program sector 2
		bResult &= EraseST7Sectors(false);
		bResult &= ProgramSectors(sFolder, false);

		// calculate and read back checksum
		bResult &= UpdateChkSum();

		bResult &= SetVpp4IAP(false);

		// read back checksum
		bResult &= ReadMemOnST7(0xEFFE);

		return bResult;
	}

	bool CProgBoard::SetVpp4IAP(bool bOn)
	{
		// read status
		if(! m_usb.is_open() )
			return false;

		// send request
		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(1) ;

		rq.Function = URB_FUNCTION_VENDOR_OTHER ;
		rq.Direction = VENDOR_DIRECTION_IN ;
		rq.Request = bOn ? 0x0 : 0x01 ;
		rq.Value = 0 ;
		rq.Index = 0 ;
		rq.Length = 0x01 ;

		if(! m_usb.OutCtrlRequest(rq, vBuffer))
			return  false;

		Sleep(200);
		return true;
	}
	bool CProgBoard::UnlockRASS()
	{
		// read status
		if(! m_usb.is_open() )
			return false;

		// send request
		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(1) ;

		rq.Function = URB_FUNCTION_VENDOR_OTHER ;
		rq.Direction = VENDOR_DIRECTION_IN ;
		rq.Request = 0x03 ;
		rq.Value = 0 ;
		rq.Index = 0 ;
		rq.Length = 0x01 ;

		if(! m_usb.OutCtrlRequest(rq, vBuffer))
			return  false;
		return true;
	}

	/**
	* \brief Erase ST7 Sector 1 or 2
	* \param bSectr1 bool, true sector 1, false sector 2
	* \return
	*/
	bool CProgBoard::EraseST7Sectors(bool bSect1)
	{
		// read status
		if(! m_usb.is_open() )
			return false;

		// send request
		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(1) ;

		rq.Function     = URB_FUNCTION_VENDOR_OTHER ;
		rq.Direction    = VENDOR_DIRECTION_IN ;
		rq.Request  = bSect1 ? 0x04 : 0x05 ;
		rq.Value    = 0 ;
		rq.Index    = 0 ;
		rq.Length   = 0x01 ;

		if(! m_usb.OutCtrlRequest(rq, vBuffer))
			return  false;
		return true;
	}

	bool CProgBoard::ProgramSectors(const wstring& sFolder, bool bSect1)
	{
		const unsigned int iSect1StartAddr  = 0xE000;
		const unsigned int iSect2StartAddr  = 0x8000;
		const unsigned int iSect1Size       = 0x1000;
		const unsigned int iSect2Size       = 0x6000;

		vector<unsigned char> vBuffFile;
		vector<unsigned char> vBuffPage(0x100);
		vector<unsigned char>::iterator itr, itr_end;
		vBuffFile.reserve(0x6000);

		unsigned int iStartAddr = bSect1 ? iSect1StartAddr : iSect2StartAddr;

		// prog sectors
		CFileIO f;
		CNTRPIPE_RQ rq ;

		f.ReadByFileExt(bSect1 ? (sFolder + L"1.bin").c_str() : (sFolder + L"2.bin").c_str(), vBuffFile);

		if(vBuffFile.empty()) return false;

		vBuffFile.resize( bSect1 ? iSect1Size : iSect2Size);

		itr = vBuffFile.begin();
		itr_end = vBuffFile.end();

		for( ;itr != itr_end; itr = boost::next(itr, 0x100))
		{
			/// receive page
			rq.Function     = URB_FUNCTION_VENDOR_OTHER;
			rq.Direction    = VENDOR_DIRECTION_OUT;
			rq.Request      = 0x1;
			rq.Value        = 0;
			rq.Index        = 0;
			rq.Length       = 0x100;   /// plage size for ST7 Iap prog

			if(! m_usb.OutCtrlRequest(
				rq, 
				vector<unsigned char>(itr, boost::next(itr, 0x100))
				)
				)
				return false;

			/// program page
			rq.Function     = URB_FUNCTION_VENDOR_OTHER;
			rq.Direction    = VENDOR_DIRECTION_IN;
			rq.Request      = 0x8;
			rq.Value        = 0;
			rq.Index        = iStartAddr & 0xFFFF;   ///< ConvLongToInt(lngST7address Mod &H10000)
			rq.Length       = 0x1;
			iStartAddr     += 0x100;

			if(! m_usb.OutCtrlRequest(
				rq, 
				vector<unsigned char>(1)
				)
				)
				return false;

		}
		return true;
	}

	bool CProgBoard::UpdateChkSum()
	{
		// read status
		if(! m_usb.is_open() )
			return false;

		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(2) ;

		// send to calculate checksum
		rq.Function     = URB_FUNCTION_VENDOR_OTHER ;
		rq.Direction    = VENDOR_DIRECTION_IN ;
		rq.Request      = 0x9;
		rq.Value        = 0 ;
		rq.Index        = 0 ;
		rq.Length       = 2 ;

		if(! m_usb.OutCtrlRequest(rq, vBuffer))
			return  false;

		return true;
	}


	bool CProgBoard::ReadMemOnST7(unsigned int iAddr)
	{
		// read status
		if(! m_usb.is_open() )
			return false;

		CNTRPIPE_RQ rq ;
		vector<unsigned char> vBuffer(2) ;

		// read 
		rq.Function     = URB_FUNCTION_VENDOR_OTHER ;
		rq.Direction    = VENDOR_DIRECTION_IN ;
		rq.Request      = 0x7;
		rq.Value        = 0 ;
		rq.Index        = iAddr & 0xFFFF ;
		rq.Length       = 2;

		if(! m_usb.OutCtrlRequest(rq, vBuffer))
			return  false;

		return true;
	}

	bool CProgBoard::DownloadTFIT(vector<unsigned char>& vc)
	{
		CEepromOnFlashCard /*CEepromOnProgrammerBoard*/  eepromOnCard(m_usb);

		if(vc.size() > 512) vc.resize(512);         /// truncate large files to save time

		power::CAutoVccPower ap(m_usb, m_context.power.vcc);   // power ON(3.5V)
		bool bRet = eepromOnCard.DownloadTFIT(vc);
		//SetVccValue(CVppVccPower::POWEROFF);   // power OFF

		return bRet;
	}
	bool CProgBoard::ReadAllFromEEPROMOnCard(vector<unsigned char>& vc)
	{
		CEepromOnFlashCard /*CEepromOnProgrammerBoard*/  eepromOnCard(m_usb);

		power::CAutoVccPower ap(m_usb, m_context.power.vcc);   // power ON(3.5V)
		bool bRet = eepromOnCard.ReadAll(vc);
		//SetVccValue(CVppVccPower::POWEROFF);   // power OFF

		return bRet;
	}
	bool CProgBoard::ResetCntrEEPROMOnCard()
	{
		CEepromOnFlashCard   eepromOnCard(m_usb);

		power::CAutoVccPower ap(m_usb, m_context.power.vcc);   // power ON(3.5V)
		bool bRet = eepromOnCard.ResetCounter();
		//SetVccValue(CVppVccPower::POWEROFF);   // power OFF

		return bRet;

	}
	bool CProgBoard::ReadTFITFromEEPROMOnCard(vector<unsigned char>& vc)
	{
		CEepromOnFlashCard /*CEepromOnProgrammerBoard*/  eepromOnCard(m_usb);

		power::CAutoVccPower ap(m_usb, m_context.power.vcc);   // power ON(3.5V)
		bool bRet = eepromOnCard.ReadTFIT(vc);
		//SetVccValue(CVppVccPower::POWEROFF);   // power OFF

		return bRet;
	}

}
