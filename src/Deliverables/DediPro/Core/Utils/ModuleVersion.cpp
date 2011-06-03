#include "ModuleVersion.h"

#include <sstream>
#include <iomanip>
#include <boost/algorithm/string/replace.hpp>

#include "util.h"

namespace DediVersion
{
using std::wstringstream;
using std::wstring;

std::wstring GetDediproVersion( bool bDetailed)
    {
        CModuleVersion ver(path_util::GetCurrentExePath().c_str());
        std::wstring moduleVer(ver.GetValue(L"SpecialBuild"));
        moduleVer += L" ";
        moduleVer += ver.GetValue(L"FileVersion");

        boost::algorithm::replace_all(moduleVer, L", ", L".");
        //moduleVer.replace(',','.');

        if(bDetailed)
        {
            moduleVer += L"\r\nLast Built on ";
            std::string s(__DATE__ "\r\n");
            moduleVer += std::wstring().assign(s.begin(), s.end());

            moduleVer += ver.GetValue(L"LegalCopyright") + L"\r\n";
            moduleVer += ver.GetValue(L"LegalTrademarks");

        }

        return moduleVer;
    }

    bool isFinalRelease()
    {
        CModuleVersion ver(path_util::GetCurrentExePath().c_str());

        return ver.GetValue(L"PrivateBuild").empty();

    }

    //////////////////////////////// CModuleVersion ////////////////////////////////
    ///////// this class is written by Paul DiLascia //////////////////////////////

    CModuleVersion::CModuleVersion(HMODULE hModule) : m_pVersionInfo(NULL)
    {
        GetFileVersionInfo(hModule);
    }

    CModuleVersion::CModuleVersion(LPCTSTR modulename) : m_pVersionInfo(NULL)
    {
        if (modulename) 
        {
            //CLoadLibrary lib(modulename);
            //if (lib) 
            {
                //GetFileVersionInfo(lib);
                GetFileVersionInfo(LoadLibrary(modulename));
            }
        }
    }

    //////////////////
    // Destroy: delete version info
    //
    CModuleVersion::~CModuleVersion()
    {
        delete [] m_pVersionInfo;
    }

    //////////////////
    // Get file version info for a given module
    // Allocates storage for all info, fills "this" with
    // VS_FIXEDFILEINFO, and sets code page.
    //
    BOOL CModuleVersion::GetFileVersionInfo(HMODULE hModule)
    {
        m_translation.charset = 1252;		// default = ANSI code page
        memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));

        // get own module (EXE) filename 
        TCHAR fn[_MAX_PATH];
        GetModuleFileName(hModule, fn, _MAX_PATH);

        // read file version info
        DWORD dwDummyHandle; // will always be set to zero
        DWORD len = GetFileVersionInfoSize(fn, &dwDummyHandle);
        if (len <= 0)
            return FALSE;

        if (m_pVersionInfo)
            delete m_pVersionInfo;
        m_pVersionInfo = new BYTE[len]; // allocate version info
        if (!::GetFileVersionInfo(fn, 0, len, m_pVersionInfo))
            return FALSE;

        LPVOID lpvi;
        UINT iLen;
        if (!VerQueryValue(m_pVersionInfo, L"\\", &lpvi, &iLen))
            return FALSE;

        // copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
        *(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpvi;

        // Get translation info
        // Note: VerQueryValue could return a value > 4, in which case
        // multiple languages are supported and VerQueryValue returns an
        // array of langID/code page pairs and you have to decide which to use.
        if (VerQueryValue(m_pVersionInfo,
            L"\\VarFileInfo\\Translation", &lpvi, &iLen) && iLen >= 4) 
        {
                m_translation = *(TRANSLATION*)lpvi;
                //TRACE(_L("CModuleVersion::code page = %d\n"), m_translation.charset);
        }

        return dwSignature == VS_FFI_SIGNATURE;
    }

    //////////////////
    // Get string file info.
    // Key name is something like "CompanyName".
    // returns the value as a CString.
    //
    std::wstring CModuleVersion::GetValue(LPCTSTR lpKeyName)
    {
        std::wstring sVal;
        if (m_pVersionInfo) {

            // To get a string value must pass query in the form
            //
            //    "\StringFileInfo\<langID><codepage>\keyname"
            //
            // where <lang-codepage> is the languageID concatenated with the
            // code page, in hex. Wow.
            //
            std::wstringstream query;
            query << L"\\StringFileInfo\\"
                << std::setfill(L'0')<< std::setw(4)<<std::hex << m_translation.langID
                << std::setfill(L'0')<< std::setw(4)<<std::hex << m_translation.charset
                << L"\\"
                << wstring(lpKeyName);
            //query.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
            //    m_translation.langID,
            //    m_translation.charset,
            //    lpKeyName);
            // \\StringFileInfo\\0409\\04e4\\FileVersion


            LPCTSTR pVal;
            UINT iLenVal;
            if (VerQueryValue(m_pVersionInfo, (LPTSTR)(LPCTSTR)(query.str().c_str()),
                (LPVOID*)&pVal, &iLenVal)) {

                    sVal = pVal;
            }
        }
        return sVal;
    }


    /////////////////
    // Get DLL Version by calling DLL's DllGetVersion proc
    //
    //BOOL CModuleVersion::DllGetVersion(LPCTSTR modulename, DLLVERSIONINFO& dvi)
    //{
    //	CLoadLibrary lib(modulename);
    //	if (!lib)
    //		return FALSE;
    //
    //	// typedef for DllGetVersion proc
    //	typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);
    //
    //	// Must use GetProcAddress because the DLL might not implement 
    //	// DllGetVersion. Depending upon the DLL, the lack of implementation of the 
    //	// function may be a version marker in itself.
    //	//
    //	DLLGETVERSIONPROC pDllGetVersion =
    //		(DLLGETVERSIONPROC)GetProcAddress(lib, "DllGetVersion");
    //	if (!pDllGetVersion)
    //		return FALSE;
    //
    //	memset(&dvi, 0, sizeof(dvi));			 // clear
    //	dvi.cbSize = sizeof(dvi);				 // set size for Windows
    //
    //	return SUCCEEDED((*pDllGetVersion)(&dvi));
    //}

    BOOL Is_Vista_or_Later () 
    {
        OSVERSIONINFOEX osvi;
        DWORDLONG dwlConditionMask = 0;
        int op=VER_GREATER_EQUAL;

        // Initialize the OSVERSIONINFOEX structure.

        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        osvi.dwMajorVersion = 6;
        osvi.dwMinorVersion = 0;
        osvi.wServicePackMajor = 0;
        osvi.wServicePackMinor = 0;

        // Initialize the condition mask.

        VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
        VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
        VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
        VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

        // Perform the test.

        return VerifyVersionInfo(
            &osvi, 
            VER_MAJORVERSION | VER_MINORVERSION | 
            VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
            dwlConditionMask);
    }

}
