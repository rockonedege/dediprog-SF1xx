
#ifndef _CHIPINFO_H
#define _CHIPINFO_H

#include "memory_id.h"

#include <string>
using std::wstring;
#include <vector>
using std::vector ;

#include <boost/utility.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace ChipInfo
{
    using memory::memory_id;

    namespace AT45DB
    {
        bool IsAT45DB(const memory_id& id);
        wstring get_alter_chipsize_for_AT45(const memory_id& id);
    }

    /**
    * \brief
    * chip information class 
    * 
    * this class is constructed with a valid path to a memory chip file.
    * it parses the file in the constructor.
    * 
    * \remarks
    * before calling any other method, be sure to call is_good() to check the wellness  
    *  which indicates whether the instance is constructed successfully or not
    *
    * \see
    * \c bool CChipInfo::is_good()
    */
    class CChipInfo:boost::noncopyable
    {
    public:
        const vector<memory_id>& get_single_identifyings() { return m_single_identifying; }
        const vector<memory_id>& get_dual_identifyings() { return m_dual_identifying; }

    private:
        bool open_db();

    public :
        CChipInfo() ;

        bool is_good()    { return m_status;};   
    private:
        bool m_status;
        wstring m_path;

        vector<memory_id> m_single_identifying;
        vector<memory_id> m_dual_identifying;

    };

    class CSortedChipList
    {
    public:
        CSortedChipList();
    public:
        const vector<memory_id>& get_supported_chiplist_ordered_by_typename(); 
        const vector<memory_id>& get_supported_chiplist_ordered_by_manufacturer();

    private:

        struct CompareByTypeName
        {
            bool operator()(const memory_id& a, const memory_id& b)
            {
                return boost::algorithm::ilexicographical_compare(a.TypeName, b.TypeName);

            }

        };

        struct CompareByManufacturer
        {
            bool operator()(const memory_id& a, const memory_id& b)
            {
                return boost::algorithm::ilexicographical_compare(a.Manufacturer, b.Manufacturer);
            }
        };


    private:
        vector<memory_id> m_list;

    };

}// end of namespace ChipInfo

#endif    //_CHIP_H