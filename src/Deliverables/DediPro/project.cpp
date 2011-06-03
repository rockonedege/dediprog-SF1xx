
#include "stdafx.h"

#include ".\Core\Chip\25xFxx.h"
#include ".\Core\Chip\25xFxxA.h"
#include ".\Core\Chip\25xFxxB.h"
#include ".\Core\Chip\M25Pxx.h"
#include ".\Core\Chip\M25PExx.h"
#include ".\Core\Chip\M45PExx.h"
#include ".\Core\Chip\W25Xxx.h"
#include ".\Core\Chip\W25Pxx.h"
#include ".\Core\Chip\W25Bxx.h"
#include ".\Core\Chip\PM25LVxxx.h"
#include ".\Core\Chip\S25FLxxx.h"
#include ".\Core\Chip\MX25Lxxx.h"
#include ".\Core\Chip\EN25Xxx.h"
#include ".\Core\Chip\AT26xxx.h"
#include ".\Core\Chip\AT25Fxxx.h"
#include ".\Core\Chip\A25Lxxx.h"
#include ".\Core\Chip\F25Lxx.h"
#include ".\Core\Chip\Eeprom.h"
#include ".\Core\Chip\S33x.h"
#include ".\Core\Chip\AT45DBxxxD.h"
#include ".\Core\Chip\AT45DBxxxB.h"
#include ".\Core\Chip\AT25FSxxx.h"
#include ".\Core\Chip\LE25FWxxx.h"
#include ".\Core\Chip\TS25Lxx.h"
#include ".\Core\Chip\MCF.h"


///
#include "FlashIdentifier.h"
#include "Project.h"
#include "PreferenceEx.h"
#include ".\3rd\File\FileIO.h"
#include ".\3rd\Logger\Logger.h"
#include ".\Core\Utils\AddrRoundup.h"
#include "versioninfo.h"
//#include ".\3rd\VisualLeakDetector\include\vld.h"


/// boost library includes

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>

///std library includes

#include <sstream>
#include <iomanip>
#include <algorithm>

//using boost::thread ;
using boost::bind ;
using boost::assign::list_of;
using std::copy;
using std::advance;

using std::wstringstream;
using std::vector ;
using std::pair ;
using std::make_pair;
using std::generate;

using namespace boost::filesystem;

/**
* \brief
* USB GUID, product ID, vendor ID.
* 
* Write detailed description for Dediprogrammer_GUID here.
* 
* \remarks
* Write remarks for Dediprogrammer_GUID here.
*    Device Interface Name 	        GUID
*    USB Raw Device 	            {a5dcbf10-6530-11d2-901f-00c04fb951ed}
*    Disk Device 	                {53f56307-b6bf-11d0-94f2-00a0c91efb8b}
*    Network Card 	                {ad498944-762f-11d0-8dcb-00c04fc3358c}
*    Human Interface Device (HID) 	{4d1e55b2-f16f-11cf-88cb-001111000030}
*    Palm 	                        {784126bf-4190-11d4-b5c2-00c04f687a67}
* 
* \see
* Separate items with the '|' character.
*/

const USB_INFO Dediprogrammer_GUID[] = 
{
    {
        { 0x73BFF151, 0xCB17, 0x4D4D, { 0x83, 0xD7, 0x46, 0xF6, 0x87, 0x6B, 0xED, 0xAE} },
            0xDADA,
            0x483
    }
} ;


// device type , to select the configuration of usb
enum    USB_DEVICE {
    SF100_200 
} ;


bool FillVectorWithFile(const wstring& path, CFileIO::FILE_FORMAT format, vector<unsigned char>& vc)
{
    CFileIO file ;
    bool result;

    vc.clear();
    switch(format)
    {
    case CFileIO::BIN:
        result = file.ReadBINFile(path, vc);
        break;
    case CFileIO::S19:
        result = file.ReadS19File(path, vc);
        break;
    case CFileIO::HEX:
        result = file.ReadHEXFile(path, vc);
        break;
    default:
        result = file.ReadBINFile(path, vc);
    }

    return result;
}



/**
* \brief
* constructor via configuration file
* 
*
* \throws <exception class>
* Description of criteria for throwing this exception.
* 
* A project is an abstract a series of operation task, that is, each operation is 
* associated to a project instance, a default CProject instance is constructed once 
* the application starts up.  
* 
* \remarks
* Write remarks for CProject here.
* 
* \see
* Separate items with the '|' character.
*/
CProject::CProject(USER_GROUP user_group, Context::STARTUP_MODE target_flash):
m_usb(Dediprogrammer_GUID[SF100_200]) 
, m_board(0)
, m_chip(0)
, m_imageFileInfo(make_pair<wstring, CFileIO::FILE_FORMAT>(L"", CFileIO::BIN))
, m_allChipIdentifyingTrials(L"")
, m_user_group(user_group)
, m_context(PreferenceEx::CPreferenceReader().get_context_set()[user_group])
{
    m_thread.reset(0);

    // pre-flash id preparations

    // init board and target flash
    if(!(Context::STARTUP_PREVIOUS == target_flash 
        || Context::STARTUP_SPECIFY_LATER == target_flash)) 
        m_context.StartupMode = target_flash;

    m_board.reset(new programmer_board::CProgBoard(this));
    m_board->SetTargetFlash();

    // init power
    //m_context.power.vcc = power::vcc3_5V;

}

CProject::~CProject()
{
    PreferenceEx::CPreferenceWriter().persist_contexts(m_context);
    m_chip.reset(0L) ;
}

/**
* \brief
*  Create the one and only CProject instance.
* 
* \param customID
* a unique ID to identify the chip type. 
* if 0, an auto detected unique chip ID by reading electronic signatures will be used while initialising 
* if otherwise, the passed in unique ID(\c customID) will be used while initialise  
* 
* \returns
* true, if 1) a chip instance constructed(i.e a chip identified(auto) or designated(customer)), 
*          2) usb status is OK thus a project instance is sonctructed 
* false, otherwise
* 
* 
* \remarks
* a non-zero \c customID is assigned typically when it's the chip cannot be identified by reading its elctronic signature
* 
* \see
* CProject::CreateChipInstance(unsigned int customID).
*/
void CProject::ResetChipId()
{
    memory::memory_id id;
    m_chip_id = id;//make_pair(0, make_pair(L"", L""));
}

memory::memory_id CProject::GetFirstDetectionMatch()
{
    memory::memory_id id;// = make_pair(0, make_pair(L"", L""));
    if(m_usb.is_open(/*true*/)) 
    {
        CFlashIdentifier fid(this);

        if(!fid.get_default_match(id))
        {
            m_allChipIdentifyingTrials = fid.get_formated_trials();
        }

        m_AllMatchesInString = fid.get_formated_matches();

    }
    return id;
}

// fail in case of 1) USB failure, 2) unrecognised ID.
bool CProject::Init()  // by auto read ID
{
    return Init(GetFirstDetectionMatch());
}

bool CProject::Init(const memory::memory_id& customId) // by designated ID
{
    if(! m_usb.is_open(/*true*/)) 
    {
        m_allChipIdentifyingTrials = L"USB not working";
        return false ;
    }

    ResetLock();

    m_chip_id = customId;

    m_downAddrRange = make_pair( 0, m_chip_id.ChipSizeInByte );

    m_context.power.vpp = power::helper::convert_vpp_to_enum(m_chip_id.VppSupport);

    if( CreateChipInstance())
    {
        wstring supported_chip_vendor(this->getBoardInfo().supported_chip_vendor);
        if(! (
                boost::iequals(boost::trim_copy(m_chip_id.Manufacturer), boost::trim_copy(supported_chip_vendor)) 
                || 
                supported_chip_vendor.empty()
            )
           )
        {
            wstringstream wss;
            wss << L"The chip (" << m_chip_id.TypeName << L") is not supported in this version.\n"
                << L"This is a limited version only supporting " <<  supported_chip_vendor << L" chips\n"
                << L"Please contact Dediprog Inc. for more info.";

            Log(wss.str());

            m_context.runtime.is_feature_locked = true;
        }
        return true;
    }
    else
    {
        return false;
    };
}

