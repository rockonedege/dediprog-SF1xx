/**
* <DpCmd.h>
* 
* Copyright (c) 2006 by Dediprog
*/
#include "stdafx.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>

#include "DpCmd.h"
#include "Console.h"
//#include "FlashCardSettings.h"
#include "VerInfo.h"
#include "IntelHexFile.h"
#include "PreferenceEx.h"
#include "DediContext.h"
#include "numeric_conversion.h"
#include "moduleversion.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "memory_id.h"


using std::cout;
using std::wcout;
using std::copy;
using std::string;
using std::wstring;
using boost::timer;
using boost::progress_display;

//const
static const string    msg_err_communication = "Error: USB communication error or configuration file missing.";
static const string    msg_err_openfile      = "Error: Failed to open file.";
static const string    msg_err_identifychip  = "Error: chip not identified.";
static const string    msg_err_timeout_abortion = "\nError: Abort abnormally due to timeout.\n*** You might need to un-plug the USB device to recover the state. ***\n";
static const string    msg_err_lengthoverflow_abortion= "\nError: Length parameter exceeding file size, operation aborted\n";
static const string    msg_err_addroverflow_abortion  = "\nError: Exceeding chip size, operation aborted\n";
static const string    msg_warning_optionignore_al  = "\nWarning: either --addr or --length cannot be used with --auto, will be ignored\n";
static const string    msg_info_loading     = "\nLoading file, please wait ...";
static const string    msg_info_checking     = "\nChecking, please wait ...";
static const string    msg_info_erasing      = "\nErasing, please wait ...";
static const string    msg_info_programming  = "\nProgramming, please wait ...";
static const string    msg_info_reading      = "\nReading, please wait ...";
static const string    msg_info_auto         = "\nAuto Sequences, please wait ...";
static const string    msg_info_verifying    = "\nVerifying, please wait ...";
static const string    msg_info_unknownoption= "\nError: illegal option.";
static const string    msg_info_chipblank    = "\nThe chip is blank";
static const string    msg_info_chipnotblank = "\nThe chip is NOT blank";
static const string    msg_info_eraseOK      = "\nErase OK";
static const string    msg_info_erasefail    = "\nError: Erase Failed";
static const string    msg_info_progOK       = "\nProgram OK";
static const string    msg_info_progfail     = "\nError: Program Failed";
static const string    msg_info_readOK       = "\nRead OK";
static const string    msg_info_readfail     = "\nRead Failed";
static const string    msg_info_autoOK       = "\nAutomatic program OK";
static const string    msg_info_autofail     = "\nError: Automatic program Failed";
static const string    msg_info_verifyOK     = "\nVerify OK";
static const string    msg_info_verifyfail   = "\nError: Verify Failed";
static const string    msg_info_basicusage   = "Basic Usages:\n"
"Dpcmd -uxxx\n"
"Dpcmd /uxxx\n"
"Dpcmd --auto=xxx\n"
"(space is not needed between the switches and parameters. E.g. dpcmd -ubio.bin)";
static const string    msg_info_usageexample = 
"Usage Examples:\n"
"1. dpcmd -r\"f:\\file.bin\", \n    reads the chip and save it into a file \"file.bin\"\n"
"2. dpcmd -rSTDOUT -a0x100 -l0x23, \n    reads 0x23 bytes starting from 0x100 and display it on the screen\n"
"3. dpcmd -ufile.bin, \n    Update the whole chip with file.bin\n"
"4. Dpcmd -ufile.bin -a0x100, \n    Replace the serial flash contents for the size of file.bin with file.bin.\n"
"    (contents outside the replace area will be kept unchanged)\n"
"5. dpcmd -pfile.bin -a0x100, \n    program file.bin into the serial flash starting from address 0x100\n"
"    (the target area is assumed to be blank already)\n"
"6. dpcmd -pfile.bin -x0xaa, \n    programs file.bin into the serial flash and fill the rest area with 0xaa\n"
"    (the target area is assumed to be blank already)\n"
"Remark: -a, -l only works with -p, -r, -s and -u\n"
"Remark: -x only works with -p";

