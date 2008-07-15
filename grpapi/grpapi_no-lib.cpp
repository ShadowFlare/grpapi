/* License information for this code is in license.txt */

#include "grpapi_no-lib.h"

struct GRPAPIMODULE {
	GRPAPIMODULE();
	~GRPAPIMODULE();
} GrpApi;

HINSTANCE hGrpApi = 0;

BOOL   (WINAPI* LoadPalette)(LPCSTR lpFileName, DWORD *dwPaletteBuffer) = 0;
HANDLE (WINAPI* LoadGrp)(LPCSTR lpFileName) = 0;
BOOL   (WINAPI* DestroyGrp)(HANDLE hGrp) = 0;
BOOL   (WINAPI* DrawGrp)(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha) = 0;
BOOL   (WINAPI* GetGrpInfo)(HANDLE hGrp, GRPHEADER *GrpInfo) = 0;
BOOL   (WINAPI* GetGrpFrameInfo)(HANDLE hGrp, WORD nFrame, DWORD *nLeft, DWORD *nTop, DWORD *nWidth, DWORD *nHeight) = 0;
HANDLE (WINAPI* CreateGrp)(signed short *lpImageData, WORD nFrames, WORD wMaxWidth, WORD wMaxHeight, BOOL bNoCompress, DWORD *nGrpSize) = 0;
void (WINAPI* SetFunctionGetPixel)(GETPIXELPROC lpGetPixelProc) = 0;
void (WINAPI* SetFunctionSetPixel)(SETPIXELPROC lpSetPixelProc) = 0;
BOOL (WINAPI* SetMpqDll)(LPCSTR lpDllFileName) = 0;
BOOL   (WINAPI* LoadGrpApi)() = 0;
void   (WINAPI* FreeGrpApi)() = 0;

GRPAPIMODULE::GRPAPIMODULE()
{
	if (hGrpApi!=0) return;
	hGrpApi = LoadLibrary("grpapi.dll");

	if (hGrpApi!=0) {
		*(FARPROC *)&LoadPalette = GetProcAddress(hGrpApi,"LoadPalette");
		*(FARPROC *)&LoadGrp = GetProcAddress(hGrpApi,"LoadGrp");
		*(FARPROC *)&DestroyGrp = GetProcAddress(hGrpApi,"DestroyGrp");
		*(FARPROC *)&DrawGrp = GetProcAddress(hGrpApi,"DrawGrp");
		*(FARPROC *)&GetGrpInfo = GetProcAddress(hGrpApi,"GetGrpInfo");
		*(FARPROC *)&GetGrpFrameInfo = GetProcAddress(hGrpApi,"GetGrpFrameInfo");
		*(FARPROC *)&CreateGrp = GetProcAddress(hGrpApi,"CreateGrp");
		*(FARPROC *)&SetFunctionGetPixel = GetProcAddress(hGrpApi,"SetFunctionGetPixel");
		*(FARPROC *)&SetFunctionSetPixel = GetProcAddress(hGrpApi,"SetFunctionSetPixel");
		*(FARPROC *)&SetMpqDll = GetProcAddress(hGrpApi,"SetMpqDll");
		*(FARPROC *)&LoadGrpApi = GetProcAddress(hGrpApi,"LoadGrpApi");
		*(FARPROC *)&FreeGrpApi = GetProcAddress(hGrpApi,"FreeGrpApi");
	}
}

GRPAPIMODULE::~GRPAPIMODULE()
{
	LoadPalette = 0;
	LoadGrp = 0;
	DestroyGrp = 0;
	DrawGrp = 0;
	GetGrpInfo = 0;
	GetGrpFrameInfo = 0;
	CreateGrp = 0;
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