bool CProject::CreateChipInstance()
{   
    power::CAutoVccPower autopower(m_usb,m_context.power.vcc);

    if(tryChipType<CAT25FSxxx>()) return true;
    if(tryChipType<CAT25Fxxx>()) return true;
    if(tryChipType<C25xFxx>()) return true;
    if(tryChipType<C25xFxxA>()) return true;
    if(tryChipType<C25xFxxB>()) return true;
    if(tryChipType<CA25Lxxx>()) return true;
    if(tryChipType<CAT26xxx>()) return true;
    if(tryChipType<CEN25Xxx>()) return true;
    if(tryChipType<CF25Lxx>()) return true;
    if(tryChipType<CM25PExx>()) return true;
    if(tryChipType<CM25Pxx>()) return true;
    if(tryChipType<CM45PExx>()) return true;
    if(tryChipType<CMX25Lxxx>()) return true;
    if(tryChipType<CPM25LVxxx>()) return true;
    if(tryChipType<CS25FLxxx>()) 
    {   
        CFlashIdentifier fi(this);

        m_context.chip.is_S25FL128P_256KSectors = fi.is_S25FL128P_256KSectors();
        return true;
    }
    if(tryChipType<CxxxS33x>()) return true;
    if(tryChipType<CW25Bxx>()) return true;
    if(tryChipType<CW25Pxx>()) return true;
    if(tryChipType<CW25Xxx>()) return true;
    if(tryChipType<CLE25FWxxx>()) return true;
    if(tryChipType<CAT45DBxxxD>())
    {
        m_chip_id.ChipSizeInByte =  ((CAT45DBxxxD*)m_chip.get())->GetChipSizeInBytes(); 
        ((CAT45DBxxxD*)m_chip.get())->PrependPageModeInfo();

        return true;
    }
    if(tryChipType<CAT45DBxxxB>()) 
    {
        m_chip_id.ChipSizeInByte =  ((CAT45DBxxxB*)m_chip.get())->GetChipSizeInBytes(); 
        ((CAT45DBxxxB*)m_chip.get())->PrependPageModeInfo();

        return true;
    }
    if(tryChipType<CTS25Lxxx>()) return true;

    if(tryChipType<CMCF>()) 
    {
        wstringstream wss;
		wss << L"Warning: the config register needs to be set properly before any program/erase.\n"
			<< L"currently, CR = 0x" << hex << m_context.chip.mcf_config_register << L"\n"
			<< L"Go to Config -> Freescale to change it if necessary.";

        return true;
    }

	m_chip.reset(0);
    return false;

}

CFileIO::FILE_FORMAT ConvertFileFormatNotation(const wstring& s)
{
    CFileIO::FILE_FORMAT f;

    if(L"IntelHex" == s)
    {
        f = CFileIO::HEX;
    }
    else if(L"MotorolaS19" == s)
    {
        f = CFileIO::S19;
    }
    else if(L"RawBin" == s)
    {
        f = CFileIO::BIN;
    }
    else
    {
        f = CFileIO::BIN;
    }

    return f;
}

bool CProject::ReloadLastFileFromPreferenceProfile()
{
    wstring      bkpCRC = getDataCrcString(true);
    size_t       bkpSize= m_vBufferforLoadedFile.size();

    if(!LoadFileWithInfo(m_imageFileInfo))  return false;

    wstringstream wss;
    wss << L"Re-Loaded file: " <<  m_imageFileInfo.first.substr(m_imageFileInfo.first.rfind(L'\\') + 1) << L"\n";

    if(bkpSize != m_vBufferforLoadedFile.size()) 
        wss << L"Warning: File size Changed from " <<  bkpSize << L" bytes"
        << L" to "
        << m_vBufferforLoadedFile.size() << L" bytes"
        << L"\n";
    if(bkpCRC != getDataCrcString(true))  
        wss << L"Warning: File checksum Changed from " <<  bkpCRC
        << L" to "
        << getDataCrcString(true) << L"\n";
    Log(wss.str());
    // Logger::LogIt(m_context.runtime.current_status);

    return true;
}

bool CProject::LoadFileWithInfo(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi)
{
    wstring path(fi.first);
    Log(path + L" being Loaded.");
    m_imageFileInfo = fi;

    if(path.empty()) 
    {
        Log(L"Error: File name missing, loading file aborted.");
        // Logger::LogIt(m_context.runtime.current_status);
        return false;
    }

    if(FillVectorWithFile(path, fi.second, m_vBufferforLoadedFile)) 
    {
        m_context.file.file_buffer_crc = crc::CRC32(m_vBufferforLoadedFile);
        m_context.file.file_size = m_vBufferforLoadedFile.size();

        m_context.file.CurrentFileInBuffer = path;
        m_context.file.CurrentFileInBufferFormat = m_context.file.LastLoadedFileFormat;
        m_context.Batch_CustomizedDataLength = m_vBufferforLoadedFile.size();
        return true;
    }
    else
    {
        Log(L"Error: Failed to open file: " + path);
        return false;
    }
}

bool CProject::LoadFileByExtension(const wstring& path)
{
    CFileIO file ;
    return LoadFileWithInfo(std::make_pair(path, file.GetFileFormatFromExt( path.c_str())));
}

void CProject::threadLoadFileWithInfo(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi)
{
    m_context.runtime.is_operation_successful = true;
    m_context.runtime.is_operation_on_going = true;

    {
        boost::timer t;

        if(LoadFileWithInfo(fi))
        {
            m_context.runtime.is_operation_successful = true;
            Log(fi.first + L" Loaded."); 
            // Logger::LogIt(m_context.runtime.current_status);
        }
        else
        {
            m_context.runtime.is_operation_successful = false;
        }

        m_context.runtime.elapsed_time_of_last_operation = t.elapsed();
    }

    wstringstream ss;
    ss << L"Operation completed. \n" << m_context.runtime.elapsed_time_of_last_operation << L" seconds elapsed.";
    Log(ss.str());

    m_context.runtime.is_operation_on_going = false;

    return;
}

void CProject::ThreadLoadFileByExtension(const wstring& path)
{
    CFileIO file ;
    threadLoadFileWithInfo(std::make_pair(path, file.GetFileFormatFromExt( path.c_str())));
}

void CProject::LoadFile(const wstring& path)
{
    m_thread.reset(new boost::thread(boost::bind(&CProject::ThreadLoadFileByExtension, this, path)));
}

void CProject::LoadFile(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi)
{
    m_thread.reset(new boost::thread(boost::bind(&CProject::threadLoadFileWithInfo, this, fi)));
}

// dowload to a whole file to chip starting from addr 0
// - erase whole chip
// - program file
bool CProject::DownloadFileUsingChipErase(bool bCheckBlank)
{ 

    // if blankcheck chosen
    if(bCheckBlank)
    {
        if(!threadBlankCheck())
        {
            if(!threadEraseWholeChip()) return false;
        }
    }
    else
    {
        if(!threadEraseWholeChip())   return false;
    }

    // need to re-set power, for large-sized chips(e.g 128Mbit), 
    // there may cause the first page not to be programed without this power reset.
    RefreshVccValue();

    m_downAddrRange = make_pair(0, m_vBufferforLoadedFile.size());

    Log(L"Programming ...");
    return threadProgram(false);
}

