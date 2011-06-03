#include "SerialFlashInfoEx.h"

#include "numeric_conversion.h"
#include "rapidxml_helper.h"
#include "util.h"

#include <boost/lexical_cast.hpp>

#include "AT45DBxxxD.h"
#include "AT45DBxxxB.h"

namespace ChipInfo
{
    using memory::memory_id;

    const wchar_t* tagTypeName = L"TypeName";
    const wchar_t* tagUniqueID =   L"UniqueID";
    const wchar_t* tagClass = L"Class";
    const wchar_t* tagDescription = L"Description";
    const wchar_t* tagManufacturer = L"Manufacturer";
    const wchar_t* tagManufactureUrl = L"ManufactureUrl";

    const wchar_t* tagVoltage =  L"Voltage";
    const wchar_t* tagClock =   L"Clock";

    const wchar_t* tagManufactureID =  L"ManufactureID";
    const wchar_t* tagJedecDeviceID =   L"JedecDeviceID";
    const wchar_t* tagAlternativeID =   L"AlternativeID";

    const wchar_t* tagChipSizeInKByte =   L"ChipSizeInKByte";
    const wchar_t* tagSectorSizeInByte =  L"SectorSizeInByte"; 
    const wchar_t* tagPageSizeInByte = L"PageSizeInByte";
    const wchar_t* tagBlockSizeInByte =   L"BlockSizeInByte";

    const wchar_t* tagAddrWidth =    L"AddrWidth";

    const wchar_t* tagPageEraseSupport = L"PageEraseSupport";
    const wchar_t* tagPageWriteSupport = L"PageWriteSupport";
    const wchar_t* tagPageProgramSupport =  L"PageProgramSupport";
    const wchar_t* tagSectorEraseSupport =  L"SectorEraseSupport";
    const wchar_t* tagBlockEraseSupport =   L"BlockEraseSupport";
    const wchar_t* tagAAISingleByteProgramSupport =  L"AAISingleByteProgramSupport";
    const wchar_t* tagAAIWordProgramSupport =   L"AAIWordProgramSupport"; 
    const wchar_t* tagDeepPowerDown =       L"DeepPowerDown";
    const wchar_t* tagDualID =         L"DualID";
    const wchar_t* tagVppSupport =         L"VppSupport";



    namespace AT45DB
    {
        //enum
        //{
        //    AT45DB011D = 0x1F22,
        //    AT45DB021D = 0x1F23,
        //    AT45DB041D = 0x1F24,
        //    AT45DB081D = 0x1F25,
        //    AT45DB161D = 0x1F26,
        //    AT45DB321D = 0x1F27,
        //    AT45DB642D = 0x1F28
        //};

        bool IsAT45DB(const memory_id& id) 
        {

            //size_t eId = id.UniqueID;
            //return (    AT45DB011D == eId ||
            //    AT45DB021D == eId ||
            //    AT45DB041D == eId ||
            //    AT45DB081D == eId ||
            //    AT45DB161D == eId ||
            //    AT45DB321D == eId ||
            //    AT45DB642D == eId);
            return (boost::algorithm::iequals(id.Class, CAT45DBxxxD::getClassName())
                || boost::algorithm::iequals(id.Class, CAT45DBxxxB::getClassName()));

        }


        wstring get_alter_chipsize_for_AT45(const memory_id& id)
        {
            if(!IsAT45DB(id)) return L"";

            size_t alter_size(id.ChipSizeInByte);

            alter_size += (id.ChipSizeInByte>>5); // i.e ChipSizeInByte * ((256 + 8)/256) 

            wstring s(L"/");
            s +=  boost::lexical_cast<wstring>(alter_size);

            return s;
        }

    }


    CChipInfo::CChipInfo()
        :m_status(false),
        m_path(path_util::GetCommonAppDataPath() +L"ChipInfoDb.dedicfg")
    {
        m_status = open_db();
    }

