#include "stdafx.h"            

#include "logger.h"

#include <deque>

#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

using std::wstring;

std::deque<std::wstring> Logger::m_deque;

wstring PrefixTimeStamp(const wstring& s)
{
    using namespace boost::posix_time;

    ptime t(second_clock::local_time());

    std::wstringstream wss;
    wss << to_simple_wstring(t)
        << L":    "
        << s;

    return wss.str();

    //CTime t(CTime::GetCurrentTime());
    //CString S(t.Format("%c:    "));

    //return (S + s.c_str()).GetString();
}

void Logger::LogIt(wstring s)
{
    using namespace boost::algorithm;

    trim(s);

    std::vector<std::wstring> v;
    split(v, s, is_any_of(L"\n"), token_compress_on);

    BOOST_FOREACH(wstring ws, v)
    {
        m_deque.push_back(PrefixTimeStamp(ws));
    }
}

wstring Logger::GetLog()
{
    wstring s;

    if(! m_deque.empty()) 
    {
        s = m_deque.front();
        m_deque.pop_front();
    }
    else
    {
        s.clear();
    }
    return s;
}