bool CProject::DownloadFileUsingBatchErase()
{

    class IncrementBySector
    {
        size_t i;
        size_t m_sector_size;

    public:
        IncrementBySector(size_t sector_size):i(0), m_sector_size(sector_size)
        {

        }
        size_t operator()()
        {
            return m_sector_size * i++;
        }
    };

    // calculate effective area ...
    size_t minSectorSize =  m_context.chip.is_S25FL128P_256KSectors ? (1 << 18) : ( 1 << 16);
    pair<size_t, size_t> effectiveRange(0 , 
        ((m_downAddrRange.second & (minSectorSize - 1)) ? (m_downAddrRange.second | (minSectorSize - 1)) + 1 : m_downAddrRange.second));

    // calculate erase area ...
    vector<size_t> addrs(effectiveRange.second / minSectorSize);
    generate(addrs.begin(), addrs.end(), IncrementBySector(minSectorSize));

    // erasing ...
    Log(L"Erase area for download  ...");
    m_context.runtime.current_status += m_context.chip.is_S25FL128P_256KSectors ? L"(by 256KB)": L"(by 64KB)";

    m_chip->batchErase(addrs);  

    //refresh power
    if(boost::algorithm::iequals(SUPPORT_MACRONIX_MX25Lxxx, m_chip_id.Class)) RefreshVccValue();

    // programming ...
    Log(L"Programming ...");

    vector<unsigned char> vc(m_vBufferforLoadedFile);
    vc.resize(effectiveRange.second);
    return m_chip->rangeProgram(effectiveRange, vc);
}

bool CProject::BlazeUpdate()
{
    address_util::CAddrRoundup addr_round(m_context.chip.is_S25FL128P_256KSectors ? (1 << 18) : ( 1 << 16));
    pair<size_t, size_t> effectiveRange(addr_round.SectionRound(m_downAddrRange));

    if(!threadReadPartialChipData(effectiveRange)) return false;

    unsigned int offsetOfRealStartAddrOffset = m_downAddrRange.first - effectiveRange.first;


    vector<unsigned char> vc;
	vc.swap(m_vBufferForLastReadData);
	// m_vBufferForLastReadData.assign(boost::next(vc.begin(), offsetOfRealStartAddrOffset), 
		// boost::next(vc.begin(), m_downAddrRange.second - effectiveRange.first));

    vector<size_t> addrs(
        addr_round.GenerateDiff(
			vector<unsigned char>(
                boost::next(vc.begin(), offsetOfRealStartAddrOffset), 
                boost::next(vc.begin(), offsetOfRealStartAddrOffset + + (m_downAddrRange.second - m_downAddrRange.first))),
			m_vBufferforLoadedFile,
			m_downAddrRange.first)
        );

    if(addrs.empty())  // speed optimisation 
    {
        Log(L"Contents identical, update skipped  ...");
        return true;
    }
    else
    {
		wstringstream wss;
		wss << addrs.size() << L" segments" 
			<< (m_context.chip.is_S25FL128P_256KSectors ? L"(256KB each)": L"(64KB each)") 
			<< L" in total need to be updated.\n";
		Log(wss.str());

		vector<size_t> condensed_addr(addr_round.Condense(vc, addrs, effectiveRange.first));

		wss.str(L"");
		wss << L"Erase " << std::hex << condensed_addr.size() << L" non-blank segments"
			<<( m_context.chip.is_S25FL128P_256KSectors ? L"(by 256KB)": L"(by 64KB)")
			<<L" ...\n";
		Log(wss.str());

        m_chip->batchErase(condensed_addr); 

        // due to a chip bug of MX25L64D, need to power off the chip between erase and prog
        if(boost::algorithm::iequals(SUPPORT_MACRONIX_MX25Lxxx, m_chip_id.Class)) RefreshVccValue();

        Log(L"Preparing for programming ...");
        copy(m_vBufferforLoadedFile.begin(), 
             boost::next(m_vBufferforLoadedFile.begin(), m_downAddrRange.second - m_downAddrRange.first), 
             vc.begin() + offsetOfRealStartAddrOffset);

        size_t i = 1;
        BOOST_FOREACH(size_t addr, addrs)
        {
            wstringstream wss;
            wss << L"Programming " << i++ << L" of " << addrs.size() << L" sectors, starting from Address 0x" << std::hex << addr << L" ....\n";
            Log(wss.str());

            size_t idx_in_vc = addr - effectiveRange.first;

			if(!m_chip->rangeProgram(
                std::make_pair(addr, addr + addr_round.GetStep()), 
                vector<unsigned char>(boost::next(vc.begin(), idx_in_vc), boost::next(vc.begin(), idx_in_vc+ addr_round.GetStep()))
                )) return false;
        }
        return true;
    }
    return true;
}

bool CProject::batchUpdateUsingSectorErase()
{

    return BlazeUpdate();

    // get contents from chip

    //size_t minSectorSize =  m_context.chip.is_S25FL128P_256KSectors ? (1 << 18) : ( 1 << 16);
    //pair<size_t, size_t> effectiveRange(m_downAddrRange.first &(~(minSectorSize - 1)) , 
    //    ((m_downAddrRange.second & (minSectorSize - 1)) ? (m_downAddrRange.second | (minSectorSize - 1)) + 1 : m_downAddrRange.second));

    address_util::CAddrRoundup addr_round(m_context.chip.is_S25FL128P_256KSectors ? (1 << 18) : ( 1 << 16));
    pair<size_t, size_t> effectiveRange(addr_round.SectionRound(m_downAddrRange));

    vector<unsigned char> vc;
    if(!threadReadPartialChipData(effectiveRange)) return false;
    vc.swap(m_vBufferForLastReadData);

    unsigned int relativeStartAddr = m_downAddrRange.first - effectiveRange.first;

    // apply vpp
    // power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());

    // erase whole chip if partial erase not supported
	if(!address_util::helper::is_all_0xFF(vc))
    {

        Log(L"Check affected area ...");
        // query partial erase
        vector<size_t> addrs(
            addr_round.GenerateDiff(
            vector<unsigned char>(vc.begin() + relativeStartAddr, vc.begin() + relativeStartAddr + (m_downAddrRange.second - m_downAddrRange.first)),
            m_vBufferforLoadedFile,
            m_downAddrRange.first)
            );

        //Different(vector<unsigned char>(vc.begin() + relativeStartAddr, vc.begin() + relativeStartAddr + (m_downAddrRange.second - m_downAddrRange.first)),
        //    m_vBufferforLoadedFile, minSectorSize, m_downAddrRange.first, addrs);

        if(addrs.empty())  // speed optimisation 
        {
            Log(L"Contents identical, update skipped  ...");
            return true;

        }
        else
        {
            Log(std::wstring(L"Erase area for update  ...") + (m_context.chip.is_S25FL128P_256KSectors ? L"(by 256KB)": L"(by 64KB)"));

            m_chip->batchErase(addrs);  
        }
    }
    else
    {
        Log(L"Chip blank, Erase skipped");
    }


    // due to a chip bug of MX25L64D, need to power off the chip between erase and prog
    if(boost::algorithm::iequals(SUPPORT_MACRONIX_MX25Lxxx, m_chip_id.Class)) RefreshVccValue();

    copy(m_vBufferforLoadedFile.begin(), 
        m_vBufferforLoadedFile.begin() + (m_downAddrRange.second - m_downAddrRange.first), 
        vc.begin() + relativeStartAddr);

    Log(L"Programming ...");

    return m_chip->rangeProgram(effectiveRange, vc);

}

