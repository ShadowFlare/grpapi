#include "grpapi_no-lib.h"

struct GRPAPIMODULE {
	GRPAPIMODULE();
	~GRPAPIMODULE();
} GrpApi;

HINSTANCE hGrpApi = 0;

funcLoadPalette LoadPalette = 0;
funcLoadGrp LoadGrp = 0;
funcDestroyGrp DestroyGrp = 0;
funcDrawGrp DrawGrp = 0;
funcGetGrpInfo GetGrpInfo = 0;
funcSetFunctionGetPixel SetFunctionGetPixel = 0;
funcSetFunctionSetPixel SetFunctionSetPixel = 0;
funcSetMpqDll SetMpqDll = 0;
funcLoadGrpApi LoadGrpApi = 0;
funcFreeGrpApi FreeGrpApi = 0;

GRPAPIMODULE::GRPAPIMODULE()
{
	if (hGrpApi!=0) return;
	hGrpApi = LoadLibrary("grpapi.dll");

	if (hGrpApi!=0) {
		LoadPalette = (funcLoadPalette)GetProcAddress(hGrpApi,"LoadPalette");
		LoadGrp = (funcLoadGrp)GetProcAddress(hGrpApi,"LoadGrp");
		DestroyGrp = (funcDestroyGrp)GetProcAddress(hGrpApi,"DestroyGrp");
		DrawGrp = (funcDrawGrp)GetProcAddress(hGrpApi,"DrawGrp");
		GetGrpInfo = (funcGetGrpInfo)GetProcAddress(hGrpApi,"GetGrpInfo");
		SetFunctionGetPixel = (funcSetFunctionGetPixel)GetProcAddress(hGrpApi,"SetFunctionGetPixel");
		SetFunctionSetPixel = (funcSetFunctionSetPixel)GetProcAddress(hGrpApi,"SetFunctionSetPixel");
		SetMpqDll = (funcSetMpqDll)GetProcAddress(hGrpApi,"SetMpqDll");
		LoadGrpApi = (funcLoadGrpApi)GetProcAddress(hGrpApi,"LoadGrpApi");
		FreeGrpApi = (funcFreeGrpApi)GetProcAddress(hGrpApi,"FreeGrpApi");
	}
}

GRPAPIMODULE::~GRPAPIMODULE()
{
	LoadPalette = 0;
	LoadGrp = 0;
	DestroyGrp = 0;
	DrawGrp = 0;
	GetGrpInfo = 0;
	SetFunctionGetPixel = 0;
	SetFunctionSetPixel = 0;
	SetMpqDll = 0;
	LoadGrpApi = 0;
	FreeGrpApi = 0;

	if (hGrpApi==0) return;
	FreeLibrary(hGrpApi);
	hGrpApi = 0;
}

void LoadGrpApiLib()
{
}

void FreeGrpApiLib()
{
}
