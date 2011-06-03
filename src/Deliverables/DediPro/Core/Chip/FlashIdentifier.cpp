/// class CFlashIdentifier

#include "memory_id.h"
#include "FlashIdentifier.h"
#include "SerialFlashInfoEx.h"
#include "ProgBoard.h"
#include "project.h"

#include "macro.h"
#include "UsbDevice.h"

#include <utility>
#include <vector>
#include <set>
#include <sstream>

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/timer.hpp>

using std::make_pair;
using std::vector;
using std::set;
using std::wstringstream;

using boost::array;

using programmer_board::CProgBoard;

CFlashIdentifier::CFlashIdentifier(CProject* project)
:m_project(project)
,m_usb(m_project->getUsbInstance())
//,m_board(board)
, m_matches()
{
    Identify();
}

bool CFlashIdentifier::get_default_match(memory::memory_id& id)
{
    if(!m_matches.empty())
    {
        id = *m_matches.begin();
        return true;
    }
    else
    {
        memory::memory_id i;
        id = i;
        return false;
    }
}

bool CFlashIdentifier::is_S25FL128P_256KSectors()
{
    power::CAutoVccPower autopower(m_usb, m_project->get_context().power.vcc);
    long long id = ReadId(boost::tuples::make_tuple(0x9F, 1, 5));

    return (0x0120180300 == id);
}



/**
* \brief
* Write brief comment for GetDefaultHandlerClassName here.
* 
* \returns
* HandlerClassName of the matching class 
*
*    CLASS     IN/OUT CODE
*   25FxxxS33   1,3   0x9F 
*   25xFxxB     1,3   0x9F
*   AT25Fxxx    1,2   0x15
*   AT26xxx     1,3   0x9F
*   F25Lxxx     1,3   0x9F
*   M25PExx     1,3   0x9F
*   M25Pxx      1,3   0x9F
*   M45PExx     1,3   0x9F
*   MX25Lxxx    1,3   0x9F
*   W25Pxx      1,3   0x9F
*   W25Xxx      1,3   0x9F
*   S25FLxxx    1,3   0x9F

*   A25Lxxx     1,4   0x9F

*   EN25Xxx     4,2   0x90 / 1,3 0x9F
*   W25Bxx      4,2   0x90

*   25xFxx      4,2   0xAB
*   25xFxxA     4,2   0xAB

*   PM25LVxxx   4,3   0xAB 
* 
* \remarks
* Write remarks for Get here.
* 
* \see
* Separate items with the '|' character.
*/
// new algorithm
// 1. if in sigle identifyings, put to the final set. --> check unique_id ONly
// 2 if in the dual identifyings for 1st time,  put to the shelf  ; if the second time, put to the final set.  --> check both unique_id and the alter_id.