bool CProject::batchUpdateUsingChipErase()
{
    vector<unsigned char> vc;
    if(!threadReadWholeChipData()) return false;
    vc.swap(m_vBufferForLastReadData);

	if(!address_util::helper::is_all_0xFF(vc))
    {
        Log(L"Chip Not blank, Erase first");
        if(!threadEraseWholeChip()) return false;
    }
    else
    {
        Log(L"Chip blank, Erase skipped");
    }

    // due to a chip bug of MX25L64D, need to power off the chip between erase and prog
    //if(CChipInfo::CHIPID_MX25L64 == m_chip_id.first) RefreshVccValue();
    if(boost::algorithm::iequals(SUPPORT_MACRONIX_MX25Lxxx, m_chip_id.Class)) RefreshVccValue();

    copy(m_vBufferforLoadedFile.begin(), m_vBufferforLoadedFile.begin() + (m_downAddrRange.second - m_downAddrRange.first), vc.begin() + m_downAddrRange.first);

    Log(L"Programming ...");
    return m_chip->rangeProgram(make_pair(0, m_chip_id.ChipSizeInByte), vc);
}


bool CProject::batchUpdate()
{
    //bool fast_load = (m_chip_id.ChipSizeInByte > (1<<19));
    bool fast_load = (m_chip_id.ChipSizeInByte > (1<<16));

    return (m_chip->Is64KEraseSupported() && fast_load) ? batchUpdateUsingSectorErase():batchUpdateUsingChipErase();
}

bool CProject::BatchCustom()
{
    return true;
}


bool CProject::is_FileBufferEmpty()
{
    if(m_vBufferforLoadedFile.empty()) 
    {
        Log(L"Error: No file loaded, operation aborted!");
        // Logger::LogIt(m_context.runtime.current_status);
        return true;
    }
    else
    {
        return false;
    }
}

bool CProject::threadSADownload()
{
    int target_size = m_context.flashcard.FlashCard_TargetSizeInBytes;

    bool full_erase = target_size > (1<<25);
    bool result;

    if(full_erase)
    {
        result = DownloadFileUsingChipErase(false);
    }
    else
    {
        m_downAddrRange = make_pair(0, m_vBufferforLoadedFile.size());

        if(!ValidateAddrRangeBeforeProgram()) return false;
        //result = batchFastUpdateFile();
        result = DownloadFileUsingBatchErase();

    }

    result = threadCompareFileAndChip();

    return  result; 
}


// get required information 
// 0 = custom
// 1 = download current file
// 2 = download last file
// 3 = update with current file
// 4 = update with last file
bool CProject::threadPredefinedBatchSequences()
{
    // power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());

    if(is_FileBufferEmpty()) return false;

    size_t option;
    bool bReloadFile; 
    bool bVerifyAfterCompletion;

    //CPreference *preference = CPreference::getInstance();
    //option = preference->getIntHex(dedi_preference::Batch_SelectedOption);
    //bReloadFile = preference->getBool(dedi_preference::Batch_ReloadFile); 
    //bVerifyAfterCompletion = preference->getBool(dedi_preference::Batch_VerifyAfterCompletion);

    option = m_context.Batch_SelectedOption;
    bReloadFile = m_context.Batch_ReloadFile; 
    bVerifyAfterCompletion = m_context.Batch_VerifyAfterCompletion;
    // if reload file chosen
    if(bReloadFile) 
    {
        if(!ReloadLastFileFromPreferenceProfile()) return false;
    }

    if( 1 == option )
    {
        m_context.runtime.padding_char = m_context.Batch_PaddingCharacterWhenDownloadWithBlankCheck ;
    }
    else if(2 == option)
    {
        m_context.runtime.padding_char = m_context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck ;
    }
    else
    {

    }

    if(!ValidateAddrRangeBeforeProgram()) return false;

    bool result = true;
    switch(option)
    {
    case 0:
        result = BatchCustom();  // reserved
        break; 
    case 1: 
        result = DownloadFileUsingChipErase(true); 
        break; 
    case 2: 
        result = DownloadFileUsingChipErase(false); 
        break; 
    case 3:
        result = batchUpdate( ); 
        break; 
    default:
        Log(L"Error: Undefined operation!");
        // Logger::LogIt(m_context.runtime.current_status);
        return false;
    }

    if(bVerifyAfterCompletion && result) 
    {
        if(boost::algorithm::iequals(m_chip_id.Class, CAT45DBxxxB::getClassName()))RefreshVccValue();
        result &= threadCompareFileAndChip();
    }

    return result;
}

bool CProject::threadEraseWholeChip()
{
    // power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());

    Log(L"Erasing a whole chip ...");
    // Logger::LogIt(m_context.runtime.current_status);
    bool  result = m_chip->chipErase() ;
    Log(result ? L"A whole chip erased" : L"Error: Failed to erase a whole chip");
    // Logger::LogIt(m_context.runtime.current_status);
    return result;
}

bool CProject::threadBlankCheck()
{
    Log(L"Blank Checking ...");
    // Logger::LogIt(m_context.runtime.current_status);
    bool result = m_chip->rangeBlankCheck(make_pair(0, m_chip_id.ChipSizeInByte)) ;
    Log(result ? L"The chip is blank" : L"Warning: The chip is NOT blank");
    // Logger::LogIt(m_context.runtime.current_status);

    return result;
}

bool CProject::is_memoryChipChangedAfterLastDetection()
{
    const wstring NOT_SET(L"not_set");
    static wstring last_type(NOT_SET);

    if(last_type != NOT_SET)
    {
        if((m_chip_id.TypeName != last_type) && !m_chip_id.TypeName.empty())
        {
            last_type = m_chip_id.TypeName;
        }
        else
        {
            return false;
        }

    }
    return false;
    //return false;

    //if(GetTargetMode() != m_mode)
    //{
    //	m_mode = GetTargetMode();
    //	return false;
    //}

    //size_t id = this->m_chip_id.first;

    //MEMORYID_MAP mm;
    //readIds(mm);

    //if(mm.empty()) return false; // could be manully selected mode, in which case 

    //return mm.find(id) == mm.end(); 

    return true;
}

void CProject::threadRun(OPERATION_TYPE opType)
{
    if(m_context.runtime.is_feature_locked) 
    {
        Log(L"This feature is locked. Aborted.");
        return;
    }

    if(m_context.runtime.is_operation_on_going) 
    {
        Log(L"Error: Another read or write operation is running. Aborted.");
        return;
    }

    m_context.runtime.is_operation_successful = true;
    m_context.runtime.is_operation_on_going = true;

    if(is_good())
    {
        boost::timer timer ;

        if(!is_memoryChipChangedAfterLastDetection())
        {
            power::CAutoVccPower autopowerVcc(m_usb, m_context.power.vcc);

            // operations
            switch(opType)
            {
            case BLANKCHECK_WHOLE_CHIP:
                m_context.runtime.is_operation_successful &= threadBlankCheck();
                break;

            case ERASE_WHOLE_CHIP:
                { // apply vpp
                    power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());
                    m_context.runtime.is_operation_successful &= threadEraseWholeChip();
                }
                break;

            case PROGRAM_CHIP_WITH_OR_WITHOUT_PADDINGS:
                { // apply vpp
                    power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());

                    m_context.runtime.padding_char = m_context.Prog_PaddingCharacterWhenProgrammingWholeChip;
                    m_context.runtime.is_operation_successful &= threadProgram(true);
                }
                break;

            case READ_WHOLE_CHIP:
                m_context.runtime.is_operation_successful &= threadReadWholeChipData();
                break;

            case READ_ANY_BY_PREFERENCE_CONFIGURATION:
                m_context.runtime.is_operation_successful &= ThreadReadAnyByPreferenceConfiguration();
                break;

            case VERIFY:
                m_context.runtime.is_operation_successful &= threadCompareFileAndChip(); 
                break;

            case UPDATE_PARTIAL_CHIP_WITH_CURRENT_BUFFER:
                { // apply vpp
                    power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());
                    m_context.runtime.is_operation_successful &= batchUpdate();
                }
                break;
            case AUTO:
                { // apply vpp
                    power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());
                    m_context.runtime.is_operation_successful &= threadPredefinedBatchSequences() ;
                }
                break;

            case Download2Card:
                if(UpdateEEPROMOnCard())
                {
                    power::CAutoVppPower autopowerVpp(m_usb, SupportedVpp());
                    m_context.runtime.is_operation_successful &=  threadSADownload() ;
                    //m_context.runtime.is_operation_successful &= threadCompareFileAndChip();

                }
                else
                {
                    m_context.runtime.is_operation_successful = false;
                }
                break;

            default:
                break;
            }

            //m_context.runtime.is_operation_successful &= m_board.SetVccValue(CVppVccPower::POWEROFF);   // power OFF

            m_context.runtime.elapsed_time_of_last_operation = timer.elapsed() ;

            wstringstream ss;
            ss << L"Operation completed. \n" << m_context.runtime.elapsed_time_of_last_operation << L" seconds elapsed.";
