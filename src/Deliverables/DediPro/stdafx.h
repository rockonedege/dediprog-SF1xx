// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if defined(DEDICORE)
    #define _AFXDLL

    #ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
    #define WINVER 0x0500           // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
    #define _WIN32_WINNT 0x0500     // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
    #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
    #endif

    #ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
    #define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
    #endif

    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    // Windows Header Files:
    #include <windows.h>



    // TODO: reference additional headers your program requires here
    #include "util.h"
    #include <utility>
    #include <map>
    #include <string>
    #include "VerInfo.h"

#elif defined(DCORE)
    #define _AFXDLL

    #ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
    #define WINVER 0x0500           // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
    #define _WIN32_WINNT 0x0500     // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
    #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
    #endif

    #ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
    #define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
    #endif

    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

    #ifndef VC_EXTRALEAN
    #define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
    #endif

    #include <afx.h>
    #include <afxwin.h>         // MFC core and standard components
    #include <afxext.h>         // MFC extensions
    #ifndef _AFX_NO_OLE_SUPPORT
    #include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
    #endif
    #ifndef _AFX_NO_AFXCMN_SUPPORT
    #include <afxcmn.h>                     // MFC support for Windows Common Controls
    #endif // _AFX_NO_AFXCMN_SUPPORT

    #include <iostream>
    // Windows Header Files:
    #include <windows.h>


    #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

    #include <atlbase.h>
    #include <atlstr.h>

    // TODO: reference additional headers your program requires here
    #include "util.h"
    #include <utility>
    #include <map>
    #include <string>
    #include "VerInfo.h"

#elif defined(DSERVER)

    #ifndef STRICT
    #define STRICT
    #endif

    #include "..\DServer\targetver.h"

    #define _ATL_APARTMENT_THREADED
    #define _ATL_NO_AUTOMATIC_NAMESPACE

    #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // some CString constructors will be explicit

    #include "resource.h"
    #include <atlbase.h>
    #include <atlcom.h>
    #include <atlctl.h>

    using namespace ATL;
#else
    #ifndef _SECURE_ATL
    #define _SECURE_ATL 1
    #endif

    #ifndef VC_EXTRALEAN
    #define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
    #endif

    // Modify the following defines if you have to target a platform prior to the ones specified below.
    // Refer to MSDN for the latest info on corresponding values for different platforms.
    #ifndef WINVER                // Allow use of features specific to Windows XP or later.
    #define WINVER 0x0500        // Change this to the appropriate value to target other versions of Windows.
    #endif

    #ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
    #define _WIN32_WINNT 0x0500    // Change this to the appropriate value to target other versions of Windows.
    #endif                        

    #ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
    #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
    #endif

    #ifndef _WIN32_IE            // Allow use of features specific to IE 6.0 or later.
    #define _WIN32_IE 0x0600    // Change this to the appropriate value to target other versions of IE.
    #endif

    #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // some CString constructors will be explicit

    // turns off MFC's hiding of some common and often safely ignored warning messages
    #define _AFX_ALL_WARNINGS

    #include <afxwin.h>         // MFC core and standard components
    #include <afxext.h>         // MFC extensions


    #include <afxdisp.h>        // MFC Automation classes

    #ifndef _AFX_NO_OLE_SUPPORT
    #include <afxdtctl.h>        // MFC support for Internet Explorer 4 Common Controls
    #endif
    #ifndef _AFX_NO_AFXCMN_SUPPORT
    #include <afxcmn.h>            // MFC support for Windows Common Controls
    #endif // _AFX_NO_AFXCMN_SUPPORT


    #include "util.h"
    #include <utility>
    #include <map>
    #include <string>
    #include "VerInfo.h"
    #include ".\3rd\ColorNames\ColorNames.h"
    #include <afxdlgs.h>


    #ifdef _UNICODE
    #if defined _M_IX86
    #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #elif defined _M_IA64
    #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #elif defined _M_X64
    #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #else
    #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
    #endif
    #endif

#endif