void CFlashIdentifier::Identify()
{
    using boost::tuples::make_tuple;
    boost::array<boost::tuple<unsigned int, unsigned int, unsigned int>, 8> commands =
    {
        make_tuple(0x9F, 1 ,4),  /// A25Lxxx
        make_tuple(0x9F, 1, 3),  /// M25Pxx, M45PExx, M25PExx, C25xFxxB,W25P80/16/32, CW25Xxx, EN...,PM25LV080B/016B
        make_tuple(0x9F, 1 ,2),  /// AT45DB
        make_tuple(0x15, 1 ,2),   /// A25Fxxx
        make_tuple(0xAB, 4, 3),  /// PM25LVxxx
        make_tuple(0xAB, 4, 2),  /// 25xFxx, 25xFxxA,
        make_tuple(0x90, 4, 3),  /// PM25LV080B/016B
        make_tuple(0x90, 4, 2)  /// EN25Xxx, W25B40, W25P10/20/40
    };

    boost::timer t;

    power::CAutoVccPower autopower(m_usb, m_project->get_context().power.vcc);

    m_trials.clear();
    set<wstring> matchedTypes;
    set<wstring> DualIdTypes;
    size_t eId;

    ChipInfo::CChipInfo ci;


    m_matches.clear();
    set<size_t> distinct_id_set;
    set<wstring> dual_id_set;
    for(size_t i = 0; i < commands.size(); i++)
    {
        eId = ReadId(commands[i]);
        m_trials.push_back(std::make_pair(commands[i].get<0>(), eId));

        if(!distinct_id_set.insert(eId).second) continue;    // this id has been tried already

        /// algorithm description:
        /// a valid id is one that is not equal to 0 or all 1's. 
        /// - with one valid id, there could be multiple matching type names
        /// - with one type name there could be one or two ids to match, what ever the is is, 
        ///   the chip is uniquely identified by the typename
        /// - the loop goes on unless there are two ids matching the same type name. 
        /// 
        if(0 != eId && -1 != eId) // a valid id
        {
            BOOST_FOREACH(const memory::memory_id& mid, ci.get_single_identifyings())    // all m_single_identifying
            {
                if(mid.UniqueID == eId)
                {
                    m_matches.push_back(mid);    
                }
            }

            BOOST_FOREACH(const memory::memory_id& mid, ci.get_dual_identifyings())    // all m_dual_identifying
            {
                if(mid.UniqueID == eId || mid.AlternativeID == eId)
                {
                    if(dual_id_set.end() != dual_id_set.find(mid.TypeName))
                    {
                        m_matches.push_back(mid);    
                    }
                    else
                    {
                        dual_id_set.insert(mid.TypeName);
                    }
                }

            }
        }

    }

    double i = t.elapsed();
}

wstring CFlashIdentifier::get_formated_trials()
{
    typedef std::pair<unsigned int, unsigned int> P;

    wstringstream wss;
    wss << m_trials.size()<< L" trials: (in,out) = ";
    BOOST_FOREACH(P& p, m_trials)
    {
        wss << L"("<< std::hex << p.first << L"," << std::hex << p.second << L") ";
    }

    return wss.str();
}

wstring CFlashIdentifier::get_formated_matches() 
{
    wstringstream wss;
    wss << L"[ ";
    BOOST_FOREACH(const memory::memory_id& mid, m_matches)
    {
        wss << mid.TypeName << L" ";
    }
    wss << L"]";

    return wss.str();
}


long long CFlashIdentifier::ReadId(boost::tuple<unsigned int /*RDID code*/, unsigned int/*inByteCount*/, unsigned int/*outByteCount*/> command)
{
    // read status
    if(! m_usb.is_open() )
        return 0 ;

    // RDID is not decoded when in DP mode
    // so , first release from Deep Power Down mode or read signature
    // DoRDP() ;

    // send request
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(command.get<1>()) ;    //size 1

    // first control packet
    vInstruction[0] = command.get<0>() ;
    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_OUT ;
    rq.Request = TRANSCEIVE ;
    rq.Value = RFU ;
    rq.Index = RESULT_IN ;
    rq.Length = static_cast<unsigned long>(vInstruction.size()) ;

    if(! m_usb.OutCtrlRequest(rq, vInstruction))
        return 0 ;

    // second control packet : fetch data
    vector<unsigned char> vBuffer(command.get<2>()) ;

    rq.Function = URB_FUNCTION_VENDOR_ENDPOINT ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = TRANSCEIVE ;
    rq.Value = CTRL_TIMEOUT ;
    rq.Index = NO_REGISTER ;
    rq.Length = static_cast<unsigned long>(vBuffer.size()) ;

    if(! m_usb.InCtrlRequest(rq, vBuffer))
        return 0 ;

    //m_board.SetVccValue(CVppVccPower::POWEROFF);

    long long  value = vBuffer[0];
    for(size_t i = 1; i < command.get<2>(); ++i)
    {
        value <<= 8;
        value += vBuffer[i];
    }

    return value ;
}
