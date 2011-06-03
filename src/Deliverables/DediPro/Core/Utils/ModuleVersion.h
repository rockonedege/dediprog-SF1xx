#pragma once

#include <string>



#include <shlwapi.h>			// DllGetVersion definitions
#pragma comment(linker, "/defaultlib:version.lib")    // link with version.lib for VerQueryValue, etc.


namespace DediVersion
{


    //////////////////
    // CModuleVersion provides easy access to read version info from module's
    // resource file. For example, to get the VERSIONINFO in the current running
    // module:
    //
    //		CModuleVersion ver;
    //		if (ver.dwProductVersionMS>2) {
    //			CString s = ver.GetValue(_T("CompanyName"));
    //			...
    //		}
    //
    // You can also read the version info for another file like so:
    //
    //		CModuleVersion ver("foo.exe");
    //		...
    //
    // You can also call the static fn DllGetVersion to get the DLLVERSIONINFO.
    //

    class CModuleVersion : public VS_FIXEDFILEINFO 
    {
    protected:
        BYTE* m_pVersionInfo;	// all version info

        struct TRANSLATION {
            WORD langID;			// language ID
            WORD charset;			// character set (code page)
        } m_translation;

    public:
        CModuleVersion(HMODULE hModule=NULL);
        CModuleVersion(LPCTSTR modulename);
        virtual ~CModuleVersion();

        BOOL	  GetFileVersionInfo(HMODULE hModule=NULL);
        std::wstring GetValue(LPCTSTR lpKeyName);
        //static BOOL DllGetVersion(LPCTSTR modulename, DLLVERSIONINFO& dvi);
    };


    typedef enum
    {
        IMAGEFILE_ATTRIBUTE_NAME,
        IMAGEFILE_ATTRIBUTE_CRC,
        IMAGEFILE_ATTRIBUTE_SIZE,

        IMAGEFILE_ATTRBUTE_COUNT_OF_SHOWNITEMS,

        IMAGEFILE_ATTRIBUTE_CRC_OF_WRITTEN_PART,
        IMAGEFILE_ATTRIBUTE_PATH
    } IMAGEFILE_ATTRBUTES;

    bool isFinalRelease();
    std::wstring GetDediproVersion(bool);
}
