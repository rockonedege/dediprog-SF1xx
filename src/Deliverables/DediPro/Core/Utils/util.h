#pragma once

#include <vector>
#include <string>

using std::string;
using std::wstring;
using std::vector;

namespace crc
{
    extern "C"  unsigned int CRC32(vector<unsigned char> v);
    wstring CRC32InStringFormat(vector<unsigned char> v);

}
namespace path_util
{
wstring GetPersonalAppDataPath();
wstring GetCommonAppDataPath();
wstring GetCurrentExeDir();
wstring GetCurrentExePath();
}


//size_t HexCString_to_Size_t(CString& s);
//
//
//template <typename T>
//size_t string_to_Size_t(const std::basic_string<T>& s, bool is_hex)
//{
//	std::basic_istringstream<T> ss(s, std::ios_base::in);
//
//    size_t size;
//	ss >> (is_hex ? std::hex : std::dec )>>size;
//    return size;
//}
//
//template <typename T>
//size_t Hexstring_to_Size_t(const std::basic_string<T>& s)
//{
//    return string_to_Size_t<T>(s, true);
//}
//
//template <typename T>
//size_t Decwstring_to_Size_t(const std::basic_string<T>& s)
//{
//    return string_to_Size_t<T>(s, false);
//}
