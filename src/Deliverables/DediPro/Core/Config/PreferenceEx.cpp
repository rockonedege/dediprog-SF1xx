#include "PreferenceEx.h"
#include "DediContext.h"
#include "util.h"

#include <boost/foreach.hpp>
#include <Windows.h>
#include <boost\filesystem.hpp>


#include <iostream>
using std::wcout;

#ifdef _DEDI_CONSOLE // for dpcmd
#define USER_PREFERENCE_ROOT L"CONSOLE_USERS"
#else       // for dediprog
#define USER_PREFERENCE_ROOT L"GUI_USERS"
#endif

namespace PreferenceEx
{
    using Context::CDediContext;
    using Context::STARTUP_MODE;
    using Context::INVOKE_FROM;

    // attrib names
    const wchar_t* StartupMode = L"StartupMode";
    const wchar_t* CurrentFileInBuffer = L"CurrentFileInBuffer";
    const wchar_t* CurrentFileInBufferFormat = L"CurrentFileInBufferFormat";
    const wchar_t* LastLoadedFile = L"LastLoadedFile";
    const wchar_t* LastLoadedFileFormat = L"LastLoadedFileFormat";

    const wchar_t* Prog_PaddingCharacterWhenProgrammingWholeChip = L"Prog_PaddingCharacterWhenProgrammingWholeChip";
    const wchar_t* Prog_PartialProgram = L"Prog_PartialProgram";
    const wchar_t* Prog_CustomizedAddrFrom = L"Prog_CustomizedAddrFrom";
    const wchar_t* Prog_CustomizedDataLength = L"Prog_CustomizedDataLength";

    const wchar_t* Batch_SelectedOption = L"Batch_SelectedOption";
    const wchar_t* Batch_PartialProgram = L"Batch_PartialProgram";
    const wchar_t* Batch_VerifyAfterCompletion = L"Batch_VerifyAfterCompletion";
    const wchar_t* Batch_ReloadFile = L"Batch_ReloadFile";
    const wchar_t* Batch_PaddingCharacterWhenDownloadWithBlankCheck = L"Batch_PaddingCharacterWhenDownloadWithBlankCheck";
    const wchar_t* Batch_PaddingCharacterWhenDownloadWithNoBlankCheck = L"Batch_PaddingCharacterWhenDownloadWithNoBlankCheck";
    const wchar_t* Batch_CustomizedAddrFrom = L"Batch_CustomizedAddrFrom";
    const wchar_t* Batch_CustomizedDataLength = L"Batch_CustomizedDataLength";

    const wchar_t* Read_CustomizedAddrFrom = L"Read_CustomizedAddrFrom";
    const wchar_t* Read_CustomizedDataLength = L"Read_CustomizedDataLength";

    const wchar_t* FlashCard_Option = L"FlashCard_Option";
    const wchar_t* FlashCard_TFITFileForV116Below = L"FlashCard_TFITFileForV116Below";
    const wchar_t* FlashCard_TFITFile = L"FlashCard_TFITFile";
    const wchar_t* FlashCard_OptionIndex = L"FlashCard_OptionIndex";
    const wchar_t* FlashCard_UsingVpp = L"FlashCard_UsingVpp";
    const wchar_t* FlashCard_TargetSizeInBytes = L"FlashCard_TargetSizeInBytes";
    const wchar_t* FlashCard_TargetType = L"FlashCard_TargetType";
    const wchar_t* FlashCard_ComboSelection = L"FlashCard_ComboSelection";

    const wchar_t* InvokeFrom = L"InvokeFrom";
    const wchar_t* SavedPaths = L"SavedPaths";

    const wchar_t* Misc_ApplyVpp = L"Misc_ApplyVpp";
    const wchar_t* Misc_Vcc = L"Misc_Vcc";

    const wchar_t* MCF_CR = L"MCF_CR";

    void createFileIfNotExist()
    {
        _wmkdir(path_util::GetPersonalAppDataPath().c_str());
        ::CopyFile(
            (path_util::GetCommonAppDataPath() + L"DediPreference.xml").c_str(), 
            (path_util::GetPersonalAppDataPath() + L"DediPreference.xml").c_str(),
            true);
    }

    // base class
    CPreference::CPreference()
        :m_status(false)
        ,m_filepath(path_util::GetPersonalAppDataPath() + L"DediPreference.xml")
        //,m_filepath(L"d:/c.xml")
    {
    }

    // reader class
    CPreferenceReader::CPreferenceReader()
        :CPreference()
    {
        createFileIfNotExist();
        m_status = open_file();
    }