static const wstring   msg_info_saveOK       = L"\nSuccessfully saved into file: ";
static const wstring   msg_err_saveFail     = L"\nFailed to save into file: ";
static const wstring   defaultproj           = L"\\DefaultProject.dediprj";

unsigned long str2ulng(string s)
{
    int base = 10;

    if(s.size() > 2)
    {
        if(s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X" )
        {
            base = 16;      // hex 
        }
    }

    return  strtoul(s.c_str(), 0, base);
}


void OutputFormatedMsg(string s)
{
    cout << JadedHoboConsole::bg_red << JadedHoboConsole::fg_white 
        << s 
        << JadedHoboConsole::bg_black<< JadedHoboConsole::fg_white;
}

class my_timer:boost::progress_timer
{
public:
    ~my_timer()
    {
        std::cout << "\nTime elapsed: ";
    }
};


class CLogging
{
public:
    CLogging(CDpCmd* dp, bool skip) 
        : m_dp(dp)
        , m_file((path_util::GetPersonalAppDataPath() + L"log.txt").c_str(), ios::trunc)
        , m_skip(true)
    {
        if(m_file && m_skip)
        {
            m_file << print_time_stamp() << L"USB communication = " << std::boolalpha << m_dp->get_status() << std::endl;
        }
    }

    ~CLogging()
    {
        if(m_file && m_skip)
        {
            m_file << print_time_stamp() << L"operation result = " << std::boolalpha << m_dp->get_status() << std::endl;
        }
    }
private:
    wstring print_time_stamp()
    {
        using namespace boost::posix_time;

        ptime t(second_clock::local_time());

        std::wstringstream wss;
        wss << to_simple_wstring(t)
            << L"\t";

        return wss.str();
    }

private:
    CDpCmd* m_dp;
    std::wofstream m_file;
    bool  m_skip;
};


CDpCmd::CDpCmd()
: m_proj(new CProject(CProject::CONSOLE_USERS, Context::STARTUP_APPLI_SF_1)) 
, m_context(m_proj->get_context())
, m_status(true)
{
}

bool CDpCmd::get_status()
{
    return m_status;
}
int CDpCmd::Dispatch(int ac, TCHAR* av[])
{
    using namespace std;
    try {
        po::options_description basic_desc("Basic Switches(switches in this group are mutual exclusive)");
        po::options_description optional_desc("Advances Switches(specify the following switches to change default values)");
        po::options_description desc("");

        basic_desc.add_options()
            ("help,?",     "show this help message")
            ("list",       "print supported chip list")
            ("detect,d",   "detect chip")
            ("blank,b",    "blank check")
            ("erase,e",    "erase entire chip")
            ("read,r", po::value<string>(),    "read chip contents and save to a bin/hex/s19 file"
                                                "\n- use STDOUT for the console.")
            ("prog,p", po::value<string>(),    "program chip without erase")
            ("auto,u", po::value<string>(),    "automatically run the following sequence:"
                                                "\n- check if the chip is blank or not;"
                                                "\n- erase the entire chip(if not blank);"
                                                "\n- program a whole file starting from address 0")
                                                ("sum,s",      "display chip content checksum")
            ("fsum,f", po::value<string>(),    "display the file checksum"
                                                "\n- needs to work with a file")        ;
        optional_desc.add_options()
            ("silent,i",   "supress the display of real-time timer counting"
                            "\n- used when integrating with 3rd-party tools(e.g. IDE)")
            ("verify,v",   "verify checksum file and chip"
                            "\n- works with --prog/auto only")
            ("fill,x",   po::value<string>()->default_value("FF"), "fill spare space with an hex value(e.g.FF),"
                                                                    "\n- works with --prog, --auto only")
            ("addr,a",   po::value<string>()->default_value(""),   "starting address(e.g. 0x1000),"
                                                                    "\n- works with --prog/read/sum/auto only"
                                                                    "\n- defaults to 0, if omitted.")
            ("length,l", po::value<string>()->default_value(""), "length to read/program in bytes,"
                                                                    "\n- works with --prog/read/sum/auto only"
                                                                    "\n- defaults to whole file if omitted")
            ("timeout,t",po::value<unsigned int>()->default_value(300), "Timeout value in seconds")
            ("target,g", po::value<unsigned int>()->default_value(1), "Target Options \nAvailable values: "
                                                                    "\n    1, Chip 1(Default)"
                                                                    "\n    2, Chip 2"
                                                                    "\n    0, reference card")
            ("vcc", po::value<unsigned int>(), "specify vcc(* Not Implemented *)"
                                                "\n    0, 3.5V(Default)"
                                                "\n    1, 1.8V")
            ("vpp", "apply vpp when the memory chip supports it"
                                                "\n- work with --prog and --erase.")
            ("type", po::value<string>()->default_value(""), "Specify a type to override auto detection"
                                                "\n- use --list arguement to look up supported type.")
            ("log", "write operation result into file \"%appdata%\\dediprog\\log.txt\"")
            ("raw-instruction", po::value<string>(), "issue raw serial flash instructions. "
                                                "\n- use spaces(\" \") to delimite bytes."
                                                "\n- instructions must be enclosed in double quotation marks(\"\")"
                                                "\nExample:"
                                                "\ndpcmd --raw-instruction \"03 FF 00 12\" ")
            ("raw-require-return", po::value<size_t>()->default_value(0), "bytes of result to return after issuing raw instructions. "
                                                "\n- used along with --raw-instruction only."
                                                "\nExample:"
                                                "\ndpcmd --raw-instruction \"03 FF 00 12\" --raw-require-return 1")
            ;

        desc.add(basic_desc).add(optional_desc);

        //po::variables_map m_vm;        
        po::store(po::parse_command_line(ac, av, desc, 
            po::command_line_style::default_style | po::command_line_style::allow_slash_for_short),
            m_vm);
        po::notify(m_vm);   

        if ( 1==ac ) 
        {
            cout << msg_info_basicusage << std::endl;
            cout << desc << "\n";
            //system("Pause");
            return 1;
        }
        else if(m_vm.count("help"))
        {
            cout << msg_info_basicusage << std::endl
                << msg_info_usageexample << std::endl;
            cout << desc << "\n";
            system("Pause");
            return 1;
        }
        else
        {
            Handler();                          
        }
    }
    catch(exception& e) 
    {
        cerr << JadedHoboConsole::bg_red << JadedHoboConsole::fg_white 
            << "error: "                 
            << e.what() << "\n"
            << JadedHoboConsole::bg_black<< JadedHoboConsole::fg_white;
        return 1;
    }
    catch(...) 
    {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}


void CDpCmd::Handler( )
{
    if(!InitProject()) return;

    //my_timer t;                           // opertion timer

    if(ListTypes()) return;

    CLogging logging(this, m_vm.count("log") > 0); // start logging from here.

    if(!m_vm["type"].as<string>().empty())
    {
        Sequence();
    }
    else if(DetectChip())
    { 
        Sequence();
    }   
}

bool CDpCmd::InitProject()
{
    //m_proj.reset() ;

    boost::array< Context::STARTUP_MODE, 3> targets = 
    {
        Context::STARTUP_APPLI_CARD,
        Context::STARTUP_APPLI_SF_1,
        Context::STARTUP_APPLI_SF_2,
    };

    m_proj->setTargetFlash(targets[m_vm["target"].as<size_t>()]);

    string type(m_vm["type"].as<string>()); 
    if(!type.empty())
    {
        wstring wtype;
        wtype.assign(type.begin(), type.end());

        memory::memory_id id;
        ChipInfo::CSortedChipList sorted_list;
        BOOST_FOREACH(const memory::memory_id& mid, sorted_list.get_supported_chiplist_ordered_by_manufacturer())
        {
            if(boost::algorithm::iequals(mid.TypeName, wtype))
            {
                id = mid;
                break;
            }
        }
        std::wcout << L"Chip Type " << (id.is_empty() ? L"Unknown" : id.TypeName) << L" is applied manually."; 
        m_proj->Init(id);
    }
    else
    {
        m_proj->Init();
    }

    if(m_proj->is_good(true))
    {
        if(!m_proj->is_functions_locked())
        {
            m_status = true;
        }
        else
        {
            std::wcout << m_proj->getStatusString(); 

            m_status = false;
        }

    }

    return m_status;

}

void CDpCmd::Sequence()
{
    // *** the calling order in the following block must be kept as is ***
    RawInstructions();
    BlankCheck();
    Erase();
    Program();
    Read();
    Auto();
    CalChecksum();
    Verify();
}


bool CDpCmd::ListTypes()
{
    if(m_vm.count("list"))
    {
        ChipInfo::CSortedChipList sorted_list;
        BOOST_FOREACH(const memory::memory_id& mid, sorted_list.get_supported_chiplist_ordered_by_manufacturer())
        {
            std::wcout << mid.TypeName << L",\t\t\t\tby " << mid.Manufacturer << L"\n";
        }
        return true;
    }
    return false;
}

bool CDpCmd::DetectChip()
{   
    if(! m_proj->is_good(true)) {OutputFormatedMsg(msg_err_communication); return false; }
    if(0 == m_proj->getChipId().UniqueID) { OutputFormatedMsg(msg_err_identifychip); return false; }

    std::wcout << L"By reading the chip ID, the chip applies to "; 
    std::wcout << m_proj->getTypeDisplayName() ;

    return true;

}

void CDpCmd::SetVpp()
{
    m_context.power.Misc_ApplyVpp = ( m_vm.count("vpp") > 0);
}

bool CDpCmd::do_loadFile()
{
    string s;
    if(m_vm.count("prog"))
    {
        s = m_vm["prog"].as<string>().c_str();
    }
    else if(m_vm.count("auto"))
    {
        s = m_vm["auto"].as<string>().c_str();
    }
    else if(m_vm.count("fsum"))
    {
        s = m_vm["fsum"].as<string>().c_str();
    }
    else
    {}

    cout << msg_info_loading << "("<< s <<")";    // load file

    CString filename(s.c_str());
    CString sExt(filename.Right(4).MakeLower());
    if( sExt == ".bin" || sExt == ".hex" || sExt == ".s19")
    {
        m_proj->LoadFile(filename.GetString()); 
    }
    else
    {
        m_proj->LoadFile(pair<wstring, CFileIO::FILE_FORMAT>(filename.GetString(), CFileIO::BIN));
    }
    bool b=Wait();

    wcout << std::endl << m_proj->getStatusString();    // load file

    return b;
}

void CDpCmd::do_BlankCheck()
{
    cout << msg_info_checking;
    m_proj->Run(CProject::BLANKCHECK_WHOLE_CHIP);

    cout << (Wait() ? msg_info_chipblank : msg_info_chipnotblank); 
}

void CDpCmd::do_Erase()
{
    cout << msg_info_erasing;
    m_proj->Run(CProject::ERASE_WHOLE_CHIP);

    cout << (Wait() ? msg_info_eraseOK : msg_info_erasefail) ;
}
void CDpCmd::do_Program()
{
    if(!do_loadFile()) return;

    SaveProgContextChanges();

    cout << msg_info_programming;
    m_proj->Run(CProject::PROGRAM_CHIP_WITH_OR_WITHOUT_PADDINGS);

    if(Wait())
    {
        cout << msg_info_progOK; 
        wcout << L"\nChecksum(whole file): " << m_proj->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_CRC);
        wcout << L"\nChecksum(Written part of file): " << m_proj->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_CRC_OF_WRITTEN_PART);
    }
    else
    {
        OutputFormatedMsg(msg_info_progfail); 
    }
}