#ifdef _DEDI_CONSOLE // for dpcmd
            //        m_context.runtime.current_status += ss.str();
#else
            Log(ss.str());
            // Logger::LogIt(m_context.runtime.current_status);
#endif

        }
        else
        {
            Log(L"Error: It seems that the memory chip is changed after last detection, please detect it again. \nOperation Aborted.");
            // Logger::LogIt(m_context.runtime.current_status);
        }
    }
    else
    {
        m_context.runtime.is_operation_successful = false;
    }

    m_context.runtime.is_operation_on_going  = false;
    m_chip->ClearCancelOperationFlag();

}

bool CProject::is_BoardVersionGreaterThan_1_1_6()
{
    return is_BoardVersionGreaterThan(L"1.1.6");
}

bool CProject::is_BoardVersionGreaterThan_3_0_0()
{
    return /*true || */is_BoardVersionGreaterThan(L"3.0.0");
}

bool CProject::is_SF300()
{
    return /*true || */boost::algorithm::iequals(getBoardInfo().board_type, L"SF300");
}

bool CProject::is_BoardVersionGreaterThan(const wstring& threshold)
{
    if(is_usbWorking(true))
    {
        CVersionInfo viCurrent(getBoardInfo().firmware_version);
        CVersionInfo viThreshold(threshold);

        return (viCurrent > viThreshold);
    }
    else
    {
        return false;
    }
}


void CProject::LoadTFIT( std::vector< unsigned char >& vTFIT,size_t iSize, bool bSupportVPP, bool bAT45, bool bNewVersion)
{
    wstringstream ssFile;
    CFileIO f;
    if(bNewVersion)// for new programmer
    {
        ssFile << (bAT45? L"AT45TFIT":L"TFIT")   << (iSize>>17) << ((bSupportVPP) ? L"VPP.bin" : L".bin");
    }
    else// for old programmer
    {
        ssFile << L"TFIT"  << (iSize>>17) << ((bSupportVPP) ? L"VPPold.bin" : L"old.bin");
    }

    wstring csPath(path_util::GetCommonAppDataPath() + ssFile.str());

    f.ReadByFileExt(csPath.c_str(), vTFIT);
}

void CProject::LoadTFIT( std::vector< unsigned char >& vTFIT, bool bNewVersion)
{
    CFileIO f;

    wstring tfitFile(path_util::GetCommonAppDataPath());

    tfitFile += bNewVersion ? m_context.flashcard.FlashCard_TFITFile : m_context.flashcard.FlashCard_TFITFileForV116Below;

    f.ReadByFileExt(tfitFile.c_str(), vTFIT);
}


void CProject::UpdateOpInfo(vector<unsigned char>& v, size_t targetSize, unsigned char sequence)
{
    v[0x0] = static_cast<unsigned char>((targetSize>>24) & 0xFF);
    v[0x1] = static_cast<unsigned char>((targetSize>>16) & 0xFF);
    v[0x2] = static_cast<unsigned char>((targetSize>>8) & 0xFF);
    v[0x3] = static_cast<unsigned char>((targetSize) & 0xFF);

    v[0x4] = sequence;  
    v[0xA] = 0x00;   /// crc flag
}

void CProject::UpdateOpInfo(vector<unsigned char>& v)
{
    size_t targetSize = m_context.flashcard.FlashCard_TargetSizeInBytes;
    v[0x0] = static_cast<unsigned char>((targetSize>>24) & 0xFF);
    v[0x1] = static_cast<unsigned char>((targetSize>>16) & 0xFF);
    v[0x2] = static_cast<unsigned char>((targetSize>>8) & 0xFF);
    v[0x3] = static_cast<unsigned char>((targetSize) & 0xFF);

    v[0x4] = m_context.flashcard.FlashCard_Option;  
    v[0xA] = 0x00;   /// crc flag
}

bool CProject::UpdateEEPROMOnCard()
{
    Log(L"Updating TFIT ...");
    // Logger::LogIt(m_context.runtime.current_status);


    CEepromOnFlashCard   eepromOnCard(m_usb);

    if(is_BoardVersionGreaterThan_1_1_6())
    {
        eepromOnCard.SetChipSize (0x100);
    }
    else
    {
        // old board w/ flash memory 2 support
        eepromOnCard.SetChipSize (0x80);
    }

    bool bRet = true;

    //========================
    //<1>. Download AuthenCode
    //========================
    // bRet   = eepromOnCard.DownloadAuthenCode(vAuthCode);  // anderson 06/12/27 comment
    // bRet   = true;


    //========================
    //<2>. Download OperationInfo
    //========================
    vector<unsigned char> vOpInfo(16);

    eepromOnCard.ReadOperatitonInfo(vOpInfo);
    UpdateOpInfo(vOpInfo);
    bRet  &= eepromOnCard.DownloadOperatitonInfo(vOpInfo);


    //========================
    //<3>. Download TFIT
    //========================
    vector<unsigned char> vTFIT;
    vTFIT.reserve(1024);

    LoadTFIT(vTFIT, is_BoardVersionGreaterThan_1_1_6());

    if(vTFIT.empty())
    {
        Log(L"Error: No file or Fail to open TFIT file, Aborted.");
        // Logger::LogIt(m_context.runtime.current_status);

        return false;
    }
    bRet  &= eepromOnCard.DownloadTFIT(vTFIT);                // anderson 070108

    //===================================
    //Checksum : 
    //===================================

    Sleep(20);

    vector<unsigned char> vReadBack;
    eepromOnCard.ReadTFIT(vReadBack);


    if(crc::CRC32(vTFIT) != crc::CRC32(vReadBack))
    {
        Log(L"Error: Update TFIT fail: checksum differs.");
        // Logger::LogIt(m_context.runtime.current_status);

        //AfxMessageBox(m_context.runtime.current_status.c_str());
        return false;
    }
    else
    {
        Log(L"Update TFIT completed successfully.");
        // Logger::LogIt(m_context.runtime.current_status);

        return true;
    }
}

void CProject::Run(OPERATION_TYPE type)
{
    m_thread.reset(new boost::thread(boost::bind(&CProject::threadRun, this, type)));
}


bool CProject::DownloadTFIT(vector<unsigned char>& vc)
{
    return m_board->DownloadTFIT(vc);
}

const memory::memory_id& CProject::getChipId() const
{
    return m_chip_id;
}

size_t CProject::getChipSizeInBytes() 
{
    return m_chip_id.ChipSizeInByte;
}

const programmer_board::board_info& CProject::getBoardInfo()
{
    return m_board->GetBoardInfo();
}
wstring CProject::getImageFileInfo(DediVersion::IMAGEFILE_ATTRBUTES info) 
{
    switch(info)
    {
    case DediVersion::IMAGEFILE_ATTRIBUTE_PATH:
        return m_imageFileInfo.first;

    case DediVersion::IMAGEFILE_ATTRIBUTE_NAME:
        return m_imageFileInfo.first.substr(m_imageFileInfo.first.rfind(L'\\') + 1);

    case DediVersion::IMAGEFILE_ATTRIBUTE_SIZE:
        return getSizeOfLoadedFile();

    case DediVersion::IMAGEFILE_ATTRIBUTE_CRC:
        return getDataCrcString(true);

    case DediVersion::IMAGEFILE_ATTRIBUTE_CRC_OF_WRITTEN_PART:
        return getDataCrcString(false);
    default:
        return L"Not Available";
    }
}