    bool CPreferenceReader::open_file()
    {
        using namespace rapidxml_helper;
        try
        {
            //xml_document<wchar_t> doc;
            file<wchar_t> xml(m_filepath.c_str());
            m_doc.parse<parse_comment_nodes>(xml.data());

            xml_node<wchar_t> *root = m_doc.first_node();
            if(root)
            {
                node_iterator<wchar_t> itr_end;
                for(node_iterator<wchar_t> itr(root); itr != itr_end; ++itr)
                {
                    CDediContext context;

                    context.id = itr->name();

                    context.StartupMode = (STARTUP_MODE)get_hex_value(itr->first_attribute(StartupMode,0,false));
                    context.file.CurrentFileInBuffer = get_string_value(itr->first_attribute(CurrentFileInBuffer,0,false));
                    context.file.CurrentFileInBufferFormat = get_string_value(itr->first_attribute(CurrentFileInBufferFormat,0,false));
                    context.file.LastLoadedFile = get_string_value(itr->first_attribute(LastLoadedFile,0,false));
                    context.file.LastLoadedFileFormat = get_string_value(itr->first_attribute(LastLoadedFileFormat,0,false));

                    context.Prog_PaddingCharacterWhenProgrammingWholeChip = get_hex_value(itr->first_attribute(Prog_PaddingCharacterWhenProgrammingWholeChip,0,false));
                    context.Prog_CustomizedAddrFrom = get_hex_value(itr->first_attribute(Prog_CustomizedAddrFrom,0,false));
                    context.Prog_CustomizedDataLength = get_hex_value(itr->first_attribute(Prog_CustomizedDataLength,0,false));
                    context.Prog_PartialProgram = get_bool_value(itr->first_attribute(Prog_PartialProgram,0,false));

                    context.Batch_SelectedOption = get_hex_value(itr->first_attribute(Batch_SelectedOption,0,false));
                    context.Batch_VerifyAfterCompletion = get_bool_value(itr->first_attribute(Batch_VerifyAfterCompletion,0,false));
                    context.Batch_ReloadFile = get_bool_value(itr->first_attribute(Batch_ReloadFile,0,false));
                    context.Batch_PartialProgram = get_bool_value(itr->first_attribute(Batch_PartialProgram,0,false));
                    context.Batch_PaddingCharacterWhenDownloadWithBlankCheck = get_hex_value(itr->first_attribute(Batch_PaddingCharacterWhenDownloadWithBlankCheck,0,false));
                    context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck = get_hex_value(itr->first_attribute(Batch_PaddingCharacterWhenDownloadWithNoBlankCheck,0,false));
                    context.Batch_CustomizedAddrFrom = get_hex_value(itr->first_attribute(Batch_CustomizedAddrFrom,0,false));
                    context.Batch_CustomizedDataLength = get_hex_value(itr->first_attribute(Batch_CustomizedDataLength,0,false));
                    context.Read_CustomizedAddrFrom = get_hex_value(itr->first_attribute(Read_CustomizedAddrFrom,0,false));
                    context.Read_CustomizedDataLength = get_hex_value(itr->first_attribute(Read_CustomizedDataLength,0,false));

                    context.flashcard.FlashCard_Option = get_hex_value(itr->first_attribute(FlashCard_Option,0,false));
                    context.flashcard.FlashCard_OptionIndex = get_hex_value(itr->first_attribute(FlashCard_OptionIndex,0,false));
                    context.flashcard.FlashCard_TargetSizeInBytes = get_hex_value(itr->first_attribute(FlashCard_TargetSizeInBytes,0,false));
                    context.flashcard.FlashCard_TargetType = get_string_value(itr->first_attribute(FlashCard_TargetType,0,false));
                    context.flashcard.FlashCard_TFITFileForV116Below = get_string_value(itr->first_attribute(FlashCard_TFITFileForV116Below,0,false));
                    context.flashcard.FlashCard_TFITFile = get_string_value(itr->first_attribute(FlashCard_TFITFile,0,false));

                    context.flashcard.FlashCard_ComboSelection = get_hex_value(itr->first_attribute(FlashCard_ComboSelection,0,false));
                    context.flashcard.FlashCard_UsingVpp = get_bool_value(itr->first_attribute(FlashCard_UsingVpp,0,false));

                    context.runtime.InvokeFrom = (INVOKE_FROM)get_hex_value(itr->first_attribute(InvokeFrom,0,false));
                    context.SavedPaths = get_string_value(itr->first_attribute(SavedPaths,0,false));
                    context.power.Misc_ApplyVpp = get_bool_value(itr->first_attribute(Misc_ApplyVpp,0,false));
                    context.power.vcc = (power::VCC_VALUE)get_hex_value(itr->first_attribute(Misc_Vcc,0,false));

                    context.chip.mcf_config_register = get_hex_value(itr->first_attribute(MCF_CR,0,false));

                    m_contexts.push_back(context);

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


    // writer class
    bool CPreferenceWriter::persist_contexts(const CDediContext& context)
    {
        vector<CDediContext> contexts;
        contexts.push_back(context);
        return persist_contexts(contexts);
    }

    bool CPreferenceWriter::persist_contexts(const vector<CDediContext>& contexts)
    {
        try
        {
            xml_document<wchar_t> doc;
            file<wchar_t> xml(m_filepath.c_str());
            doc.parse<parse_comment_nodes|parse_no_utf8>(xml.data());

            xml_node<wchar_t> *root = doc.first_node();
            if(root)
            {
                node_iterator<wchar_t> itr_end;
                node_iterator<wchar_t> itr(root);

                BOOST_FOREACH(const CDediContext& ct, contexts)
                {
                    for( itr; itr != itr_end; ++itr)
                    {
                        if(boost::algorithm::iequals(ct.id, itr->name()))
                        {
                            persist_context_node(ct, &(*itr), doc);
                        }
                    }
                }
            }

            wofstream of(m_filepath.c_str()/*L"d:/c.xml"*/, ios::binary);
            IMBUE_NULL_CODECVT(of);

            of << doc;
        }
        catch(...)
        {
            m_status = false;
            return false;
        }

        return true;
    }

    void CPreferenceWriter::persist_context_node(const CDediContext& context, xml_node<wchar_t> *context_node, xml_document<wchar_t>& doc)
    {
        using namespace rapidxml_helper;

        if(boost::algorithm::iequals(context.id,  context_node->name()))
        {
            set_hex_value(doc, context_node->first_attribute(StartupMode,0,false), context.StartupMode);

            set_string_value(doc, context_node->first_attribute(CurrentFileInBuffer,0,false), context.file.CurrentFileInBuffer);
            set_string_value(doc, context_node->first_attribute(CurrentFileInBufferFormat,0,false), context.file.CurrentFileInBufferFormat);
            set_string_value(doc, context_node->first_attribute(LastLoadedFile,0,false), context.file.LastLoadedFile);

            set_string_value(doc, context_node->first_attribute(LastLoadedFileFormat,0,false), context.file.LastLoadedFileFormat);

            set_hex_value(doc, context_node->first_attribute(Prog_PaddingCharacterWhenProgrammingWholeChip,0,false), context.Prog_PaddingCharacterWhenProgrammingWholeChip);
            set_hex_value(doc, context_node->first_attribute(Prog_CustomizedAddrFrom,0,false), context.Prog_CustomizedAddrFrom);
            set_hex_value(doc, context_node->first_attribute(Prog_CustomizedDataLength,0,false), context.Prog_CustomizedDataLength);
            set_hex_value(doc, context_node->first_attribute(Prog_CustomizedDataLength,0,false), context.Prog_CustomizedDataLength);
            set_bool_value(doc, context_node->first_attribute(Prog_PartialProgram,0,false), context.Prog_PartialProgram);

            set_hex_value(doc, context_node->first_attribute(Batch_SelectedOption,0,false), context.Batch_SelectedOption);
            set_bool_value(doc, context_node->first_attribute(Batch_VerifyAfterCompletion,0,false), context.Batch_VerifyAfterCompletion);
            set_bool_value(doc, context_node->first_attribute(Batch_ReloadFile,0,false), context.Batch_ReloadFile);
            set_bool_value(doc, context_node->first_attribute(Batch_PartialProgram,0,false), context.Batch_PartialProgram);
            set_hex_value(doc, context_node->first_attribute(Batch_PaddingCharacterWhenDownloadWithBlankCheck,0,false), context.Batch_PaddingCharacterWhenDownloadWithBlankCheck);
            set_hex_value(doc, context_node->first_attribute(Batch_PaddingCharacterWhenDownloadWithNoBlankCheck,0,false), context.Batch_PaddingCharacterWhenDownloadWithNoBlankCheck);
            set_hex_value(doc, context_node->first_attribute(Batch_CustomizedAddrFrom,0,false), context.Batch_CustomizedAddrFrom);
            set_hex_value(doc, context_node->first_attribute(Batch_CustomizedDataLength,0,false), context.Batch_CustomizedDataLength);
            set_hex_value(doc, context_node->first_attribute(Read_CustomizedAddrFrom,0,false), context.Read_CustomizedAddrFrom);
            set_hex_value(doc, context_node->first_attribute(Read_CustomizedDataLength,0,false), context.Read_CustomizedDataLength);

            set_hex_value(doc, context_node->first_attribute(FlashCard_Option,0,false), context.flashcard.FlashCard_Option);
            set_hex_value(doc, context_node->first_attribute(FlashCard_OptionIndex,0,false), context.flashcard.FlashCard_OptionIndex);
            set_hex_value(doc, context_node->first_attribute(FlashCard_TargetSizeInBytes,0,false), context.flashcard.FlashCard_TargetSizeInBytes);
            set_string_value(doc, context_node->first_attribute(FlashCard_TargetType,0,false), context.flashcard.FlashCard_TargetType);
            set_string_value(doc, context_node->first_attribute(FlashCard_TFITFileForV116Below,0,false), context.flashcard.FlashCard_TFITFileForV116Below);
            set_string_value(doc, context_node->first_attribute(FlashCard_TFITFile,0,false), context.flashcard.FlashCard_TFITFile);

            set_hex_value(doc, context_node->first_attribute(FlashCard_ComboSelection,0,false), context.flashcard.FlashCard_ComboSelection);
            set_bool_value(doc, context_node->first_attribute(FlashCard_UsingVpp,0,false), context.flashcard.FlashCard_UsingVpp);

            set_hex_value(doc, context_node->first_attribute(InvokeFrom,0,false), context.runtime.InvokeFrom);
            set_string_value(doc, context_node->first_attribute(SavedPaths,0,false), context.SavedPaths);
            set_bool_value(doc, context_node->first_attribute(Misc_ApplyVpp,0,false), context.power.Misc_ApplyVpp);
            set_hex_value(doc, context_node->first_attribute(Misc_Vcc,0,false), context.power.vcc);

            set_hex_value(doc, context_node->first_attribute(MCF_CR,0,false), context.chip.mcf_config_register);
        }
    }


} // end of namespace


//void CPreferenceReader::init()
//{
//    wstringstream ss;
//    wifstream xmlFile(m_filepath, ios::binary); /// UNICODE files must be open as binary
//    IMBUE_NULL_CODECVT(xmlFile);

//    if(xmlFile.is_open()) 
//    {
//        ss << xmlFile.rdbuf();
//    }
//    xmlFile.close() ;

//    //// open chip info xml
//    CString s(ss.str().c_str());
//    m_Doc.SetDoc(CString(ss.str().c_str())) ;

//    if(m_Doc.IsWellFormed()) m_bStatus = true;
//}

//bool CPreferenceReader::getBool(const CString& AttributeName)
//{
//    return (L"Yes" == GetUsersValue(AttributeName)) ? true:false; 
//}

//void CPreferenceReader::setBool(const CString& AttributeName, bool b)
//{
//    SetUsersValue(AttributeName, b ? L"Yes" : L"No");
//}

//size_t CPreferenceReader::getIntHex(const CString& AttributeName)
//{
//   return HexCString_to_Size_t( GetUsersValue(AttributeName));
//}
//void CPreferenceReader::setInt(const CString& AttributeName, size_t i)
//{
//    std::wstringstream wss;
//    wss << std::hex << i;
//    SetUsersValue(AttributeName, wss.str().c_str());
//}


//CString CPreferenceReader::getString(const CString& AttributeName)
//{
//    return GetUsersValue(AttributeName);
//}
//void CPreferenceReader::setString(const CString& name, const CString& value)
//{
//    SetUsersValue(name, value);
//}


//CString CPreferenceReader::GetUsersValue(const CString& AttributeName) 
//{
//    m_Doc.ResetPos();
//    if( ! m_Doc.FindElem(L"Preferences") )      return L"";
//    m_Doc.IntoElem() ;

//    if( !m_Doc.FindElem(USER_PREFERENCE_ROOT) )             return L"";

//    return m_Doc.GetAttrib(AttributeName);
//}

//void CPreferenceReader::SetUsersValue(const CString& Name, const CString& Value)
//{
//    m_Doc.ResetPos();
//    if( ! m_Doc.FindElem(L"Preferences") )      return;
//    m_Doc.IntoElem() ;

//    if( !m_Doc.FindElem(USER_PREFERENCE_ROOT) )             return;
//    m_Doc.SetAttrib(Name, Value);

//    Persist();
//}

//void CPreferenceReader::Persist()
//{
//    if(! m_bStatus) return;

//    wofstream of(m_filepath.GetString(), ios::binary); /// UNICODE files must be open as binary
//    IMBUE_NULL_CODECVT(of);
//    if(of)
//    {
//        if((!m_Doc.GetDoc().IsEmpty()) && m_Doc.IsWellFormed())    of << m_Doc.GetDoc().GetString() ;
//    }
//}