void CDpCmd::do_Read()
{
    m_context.Read_CustomizedAddrFrom = numeric_conversion::hexstring_to_size_t(m_vm["addr"].as<string>());
    m_context.Read_CustomizedDataLength = numeric_conversion::hexstring_to_size_t(m_vm["length"].as<string>());

    cout << msg_info_reading;
    m_proj->Run(CProject::READ_ANY_BY_PREFERENCE_CONFIGURATION); 
    cout << (Wait() ? msg_info_readOK : msg_info_readfail); 
    cout<<"\n";
}

void CDpCmd::do_DisplayOrSave()
{
    if(!m_vm.count("read")) return;
    CString s(m_vm["read"].as<string>().c_str());

    if(s.IsEmpty()) return;

    if("STDOUT"== s)
    {
        cout<<"\n";
        cout << "Hex value display (starting from "
            << (m_vm["addr"].defaulted() ? "0" : m_vm["addr"].as<string>())
            << ", "
            << (m_vm["length"].defaulted() ? "end of file" :m_vm["length"].as<string>())
            << " bytes in total):  \n"; 

        vector<unsigned char> v(m_proj->getBufferForLastReadChipContents());
        std::copy(v.begin(), v.end(), std::ostream_iterator< unsigned short>(cout <<std::uppercase << std::setw(2) << std::hex, " "));
    }
    else
    {
        CFileIO f;
        if(f.WriteBINFile(m_proj->getBufferForLastReadChipContents(), s.GetString()))
        {
            std::wcout << msg_info_saveOK<< std::wstring(s.GetString()); 
        }
        else
        {
            std::wcout << JadedHoboConsole::bg_red << JadedHoboConsole::fg_white 
                << msg_err_saveFail <<  std::wstring(s.GetString())
                << JadedHoboConsole::bg_black << JadedHoboConsole::fg_white;
        }
    }

}