wstring CProject::getDataCrcString(bool bWholeFile) const
{
    wstringstream ss;
    if( bWholeFile )
    {
        ss << std::setfill(L'0')<<std::uppercase<<std::setw(8)<<std::hex<< m_context.file.file_buffer_crc;
    }
    else
    {
        ss << crc::CRC32InStringFormat(vector<unsigned char>(m_vBufferforLoadedFile.begin(), 
            m_vBufferforLoadedFile.begin() + (m_downAddrRange.second - m_downAddrRange.first)));
    }
    return ss.str();
}


CUSB& CProject::getUsbInstance()
{
    return m_usb;
}

double CProject::getElapsedTime() const
{
    return m_context.runtime.elapsed_time_of_last_operation;
};

wstring CProject::getStatusString()
{
    return m_context.runtime.current_status;
}

wstring CProject::getAllChipIdentifyingTrials() const
{
    return m_allChipIdentifyingTrials;
}

vector<unsigned char>& CProject::getBufferForLastReadChipContents() 
{
    return m_vBufferForLastReadData;
}

vector<unsigned char>& CProject::getBufferForLoadedFile()
{
    return m_vBufferforLoadedFile;
}

wstring CProject::getSizeOfLoadedFile()  const
{
    wstringstream wss;
    wss << L"0x" << std::hex << m_vBufferforLoadedFile.size() << " Bytes";
    return wss.str();
}

pair<size_t, size_t> CProject::getLastUpLoadedRange() const
{
    return m_upAddrRange;

}

pair<size_t, size_t> CProject::getLastDownloadedRange() const
{
    return m_downAddrRange;

}

bool CProject::UpdateFirmware(const wstring& file)
{
    return m_board->UpdateFirmware(file);
}

bool CProject::is_usbWorking(bool bReDetectUSB/* = false*/) 
{
    return (m_usb.is_open(bReDetectUSB));
}

bool CProject::is_good(bool bReDetectUSB/* = false*/) 
{
    return (m_usb.is_open(bReDetectUSB) && m_chip.get());
}

tribool CProject::is_readyToDownload()
{
    if(is_good())
    {
        if(!m_context.runtime.is_feature_locked)
        {
            if(m_vprjDownloadBuffer.size() <= m_chip_id.ChipSizeInByte)
            {
                return true;
            }
            else
            {
                Log(L"File size larger than chip size.");
                return false;
            }
        }
        else
        {
            return recognized_not_supported;
        }

    }
    else
    {
        Log(L"Error: USB communication or chip not ready.");
        return false;
    }
}

bool CProject::is_functions_locked() const 
{
    return m_context.runtime.is_feature_locked;
};

bool CProject::is_operationOnGoing() const 
{
    return m_context.runtime.is_operation_on_going;
};

bool CProject::is_operationSuccessful() const
{
    return m_context.runtime.is_operation_successful;
};


bool CProject::threadCompareFileAndChip()
{
    if(m_vBufferforLoadedFile.empty())
    {
        Log(L"Error: No loaded file to compare.");
        // Logger::LogIt(m_context.runtime.current_status);

        return false;
    }

    if(!ValidateAddrRangeBeforeProgram()) return false;

    threadReadPartialChipData(m_downAddrRange);

    size_t offset = min(m_downAddrRange.second - m_downAddrRange.first, m_vBufferforLoadedFile.size());
    unsigned int crcFile = crc::CRC32(vector<unsigned char>(m_vBufferforLoadedFile.begin(), m_vBufferforLoadedFile.begin() + offset));
    unsigned int crcChip = crc::CRC32(m_vBufferForLastReadData);

    bool result = (crcChip == crcFile);

    wstringstream ss;
    ss << (result ? L"Checksum Identical: " : L"Warning: Checksum Different: ") <<L"\n";
    //ss << L"File checksum([0, 0x"<< std::hex<< offset - 1 << L"] of 0x" << std::hex<< m_vBufferforLoadedFile.size() << L" bytes):"
    //    << std::hex << std::setw(8) << std::setfill(L'0') << crcFile
    //    << L" \n"
    //    << L"Chip checksum([0x"<< std::hex << m_downAddrRange.first << L", 0x"
    //    << std::hex << m_downAddrRange.second - 1<< L"] of 0x" << m_chip_id.ChipSizeInByte << L" bytes(chip size)): "
    //    << std::hex << std::setw(8) << std::setfill(L'0') << crcChip;
    ss << L"File checksum(" << std::hex<< m_vBufferforLoadedFile.size() << L" bytes):"
        << std::hex << std::setw(8) << std::setfill(L'0') << crcFile
        << L" \n"
        << L"Chip checksum([0x"<< std::hex << m_downAddrRange.first << L", 0x"
        << std::hex << m_downAddrRange.second - 1<< L"] of total 0x" << m_chip_id.ChipSizeInByte << L" bytes(chip size)): "
        << std::hex << std::setw(8) << std::setfill(L'0') << crcChip;
    Log(ss.str());
    // Logger::LogIt(m_context.runtime.current_status);

    return result;
}

bool CProject::ThreadReadAnyByPreferenceConfiguration()
{
    //CPreference *preference = CPreference::getInstance();
    //if(!preference->is_good()) 
    //{
    //    Log(L"Error: Configuration file missing. Please re-install Dediprog to solve this problem.");
    //    // Logger::LogIt(m_context.runtime.current_status);

    //    return false;
    //}

    //size_t addrStart = preference->getIntHex(dedi_preference::Read_CustomizedAddrFrom);
    //size_t addrLeng = preference->getIntHex(dedi_preference::Read_CustomizedDataLength);

    size_t addrStart = m_context.Read_CustomizedAddrFrom;
    size_t addrLeng = m_context.Read_CustomizedDataLength;
    if(0 == addrLeng && m_chip_id.ChipSizeInByte > addrStart) addrLeng = m_chip_id.ChipSizeInByte - addrStart;

    if(addrStart >= m_chip_id.ChipSizeInByte)
    {
        Log(L"Error: Starting address exceeding memory size, operation aborted."); 
        // Logger::LogIt(m_context.runtime.current_status);

        return false;
    }
#ifdef _DEDI_CONSOLE // for dpcmd
    else if((0 == addrStart) && (0== addrLeng))
#else       // for dediprog
    else if(0 == addrStart )
#endif
    {
        //std::cout <<"\n"<<addrStart<<"\n"<< addrLeng<<"\n" ;
        return threadReadWholeChipData();
    }
    else
    {
        if(addrStart + addrLeng > m_chip_id.ChipSizeInByte) 
        {
            Log(L"Warning: End address exceeding memory size,\n    - operation aborted when reaching memory capacity."); 
            // Logger::LogIt(m_context.runtime.current_status);
        }
        return threadReadPartialChipData(make_pair<size_t, size_t>(addrStart, addrStart + addrLeng));
    }
}

bool CProject::threadReadPartialChipData(const pair<size_t/* starting addr */,size_t/* data size */>& range)
{
    bool result;
    vector<unsigned char> vc; 

    wstringstream wss;

    wss << L"Reading From Address 0x" << std::hex << range.first
        << L" to  0x"<< std::hex << range.second - 1 << L" ...";

    Log(wss.str());
    // Logger::LogIt(m_context.runtime.current_status);

    result = m_chip->rangeRead(make_pair(range.first & 0xFFFFFE00,(range.second + 511) & 0xFFFFFE00), vc);
    Log(L"Finished reading from memory.");
    // Logger::LogIt(m_context.runtime.current_status);

    if(vc.empty()) return false;
    vector<unsigned char>::iterator offset = vc.begin() + (range.first & 0x1FF);
    vector<unsigned char>(offset, offset + (range.second - range.first)).swap(m_vBufferForLastReadData);

    m_upAddrRange = range;
    return result;

}

