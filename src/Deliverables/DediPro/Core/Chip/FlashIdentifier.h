#pragma once

#include <boost/tuple/tuple.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

class CUSB;
namespace programmer_board { class CProgBoard; }
namespace memory{ struct memory_id;}

using std::vector;
using programmer_board::CProgBoard;

class CProject;

/**
 * \brief
 * Helper class on indentifying a Serial Flash memory .
 * 
 * this class tries to read back memory ID or JEDEC ID, and returns the chip family name
 * it belongs to or the raw IDs.
 * 
 * \remarks
 * 
 * \see
 * CChipInfo
 */
class CFlashIdentifier:boost::noncopyable
{
public:
    CFlashIdentifier(CProject* project);

public:
    const vector<memory::memory_id>& get_all_matches() {return m_matches;}

    wstring get_formated_trials();
    wstring get_formated_matches();
    bool get_default_match(memory::memory_id& id);

    bool is_S25FL128P_256KSectors();


private:
    void Identify();
    long long ReadId(boost::tuple<unsigned int /*RDID code*/, unsigned int/*inByteCount*/, unsigned int/*outByteCount*/> command);

private:
    CProject*       m_project;    
    CUSB&           m_usb;      ///< reference to one and only CUSB instance
    //CProgBoard&     m_board;    ///< reference to one and only CProgBoard instance
    vector<std::pair<unsigned int, unsigned int> > m_trials;
    vector<memory::memory_id>    m_matches;
};