void CDpCmd::SaveProgContextChanges()
{
    m_context.Prog_PaddingCharacterWhenProgrammingWholeChip = numeric_conversion::hexstring_to_size_t(m_vm["fill"].as<string>());
    m_context.Prog_CustomizedAddrFrom = numeric_conversion::hexstring_to_size_t(m_vm["addr"].as<string>());
    m_context.Prog_CustomizedDataLength = numeric_conversion::hexstring_to_size_t(m_vm["length"].as<string>());
}

void CDpCmd::do_Auto()
{
    if(!do_loadFile()) return;

    SaveProgContextChanges();

    cout << msg_info_auto;
    m_proj->Run(CProject::AUTO);  

    if(Wait())
    {
        cout << msg_info_autoOK; 
        wcout << L"\nChecksum(file): " << m_proj->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_CRC);
        wcout << L"\nChecksum(Written part of file): " << m_proj->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_CRC_OF_WRITTEN_PART);
    }
    else
    {
        OutputFormatedMsg(msg_info_autofail);    
    }
}

void CDpCmd::do_Verify()
{
    cout << msg_info_verifying;
    m_proj->Run(CProject::VERIFY);

    cout << (Wait() ? msg_info_verifyOK : msg_info_verifyfail) ;

}

void CDpCmd::do_ReadSR()
{
    unsigned char c;
    if(m_proj->readSR(c))
    {
        cout <<"\nSR = "; 
        cout << std::setfill('0')<<std::uppercase<<std::setw(2)<< std::hex << (short)c << std::endl;
    }
    else
    {
        cout << "SR unavailable";
    }
}

