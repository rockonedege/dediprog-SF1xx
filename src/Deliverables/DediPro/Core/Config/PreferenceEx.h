/// \file common

#pragma once

#include "rapidxml_helper.h"

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace Context{ struct CDediContext;} // forward declaration


/**
* \brief This class parses the chip config. file.
*/

namespace PreferenceEx 
{
    using Context::CDediContext;
    using rapidxml::xml_document;

    using std::vector;
    using std::wstring;

    // base class
    class CPreference
    {
    public:
        CPreference();
    protected:
        bool m_status;
        std::wstring m_filepath;

    public:
        bool is_good() const { return m_status; } ;
    };

    // reader class
    class CPreferenceReader : CPreference
    {
    public:
        CPreferenceReader();
    public:
        const vector<CDediContext>& get_context_set(){ return m_contexts; }
    private:
        bool open_file();

    private:
        vector<CDediContext> m_contexts;
        xml_document<wchar_t> m_doc;
    };

    // writer class
    class CPreferenceWriter : boost::noncopyable, CPreference
    {
    public:
        bool persist_contexts(const vector<CDediContext>& contexts);
        bool persist_contexts(const CDediContext& context);

    private:
        void persist_context_node(const CDediContext& context, xml_node<wchar_t> *root, xml_document<wchar_t>& doc);
    };



}