bool CProject::threadReadWholeChipData()
{
    Log(L"Reading a whole memory ...");
    // Logger::LogIt(m_context.runtime.current_status);

    bool b =  m_chip->rangeRead(make_pair(0,m_chip_id.ChipSizeInByte), m_vBufferForLastReadData);

    Log(L"Finish reading a whole memory");
    // Logger::LogIt(m_context.runtime.current_status);

    if(b) 
    {
        m_upAddrRange = make_pair(0, m_chip_id.ChipSizeInByte);

    }
    return b;
}

// presumptions: target area already erased
// read -> erase -> merge -> program
bool CProject::FetchProgramRangeFromUserPrefernce()
{
    //CPreference *preference = CPreference::getInstance();
    //if(!preference->is_good()) 
    //{
    //    Log(L"Error: Configuration file missing. Please re-install Dediprog to solve this problem.");
    //    // Logger::LogIt(m_context.runtime.current_status);

    //    return false;
    //}

    if(Context::STARTUP_APPLI_CARD == m_context.StartupMode)
    {
        m_downAddrRange = make_pair(0, m_vBufferforLoadedFile.size());
        m_context.runtime.padding_char = Context::CDediContext::NO_PADDING;
    }
    else
    {

#ifdef _DEDI_CONSOLE // for dpcmd
        //CString sPadding  =  preference->getString(dedi_preference::Prog_PaddingCharacterWhenProgrammingWholeChip);
        //sPadding.Trim();
        //m_paddingChar = ((sPadding.IsEmpty()) ? NO_PADDING : HexCString_to_Size_t(sPadding));
        //m_paddingChar = m_context.Prog_PaddingCharacterWhenProgrammingWholeChip;

        size_t len = m_context.Prog_CustomizedDataLength;

        size_t addrLeng = (0 == len) ? m_vBufferforLoadedFile.size() : len;
        size_t addrStart = m_context.Prog_CustomizedAddrFrom;
        m_downAddrRange = make_pair<size_t, size_t>(addrStart, addrStart +  addrLeng);

#else       // for dediprog
        //CString invokeFrom(preference->getString(dedi_preference::InvokeFrom));
        //if(    (preference->getBool(dedi_preference::Prog_PartialProgram)  && (dedi_preference::InvokeFromProg == invokeFrom)) 
        //    || (preference->getBool(dedi_preference::Batch_PartialProgram) && (dedi_preference::InvokeFromBatch == invokeFrom))
        //  )  /// partial program
        //{
        Context::INVOKE_FROM invokeFrom = m_context.runtime.InvokeFrom;
        if(    (m_context.Prog_PartialProgram)  && (Context::InvokeFromProg == invokeFrom)
            || (m_context.Batch_PartialProgram) && (Context::InvokeFromBatch == invokeFrom)
            )  /// partial program
        {

            size_t addrStart = (Context::InvokeFromProg == invokeFrom) ? m_context.Prog_CustomizedAddrFrom : m_context.Batch_CustomizedAddrFrom;

            size_t len = (Context::InvokeFromProg == invokeFrom) ? m_context.Prog_CustomizedDataLength : m_context.Batch_CustomizedDataLength;

            size_t addrLeng = (0 == len) ? m_vBufferforLoadedFile.size() : len;

            m_downAddrRange = make_pair<size_t, size_t>(addrStart, addrStart +  addrLeng);
        }
        else    /// program min(chipsize, filesize)
        {
            //        wstring sPadding;
            //        if(Context::InvokeFromBatch == m_context.runtime.InvokeFrom)  /// partial program
            //        {
            //            if(1 == m_context.Batch_SelectedOption)
            //            {
            //                sPadding = m_context.Batch_PaddingCharacterWhenDownloadWithBlankCheck;
            //            }
            //            else
            //            {
            //                sPadding = m_context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck;
            //            }
            //        }
            //        else
            //        {
            //            sPadding =  m_context.Prog_PaddingCharacterWhenProgrammingWholeChip;
            //        }

            //        boost::algorithm::trim(sPadding);
            //        m_context.Prog_PaddingCharacterWhenProgrammingWholeChip = 
            //(sPadding.empty() ? 
            //Context::CDediContext::NO_PADDING : 
            //static_cast<unsigned int>(numeric_conversion::hexstring_to_size_t(sPadding)));

            m_downAddrRange = make_pair(0, m_vBufferforLoadedFile.size());
        }
#endif
    }

    wstringstream wss;
    wss << L"Programming Info: \n"
        << L"Source File = " << getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_NAME) <<L"(" << L"0x" << std::hex << m_vBufferforLoadedFile.size() << L" bytes), \n"
        << L"Target memory region = [0x" << std::hex << m_downAddrRange.first << L", "<< L"0x" << std::hex << m_downAddrRange.second - 1 << L"], \n";
    if(Context::CDediContext::NO_PADDING == m_context.runtime.padding_char)
    {
        wss << L"no treatment on spare memory space.";
    }
    else
    {
        wss << L"Requested to fill spare space with 0x"<< std::hex << std::setw(2) << std::setfill(L'0') << m_context.runtime.padding_char;
    }
    Log(wss.str());
    // Logger::LogIt(m_context.runtime.current_status);

    return true;
}

bool CProject::ValidateAddrRangeBeforeProgram()
{
    if(!FetchProgramRangeFromUserPrefernce()) return false;

    /// special treatment for AT45DBxxxD
    wstringstream wss;
    if(ChipInfo::AT45DB::IsAT45DB(m_chip_id))
    {
        if(boost::algorithm::iequals(m_chip_id.Class, CAT45DBxxxD::getClassName()))
        {
            if (! dynamic_cast<CAT45DBxxxD*>(m_chip.get())->ValidateAddr(m_downAddrRange.first)) 
            {
                wss << L"Error: Starting Address 0x" << std::hex << m_downAddrRange.first
                    << L" is invalid, possibly page address exceeding page size."
                    << L"\nOperation aborted!";

                Log(wss.str());
                // Logger::LogIt(m_context.runtime.current_status);

                return false;
            }
        }
        else if (boost::algorithm::iequals(m_chip_id.Class, CAT45DBxxxB::getClassName()))
        {
            if (! dynamic_cast<CAT45DBxxxB*>(m_chip.get())->ValidateAddr(m_downAddrRange.first)) 
            {
                wss << L"Error: Starting Address 0x" << std::hex << m_downAddrRange.first
                    << L" is invalid, possibly page address exceeding page size."
                    << L"\nOperation aborted!";

                Log(wss.str());
                // Logger::LogIt(m_context.runtime.current_status);

                return false;
            }
        }
        else
        {}
    }

    /// if file size exceeds memory size, it's an error
    /// if user-specified length exceeds, just ignore
    wss.str(L"");
    if(m_downAddrRange.first > m_chip_id.ChipSizeInByte - 1) 
    {
        wss << L"Error: Starting Address 0x" << std::hex << m_downAddrRange.first
            << L" >  "
            << L"0x" << std::hex << m_chip_id.ChipSizeInByte - 1 << L" (end of chip)."
            << L"\nOperation aborted!";

        Log(wss.str());

        return false;
    }

    wss.str(L"");
    size_t size = m_downAddrRange.second - m_downAddrRange.first;
    if(size > m_vBufferforLoadedFile.size()) 
    {
        wss << L"Error: End Address 0x" << std::hex << m_downAddrRange.second - 1
            << L" beyond the end of file(size: "
            << L"0x" << std::hex << m_vBufferforLoadedFile.size() << L" bytes)."
            << L"\nOperation aborted!";
        Log(wss.str());

        return false;
    }

    wss.str(L"");
    if(m_downAddrRange.second > m_chip_id.ChipSizeInByte) 
    {
        wss << L"Error: End Address 0x" << std::hex << m_downAddrRange.second
            << L" beyond the size of chip(size: "
            << L"0x" << std::hex << m_chip_id.ChipSizeInByte  << L" bytes)."
            << L"\nOperation aborted!";
        Log(wss.str());

        return false;
    }

    return true;

}