    /**
    * \brief
    *  open the xml database, 
    * 
    * \returns
    * false if failing to open or parse; true otherwise
    * 
    */
    bool CChipInfo::open_db()
    {
        using namespace rapidxml_helper;

        try
        {
            xml_document<wchar_t> doc;
            file<wchar_t> xml(m_path.c_str());
            doc.parse<0>(xml.data());

            xml_node<wchar_t> *root = doc.first_node();
            if(root)
            {
                m_single_identifying.clear();
                m_dual_identifying.clear();

                memory_id id;
                node_iterator<wchar_t> itr_end;
                for(node_iterator<wchar_t> itr(root->first_node()); itr != itr_end; ++itr)
                {
                    id.TypeName = get_string_value(itr->first_attribute(tagTypeName,0,false));
                    id.UniqueID = get_hex_value(itr->first_attribute(tagUniqueID,0,false));
                    id.Class = get_string_value(itr->first_attribute(tagClass,0,false));
                    id.Description = get_string_value(itr->first_attribute(tagDescription,0,false));

                    id.Manufacturer = get_string_value(itr->first_attribute(tagManufacturer,0,false));
                    id.ManufactureUrl = get_string_value(itr->first_attribute(tagManufactureUrl,0,false));
                    id.Voltage = get_string_value(itr->first_attribute(tagVoltage,0,false));
                    id.Clock = get_string_value(itr->first_attribute(tagClock,0,false));




                    id.ManufactureID = get_hex_value(itr->first_attribute(tagManufactureID,0,false));
                    id.JedecDeviceID = get_hex_value(itr->first_attribute(tagJedecDeviceID,0,false));
                    id.AlternativeID = get_hex_value(itr->first_attribute(tagAlternativeID,0,false));

                    id.ChipSizeInByte = get_dec_value(itr->first_attribute(tagChipSizeInKByte,0,false)) << 10;
                    id.SectorSizeInByte = get_dec_value(itr->first_attribute(tagSectorSizeInByte,0,false));
                    id.PageSizeInByte = get_dec_value(itr->first_attribute(tagPageSizeInByte,0,false));
                    id.BlockSizeInByte = get_dec_value(itr->first_attribute(tagBlockSizeInByte,0,false));

                    id.AddrWidth = get_dec_value(itr->first_attribute(tagAddrWidth,0,false));

                    id.PageEraseSupport = get_bool_value(itr->first_attribute(tagPageEraseSupport,0,false));
                    id.PageWriteSupport = get_bool_value(itr->first_attribute(tagPageWriteSupport,0,false));
                    id.PageProgramSupport = get_bool_value(itr->first_attribute(tagPageProgramSupport,0,false));
                    id.SectorEraseSupport = get_bool_value(itr->first_attribute(tagSectorEraseSupport,0,false));
                    id.BlockEraseSupport = get_bool_value(itr->first_attribute(tagBlockEraseSupport,0,false));
                    id.AAISingleByteProgramSupport = get_bool_value(itr->first_attribute(tagAAISingleByteProgramSupport,0,false));
                    id.AAIWordProgramSupport = get_bool_value(itr->first_attribute(tagAAIWordProgramSupport,0,false));
                    id.DeepPowerDown = get_bool_value(itr->first_attribute(tagDeepPowerDown,0,false));

                    id.VppSupport = get_dec_value(itr->first_attribute(tagVppSupport,0,false));

                    id.DualID = get_bool_value(itr->first_attribute(tagDualID,0,false));

                    ( id.DualID) ? m_dual_identifying.push_back(id) : m_single_identifying.push_back(id);
                }
            }
        }
        catch(...)
        {
            m_status = false;
            return false;
        }

        return true;

    }



    // CSortedChipList
    CSortedChipList::CSortedChipList()
        : m_list()
    {
        CChipInfo info;

        m_list = info.get_single_identifyings();
        m_list.insert(m_list.end(),info.get_dual_identifyings().begin(), info.get_dual_identifyings().end());
    }

    const vector<memory_id>& CSortedChipList::get_supported_chiplist_ordered_by_typename()
    {
        sort(m_list.begin(), m_list.end(), CompareByTypeName());

        return m_list;

    }

    const vector<memory_id>& CSortedChipList::get_supported_chiplist_ordered_by_manufacturer()
    {
        sort(m_list.begin(), m_list.end(), CompareByManufacturer());

        return m_list;

    }



} // end of namespace
