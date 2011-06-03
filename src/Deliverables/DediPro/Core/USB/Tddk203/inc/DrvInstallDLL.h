#ifdef DRVINSTALLDLL_EXPORTS
#define DRVINSTALLDLL_API __declspec(dllexport)
#else
#define DRVINSTALLDLL_API __declspec(dllimport)
#endif

#define CUSTOM_SPDRP_GETREGISTRYDEVICEPATH    0x1000
#define CUSTOM_SPDRP_GETDEVICEID            0x1001

#ifdef __cplusplus
extern "C" {
#endif

DRVINSTALLDLL_API DWORD CreateDeviceSet(LPTSTR szClassGUID, HDEVINFO* pDeviceInfoSet, DWORD Present);
DRVINSTALLDLL_API DWORD CreateDeviceSetFromInterface(LPTSTR szInterfaceGUID, HDEVINFO* pDeviceInfoSet, DWORD Present);
DRVINSTALLDLL_API DWORD DestroyDeviceSet(HDEVINFO DeviceInfoSet);

DRVINSTALLDLL_API DWORD GetDevicesList(HDEVINFO DeviceInfoSet,
                                       PDWORD pIndex, 
                                       PSP_DEVINFO_DATA pDevHandle, 
                                       PBOOL pPlugged);
DRVINSTALLDLL_API DWORD GetDevicesListByServiceName(HDEVINFO DeviceInfoSet,
                                                    LPTSTR szDriverName, 
                                                    PDWORD pIndex, 
                                                    PSP_DEVINFO_DATA pDevHandle, 
                                                    PBOOL pPlugged);
DRVINSTALLDLL_API DWORD GetDevicesListByHardwareID(HDEVINFO DeviceInfoSet,
                                                   LPTSTR szHardwareID, 
                                                   BOOL ExactMatch, 
                                                   PDWORD pIndex, 
                                                   PSP_DEVINFO_DATA pDevHandle, 
                                                   PBOOL pPlugged);
DRVINSTALLDLL_API DWORD GetDeviceInformationSize(HDEVINFO DeviceInfoSet,
                                                 PSP_DEVINFO_DATA pDevHandle, 
                                                 DWORD nTypeInfo, 
                                                 PDWORD pSize);
DRVINSTALLDLL_API DWORD GetDeviceInformation(HDEVINFO DeviceInfoSet,
                                             PSP_DEVINFO_DATA pDevHandle, 
                                             DWORD nTypeInfo, 
                                             PVOID pInfo, 
                                             DWORD Size);
DRVINSTALLDLL_API DWORD RemoveDevice(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA pDevHandle);
DRVINSTALLDLL_API DWORD RemoveInterface(HDEVINFO DeviceInfoSet, 
                                        PSP_DEVINFO_DATA pDevHandle,
                                        LPTSTR szInterfaceGUID);
DRVINSTALLDLL_API DWORD RemoveDeviceOEMInf(HDEVINFO DeviceInfoSet, 
                                           PSP_DEVINFO_DATA pDevHandle, 
                                           LPTSTR szInfProvider);
DRVINSTALLDLL_API DWORD RemoveOEMInfsCompatibleWithHardwareID(LPTSTR szHardwareID,
                                                              LPTSTR szInfProvider);
DRVINSTALLDLL_API DWORD UpdateDevice(LPTSTR szHardwareID, LPTSTR szInfPath, PBOOL pNeedsReboot);
DRVINSTALLDLL_API DWORD InstallDevice(LPTSTR szInfPath);
DRVINSTALLDLL_API DWORD ForceReEnumeration(BOOL bSynchronous);

#ifdef __cplusplus
}
#endif