void CDpCmd::do_RawInstructions()
{
    using namespace boost::algorithm; 
    string s(m_vm["raw-instruction"].as<string>());

	//vector<string> sequences;
 //   split(sequences, s,is_any_of("#"), token_compress_on);

 //   BOOST_FOREACH(string& one_sequence, sequences)
 //   {
	    vector<string> out;
	    split(out, s/*one_sequence*/,is_any_of(" ,;-"), token_compress_on);

        vector<unsigned char> vOut;
	    BOOST_FOREACH(string& val, out)
	    {
            unsigned char c = static_cast<unsigned char>(numeric_conversion::hexstring_to_size_t(val));
		    vOut.push_back(c);
	    }

	    vector<unsigned char> vIn(m_vm["raw-require-return"].as<size_t>());
        {
            power::CVppVccPower p(m_proj->getUsbInstance());
            p.SetVppORVccVoltage(m_proj->get_context().power.vcc, power::CVppVccPower::VOLTAGE_VCC);

            CFlashCommand cmd(m_proj->getUsbInstance());

            (!vIn.empty()) ? 
                cmd.SendCommand_OneOutOneIn(vOut,vIn)
                :
                cmd.SendCommand_OutOnlyInstruction(vOut)
            ;
        }

        if(!vIn.empty())
        {
            cout << std::endl 
                <<"issuing raw instruction \""<< s 
                << "\" returns " << vIn.size() <<" bytes as required:\n";

	        BOOST_FOREACH(unsigned char val, vIn)
	        {
		        cout << std::setfill('0')<<std::uppercase<<std::setw(2)<<std::hex << (short)val << " ";
	        }
        }
        do_ReadSR();
    
    //}
}