bool CProject::ProgramAnywhereNoUserSpecifiedPadding()
{
    /// get real length and regulate it to multiples of 512.
    size_t realEndAddr = ( m_downAddrRange.second  + 511) & 0xFFFFFE00;
    size_t realStartAddr = m_downAddrRange.first & 0xFFFFFE00;

    vector<unsigned char> vc(realEndAddr - realStartAddr, 0xFF); /// got to be 0xFF(erased) for the paddings for multiples of 512.
    copy(m_vBufferforLoadedFile.begin(), m_vBufferforLoadedFile.begin() + (m_downAddrRange.second - m_downAddrRange.first), vc.begin() + (m_downAddrRange.first & 0x1FF));

    return m_chip->rangeProgram(make_pair(realStartAddr, realEndAddr), vc);
}

bool CProject::ProgramAnywhereWithUserSpecifiedPadding()
{
    vector<unsigned char> vc(m_chip_id.ChipSizeInByte, static_cast<unsigned char>(m_context.runtime.padding_char));

    copy(m_vBufferforLoadedFile.begin(), 
        m_vBufferforLoadedFile.begin() + (m_downAddrRange.second - m_downAddrRange.first), 
        vc.begin() + m_downAddrRange.first);

    return m_chip->rangeProgram(make_pair(0, m_chip_id.ChipSizeInByte), vc);
}

// write whole memory from addr=0, padding the rest of the memory conditionally
bool CProject::threadProgram(bool bValidateParameters)
{
    if(bValidateParameters)
    {
        if(m_vBufferforLoadedFile.size() == 0)
        {
            Log(L"Error: No file loaded, operation aborted!");

            return false;
        }

        if(!ValidateAddrRangeBeforeProgram())  return false;
    }

    bool result;
    result = ((Context::CDediContext::NO_PADDING == m_context.runtime.padding_char) ? ProgramAnywhereNoUserSpecifiedPadding() : ProgramAnywhereWithUserSpecifiedPadding());

    if(result)
    {
        Log(L"Programming OK.");    

        return true;
    }
    else
    {
        Log(m_context.runtime.current_status + L"\nError: Programming Failed.");            

        return false;
    }
}


void CProject::UpdateWithExternalData(const vector<unsigned char>& vc)
{
    m_thread.reset(new boost::thread(boost::bind(&CProject::ThreadUpdateWithExternalData, this, vc)));
}

void CProject::ThreadUpdateWithExternalData(vector<unsigned char>& vc)
{
    if(m_context.runtime.is_operation_on_going)
    {
        Log(L"Error: Another read or write operation is running. Aborted..");

        return; 
    }

    Log(L"Updating memory chip...");

    m_context.runtime.is_operation_on_going = true;
    m_context.runtime.is_operation_successful = true;

    {
        boost::timer timer ;
        timer.restart() ;

        power::CAutoVccPower autopower(m_usb, m_context.power.vcc);
        m_context.runtime.is_operation_successful &= m_chip->chipErase();
        m_context.runtime.is_operation_successful &= m_chip->rangeProgram(make_pair(0, m_chip_id.ChipSizeInByte), vc);


        m_context.runtime.elapsed_time_of_last_operation = timer.elapsed() ;
    }

    Log(L"Finish Updating to memory chip");

    m_context.runtime.is_operation_on_going = false;
}

void CProject::CancelOperation()
{
    m_chip->SetCancelOperationFlag();
}

wstring CProject::getTypeDisplayName() const
{
    if(Context::STARTUP_APPLI_CARD != m_context.StartupMode)
    {
        if(CONSOLE_USERS == m_user_group)
        {
            return m_AllMatchesInString + L"\n" + m_chip_id.TypeName + L" parameters to be applied by default\n";
        }
        else   
        {
            return m_chip_id.TypeName;
        }
    }
    else
    {
        map<size_t, wstring> cardNames = boost::assign::map_list_of
            (1 << 20,  L"FC08(8Mbit)"   )
            (1 << 21,  L"FC16(16Mbit)"  )
            (1 << 22,  L"FC32(32Mbit)"  )
            (1 << 23,  L"FC64(64Mbit)"  )
            (1 << 24,  L"FC128(128Mbit)");

        return cardNames[m_chip_id.ChipSizeInByte];
    }
}

bool CProject::readIds(vector<memory::memory_id>& match ) 
{
    CFlashIdentifier fid(this);
    m_allChipIdentifyingTrials = fid.get_formated_trials();
    match = fid.get_all_matches();

    return !match.empty();
}

bool CProject::readSR(unsigned char& cSR)
{
    using namespace power;
    if(m_chip.get())
    {
        bool is_power_on = power::vccPOWEROFF != power::CVppVccPower::m_vcc;

        CVppVccPower p(m_usb);
        p.SetVppORVccVoltage(m_context.power.vcc, power::CVppVccPower::VOLTAGE_VCC);
        bool b = m_chip->readSR(cSR);
        if(!is_power_on) p.SetVppORVccVoltage(power::vccPOWEROFF, power::CVppVccPower::VOLTAGE_VCC);
        return b;
    }
    else
    {
        return false;
    }
}

bool CProject::writeSR(unsigned char cSR)
{
    using namespace power;
    if(m_chip.get())
    {
        bool is_power_on = power::vccPOWEROFF != power::CVppVccPower::m_vcc;

        CVppVccPower p(m_usb);
        p.SetVppORVccVoltage(m_context.power.vcc, power::CVppVccPower::VOLTAGE_VCC);

        bool b = m_chip->writeSR(cSR);

        if(!is_power_on) p.SetVppORVccVoltage(power::vccPOWEROFF, power::CVppVccPower::VOLTAGE_VCC);
        return b;
    }
    else
    {
        return false;
    }
}

bool CProject::setCrIfMCF()
{	
	if(boost::algorithm::iequals(m_chip_id.Class, SUPPORT_FREESCALE_MCF))
	{
		return writeSR(m_context.chip.mcf_config_register);
	}
	else
	{
		return true;
	}
}
void CProject::RefreshVccValue()
{
    power::CVppVccPower p(m_usb);
    p.RefreshVccValue(m_context.power.vcc);
}

bool CProject::isFWUpdatable(std::pair<wstring,wstring>& type_version)
{
    return m_board->isUpdatable(type_version);
}

bool CProject::UpdateFirmware()
{
    return m_board->UpdateFirmware();
}

void CProject::setTargetFlash(Context::STARTUP_MODE mode)
{
    m_context.StartupMode = mode;
    m_board->SetTargetFlash();
}

power::VPP_VALUE CProject::SupportedVpp()
{
    if(Context::STARTUP_APPLI_CARD == m_context.StartupMode)
    {
        return power::vppdo_nothing; //m_context.flashcard.FlashCard_UsingVpp ? m_context.power.vpp : power::vppdo_nothing;
    }
    else
    {
        return m_context.power.Misc_ApplyVpp ? m_context.power.vpp : power::vppdo_nothing;
    }
}

void CProject::ResetLock( )
{
    m_context.runtime.is_feature_locked = false;
}

void CProject::Log(const wstring& line)
{
    m_context.runtime.current_status = line;
    Logger::LogIt(m_context.runtime.current_status);
}
