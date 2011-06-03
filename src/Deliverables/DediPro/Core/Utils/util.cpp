#include "stdafx.h"
#include "util.h"

#include "shlobj.h"
#include <boost/crc.hpp>

#include <iomanip>
#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <sstream>

using std::for_each;
using std::wstringstream;

namespace crc
{
    extern "C" unsigned int CRC32(vector<unsigned char> v)
    {
        //boost::crc_optimal<16, 0x1021, 0xFFFF, 0, false, false>  crc_ccitt;
        //crc_ccitt = std::for_each( v.begin(), v.end(), crc_ccitt );

        // return crc_ccitt.checksum();
        unsigned int checksum = 0;
        std::for_each( v.begin(), v.end(), checksum += boost::lambda::_1 );
        return checksum & 0xFFFF;

    }

    wstring CRC32InStringFormat(vector<unsigned char> v)
    {
        std::wstringstream ss;

        if(v.empty())
        {
            ss<<L"<empty>";
        }
        else
        {
            ss<<std::setfill(L'0')<<std::uppercase<<std::setw(8)<<std::hex<< CRC32(v);
        }

        return ss.str();
    }
}

namespace path_util
{
    wstring GetSpecialPath(int csidl)
    {
        wstring s(L"\\Dediprog\\");
        TCHAR csPath[_MAX_PATH];

        //if(Is_Vista_or_Later())
        //{
        //    //if(SUCCEEDED(SHGetKnownFolderPath( 
        //    //    CSIDL_APPDATA, 
        //    //    0, 
        //    //    NULL, 
        //    //    csPath))) 
        //    //{
        //    //    return csPath + s; 
        //    //}
        //}
        //else
        {
            if(SUCCEEDED(SHGetFolderPath(NULL, 
                csidl, 
                NULL, 
                0, 
                csPath))) 
            {
                return csPath + s; 
            }
        }

        return L"." + s;
    
    }


    wstring GetPersonalAppDataPath()
    {
        return GetSpecialPath(CSIDL_APPDATA);

        //CString s(L"\\Dediprog\\");
        //TCHAR csPath[_MAX_PATH];

        //if(SUCCEEDED(SHGetFolderPath(NULL, 
        //    CSIDL_APPDATA, 
        //    NULL, 
        //    0, 
        //    csPath))) 
        //{
        //    return csPath + s; 
        //}

        //return L"." + s;

    }

    wstring GetCommonAppDataPath()
    {
        return GetSpecialPath(CSIDL_COMMON_APPDATA);
    }


    wstring GetCurrentExePath()
    {
        TCHAR csPath[_MAX_PATH];
        ::GetModuleFileName(NULL/*AfxGetInstanceHandle()*/, csPath, _MAX_PATH);

        return csPath;
    }

    wstring GetCurrentExeDir()
    {
        wstring curpath(GetCurrentExePath());
        return curpath.substr(0, curpath.rfind(L'\\') + 1);//csPath.Left(csPath.ReverseFind('\\')) + L"\\" ; 
    }

}