void CDpCmd::RawInstructions()
{
    if(!m_vm.count("raw-instruction")) return;
    do_RawInstructions();
}

void CDpCmd::BlankCheck()
{
    if(!m_vm.count("blank")) return;
    do_BlankCheck();
}


void CDpCmd::Erase()
{
    if(!m_vm.count("erase")) return;
    do_Erase();
}

void CDpCmd::Program()
{
    if(!m_vm.count("prog")) return;
    do_Program();

}

void CDpCmd::Read()
{
    if(!m_vm.count("read")) return;
    do_Read();
    do_DisplayOrSave();
}

void CDpCmd::Auto()
{
    if(!m_vm.count("auto")) return;

    do_Auto();

}

void CDpCmd::Verify()
{
    if(!m_vm.count("verify")) return;
    do_Verify();
}


void CDpCmd::CalChecksum()
{
    if(m_vm.count("fsum")) 
    {
        do_loadFile();
        wcout << L"\nChecksum(file): " << m_proj->getImageFileInfo(DediVersion::IMAGEFILE_ATTRIBUTE_CRC);
    }

    if(m_vm.count("sum")) 
    {
        do_Read();
        if( m_vm["length"].defaulted() && m_vm["addr"].defaulted())
        {
            cout << "\nChecksum of the whole chip(address starting from: "
                << "0x" << std::hex <<  m_vm["addr"].as<string>()
                << ", 0x" <<std::hex<< m_proj->getChipSizeInBytes()
                << " bytes in total):  " 
                <<std::setfill('0')
                <<std::setw(8)<<std::hex<< crc::CRC32(m_proj->getBufferForLastReadChipContents());
        }
        else
        {
            cout << "\nChecksum(address starting from: "
                << "0x" << std::hex <<  m_vm["addr"].as<string>()
                << ", 0x" <<  m_vm["length"].as<string>()
                << " bytes in total):  " 
                <<std::setfill('0')
                <<std::setw(8)<<std::hex<< crc::CRC32(m_proj->getBufferForLastReadChipContents());
        }

    }
}

bool CDpCmd::Wait()
{
    size_t timeOut =  m_vm["timeout"].as<size_t>();

    Sleep(100);   // wait till the new thread starts ....

    // wait ...
    timer tmr;
    size_t time_stamp = static_cast<size_t>(tmr.elapsed());

    while( m_proj->is_operationOnGoing() )
    {
        if(tmr.elapsed() - time_stamp > timeOut)
        {
            OutputFormatedMsg(msg_err_timeout_abortion);

            m_status = false;
            return m_status;
        }

        // progress display
#if 0 // _DEBUG
        //static wstring s(L"start.") ;
        //wstring ss( m_proj->getStatusString());
        //if(s !=  ss)
        //{
        //    s = ss;
        wcout << tmr.elapsed()<< L"  "
            << m_proj->getStatusString() << std::endl;

        //}
#else
        if(!m_vm.count("silent"))
        {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" 
                << std::setfill(' ')
                <<std::setw(7)
                <<tmr.elapsed() 
                <<"s elapsed";
        }

#endif

    };

    // check result ...
    m_status =  m_proj->is_operationSuccessful();

    if(! m_status)
    {
        wcout << L"\n" << m_proj->getStatusString() << L"\n";
    }


    //if(m_vm.count("log"))
    //{
    //       std::wofstream f(path_util::GetPersonalAppDataPath() + L"log.txt", ios::app);
    //       f << L"operation result: " << bResult << std::endl;
    //}

    return m_status;
}

