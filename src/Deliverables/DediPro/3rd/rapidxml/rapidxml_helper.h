#pragma once

#include "numeric_conversion.h"

#include <sstream>

#include <boost/algorithm/string/predicate.hpp>

#include "rapidxml_utils.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
using namespace rapidxml;

namespace rapidxml_helper
{
    using namespace numeric_conversion;

    // getters
    namespace getter 
    {
        template<typename T>
        basic_string<T> get_string_value(xml_base<T>* p)
        {
            return p ? p->value() : basic_string<T>();
        }

        template<typename T>
        size_t get_hex_value(xml_base<T>* p)
        {
            return p ? hexstring_to_size_t(p->value()) : 0;
        }

        template<typename T>
        size_t get_dec_value(xml_base<T>* p)
        {
            return p ? decstring_to_size_t(p->value()) : 0;
        }

        template<typename T>
        bool get_bool_value(xml_base<T>* p)
        {
            string s_true("true");
            return p 
                ? boost::algorithm::iequals(
                get_string_value(p), 
                basic_string<T>().assign(s_true.begin(), s_true.end())) 
                : 0;
        }
    }


    // setters
    namespace setter
    {
        // because rapidxml only holds pointers, local variables cannot be carried out of that scope
        // some global variables are introduced as placeholders

        // for basic_string
        template<typename T>
        void set_string_value(xml_document<wchar_t>& doc, xml_base<T>* p, const basic_string<T>& v)
        {
            set_string_value(doc, p, v.c_str());
        }

        // for C-style string
        template<typename T>
        void set_string_value(xml_document<wchar_t>& doc, xml_base<T>* p, const T* v)
        {
            if(p) p->value(doc.allocate_string(v));
        }

        template<typename T, bool is_hex> 
        struct set_uint_value
        {
            void operator ()(xml_document<wchar_t>& doc, xml_base<T>* p, size_t v)
            {
                if(p)
                {
                    std::basic_stringstream<T> ss(v);
                    p->value(doc.allocate_string(ss.str().c_str()));
                }
            }
        };

        template<typename T> 
        struct set_uint_value <T, true>
        {
            void operator ()(xml_document<wchar_t>& doc, xml_base<T>* p, size_t v)
            {
                if(p)
                {
                    std::basic_stringstream<T> ss;
                    ss  << std::hex 
                        << std::showbase
                        << v;

                    p->value(doc.allocate_string(ss.str().c_str()));
                }
            }
        };


        template<typename T>
        void set_hex_value(xml_document<wchar_t>& doc, xml_base<T>* p, size_t v)
        {
            set_uint_value<T, true>()(doc, p, v);
        }

        template<typename T>
        void set_dec_value(xml_document<wchar_t>& doc, xml_base<T>* p, size_t v)
        {
            set_uint_value<T, false>()(doc, p, v);
        }

        template<typename T>
        void set_bool_value(xml_document<wchar_t>& doc, xml_base<T>* p, bool v)
        {
            if(p)
            {
                std::basic_stringstream<T> ss;
                ss << std::boolalpha;
                ss << v;


                p->value(doc.allocate_string(ss.str().c_str()));
            }
        }
    }

    using namespace getter;
    using namespace setter;

}