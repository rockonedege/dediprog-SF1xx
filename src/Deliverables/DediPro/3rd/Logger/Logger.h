#pragma once

#ifndef _LOGGER_H
#define _LOGGER_H

#include <deque>
#include <string>


using std::wstring;

wstring PrefixTimeStamp(const wstring& s);

class Logger
{
private:
    static std::deque<std::wstring> m_deque;
public:
    static void LogIt(wstring s);
    static wstring GetLog();
};

#endif // end of file

