// dllmain.h : Declaration of module class.

class CDServerModule : public CAtlDllModuleT< CDServerModule >
{
public :
	DECLARE_LIBID(LIBID_DServerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DSERVER, "{C24B79B4-CE85-4981-97D9-D339555B2AFD}")
};

extern class CDServerModule _AtlModule;
