#include "grpapi.h"
#include "..\\LoadStorm\\storm.h"
#include <malloc.h>

#ifdef GRPAPI_STATIC
#define DllMain GrpMain

#include "grp_static.h"

struct GRPLIBMODULE {
	GRPLIBMODULE();
	~GRPLIBMODULE();
} GrpLib;

BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD  ul_reason_for_call, LPVOID lpReserved);

GRPLIBMODULE::GRPLIBMODULE()
{
	GrpMain(0,DLL_PROCESS_ATTACH,0);
}

GRPLIBMODULE::~GRPLIBMODULE()
{
	GrpMain(0,DLL_PROCESS_DETACH,0);
}

#endif

typedef struct {
	BYTE Left;
	BYTE Top;
	BYTE Width;
	BYTE Height;
	DWORD Offset;
} FRAMEHEADER;

void __inline SetPix(HDC hDC, int X, int Y, COLORREF clrColor, DWORD dwFlags, DWORD dwAlpha);

extern HINSTANCE hStorm;

BOOL APIENTRY DllMain( HINSTANCE hInstDLL, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }

    return TRUE;
}

BOOL GRPAPI WINAPI LoadGrpApi()
{
	if (hStorm==0) return FALSE;
	else return TRUE;
}

void GRPAPI WINAPI FreeGrpApi()
{
}

BOOL GRPAPI WINAPI LoadPalette(LPCSTR lpFileName, DWORD *dwPaletteBuffer)
{
	if (lpFileName==0 || dwPaletteBuffer==0) return FALSE;
	HANDLE hFile;
	if (SFileOpenFileEx!=0 && SFileGetFileSize!=0
	 && SFileSetFilePointer!=0 && SFileReadFile!=0
	 && SFileCloseFile!=0) {
		if (SFileOpenFileEx(0,lpFileName,1,&hFile)==0) return FALSE;
		DWORD fsz = SFileGetFileSize(hFile,0);
		SFileSetFilePointer(hFile,0,0,FILE_BEGIN);
		if (fsz>=1024) {
			memset(dwPaletteBuffer,0,1024);
			SFileReadFile(hFile,dwPaletteBuffer,1024,0,0);
		}
		else if (fsz==768) {
			char *buffer = (char *)_alloca(768);
			memset(buffer,0,768);
			SFileReadFile(hFile,buffer,768,0,0);
			for (DWORD i=0;i<256;i++) {
				memcpy(&dwPaletteBuffer[i],buffer+i*3,3);
				*(((char *)&dwPaletteBuffer[i])+3) = 0;
			}
		}
		else {
			memset(dwPaletteBuffer,0,1024);
			SFileReadFile(hFile,dwPaletteBuffer,fsz,0,0);
		}
		SFileCloseFile(hFile);
	}
	else {
		hFile = CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
		if (hFile==INVALID_HANDLE_VALUE) return FALSE;
		DWORD fsz = GetFileSize(hFile,0),tsz;
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		if (fsz>=1024) {
			memset(dwPaletteBuffer,0,1024);
			ReadFile(hFile,dwPaletteBuffer,1024,&tsz,0);
		}
		else if (fsz==768) {
			char *buffer = (char *)_alloca(768);
			memset(buffer,0,768);
			ReadFile(hFile,buffer,768,&tsz,0);
			for (DWORD i=0;i<256;i++) {
				memcpy(&dwPaletteBuffer[i],buffer+i*3,3);
				*(((char *)&dwPaletteBuffer[i])+3) = 0;
			}
		}
		else {
			memset(dwPaletteBuffer,0,1024);
			ReadFile(hFile,dwPaletteBuffer,fsz,&tsz,0);
		}
		CloseHandle(hFile);
	}
	return TRUE;
}

HANDLE GRPAPI WINAPI LoadGrp(LPCSTR lpFileName)
{
	if (lpFileName==0) return (HANDLE)-1;
	HANDLE hFile;
	char *GrpFile;
	if (SFileOpenFileEx!=0 && SFileGetFileSize!=0
	 && SFileSetFilePointer!=0 && SFileReadFile!=0
	 && SFileCloseFile!=0) {
		if (SFileOpenFileEx(0,lpFileName,1,&hFile)==0) return (HANDLE)-1;
		DWORD fsz = SFileGetFileSize(hFile,0);
		if (fsz<6) {
			SFileCloseFile(hFile);
			return (HANDLE)-1;
		}
		GrpFile = (char *)VirtualAlloc(0,fsz,MEM_COMMIT,PAGE_READWRITE);
		if (GrpFile!=0) {
			SFileSetFilePointer(hFile,0,0,FILE_BEGIN);
			SFileReadFile(hFile,GrpFile,fsz,0,0);
		}
		else GrpFile=(char *)-1;
		SFileCloseFile(hFile);
	}
	else {
		hFile = CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
		if (hFile==INVALID_HANDLE_VALUE) return (HANDLE)-1;
		DWORD fsz = GetFileSize(hFile,0),tsz;
		if (fsz<6) {
			CloseHandle(hFile);
			return (HANDLE)-1;
		}
		GrpFile = (char *)VirtualAlloc(0,fsz,MEM_COMMIT,PAGE_READWRITE);
		if (GrpFile!=0) {
			SetFilePointer(hFile,0,0,FILE_BEGIN);
			ReadFile(hFile,GrpFile,fsz,&tsz,0);
		}
		else GrpFile=(char *)-1;
		CloseHandle(hFile);
	}
	return (HANDLE)GrpFile;
}

BOOL GRPAPI WINAPI DestroyGrp(HANDLE hGrp)
{
	if (hGrp==0 || hGrp==INVALID_HANDLE_VALUE) return FALSE;
	VirtualFree(hGrp,0,MEM_RELEASE);
	return TRUE;
}

BOOL GRPAPI WINAPI DrawGrp(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha)
{
	if (hGrp==0 || hGrp==INVALID_HANDLE_VALUE || hdcDest==0 || dwPalette==0) return FALSE;
	GRPHEADER *GrpFile = (GRPHEADER *)hGrp;
	nFrame %= GrpFile->nFrames;
	FRAMEHEADER *GrpFrame = &((FRAMEHEADER *)(((char *)GrpFile)+6))[nFrame];
	DWORD Right,Bottom;
	if (dwFlags&HORIZONTAL_FLIP) Right = nXDest+GrpFile->wMaxWidth-1-GrpFrame->Left;
	if (dwFlags&VERTICAL_FLIP) Bottom = nYDest+GrpFile->wMaxHeight-1-GrpFrame->Top;
	nXDest += GrpFrame->Left;
	nYDest += GrpFrame->Top;
	WORD *GrpOffsets = ((WORD *)(((char *)GrpFile)+GrpFrame->Offset));
	for (DWORD y=0;y<GrpFrame->Height;y++) {
		BYTE *RowData = ((BYTE *)(((char *)GrpOffsets)+GrpOffsets[y]));
		WORD x=0,ofs=0;
		while (x<GrpFrame->Width) {
			if (RowData[ofs]<=64) {
				if ((dwFlags&HORIZONTAL_FLIP)==0 && (dwFlags&VERTICAL_FLIP)==0) {
					for (WORD i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,nXDest+x,nYDest+y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
						x++;
					}
				}
				else if (dwFlags&HORIZONTAL_FLIP && (dwFlags&VERTICAL_FLIP)==0) {
					for (WORD i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,Right-x,nYDest+y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
						x++;
					}
				}
				else if ((dwFlags&HORIZONTAL_FLIP)==0 && dwFlags&VERTICAL_FLIP) {
					for (WORD i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,nXDest+x,Bottom-y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
						x++;
					}
				}
				else {
					for (WORD i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,Right-x,Bottom-y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
						x++;
					}
				}
				ofs+=RowData[ofs]+1;
			}
			else if (RowData[ofs]<=128) {
				if ((dwFlags&HORIZONTAL_FLIP)==0 && (dwFlags&VERTICAL_FLIP)==0) {
					for (WORD i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,nXDest+x,nYDest+y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
						x++;
					}
				}
				else if (dwFlags&HORIZONTAL_FLIP && (dwFlags&VERTICAL_FLIP)==0) {
					for (WORD i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,Right-x,nYDest+y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
						x++;
					}
				}
				else if ((dwFlags&HORIZONTAL_FLIP)==0 && dwFlags&VERTICAL_FLIP) {
					for (WORD i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,nXDest+x,Bottom-y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
						x++;
					}
				}
				else {
					for (WORD i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
						SetPix(hdcDest,Right-x,Bottom-y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
						x++;
					}
				}
				ofs+=2;
			}
			else {
				x+=RowData[ofs]-128;
				ofs++;
			}
		}
	}
	return TRUE;
}

BOOL GRPAPI WINAPI GetGrpInfo(HANDLE hGrp, GRPHEADER *GrpInfo)
{
	if (hGrp==0 || hGrp==INVALID_HANDLE_VALUE || GrpInfo==0) return FALSE;
	memcpy(GrpInfo,hGrp,6);
	return TRUE;
}

void __inline SetPix(HDC hDC, int X, int Y, COLORREF clrColor, DWORD dwFlags, DWORD dwAlpha)
{
	if (dwFlags&SHADOW_COLOR) {
		((BYTE *)&clrColor)[0] = ((BYTE *)&dwFlags)[1];
		((BYTE *)&clrColor)[1] = ((BYTE *)&dwFlags)[2];
		((BYTE *)&clrColor)[2] = ((BYTE *)&dwFlags)[3];
		((BYTE *)&clrColor)[3] = 0;
	}
	if (dwFlags&ALPHA_BLEND) {
		DWORD dwColor = GetPixel(hDC,X,Y);

		// Old alpha
		/*((BYTE *)&dwColor)[0]*=1-((float)((BYTE *)&dwAlpha)[0]/256);
		((BYTE *)&dwColor)[1]*=1-((float)((BYTE *)&dwAlpha)[1]/256);
		((BYTE *)&dwColor)[2]*=1-((float)((BYTE *)&dwAlpha)[2]/256);
		((BYTE *)&clrColor)[0]*=(float)((BYTE *)&dwAlpha)[0]/256;
		((BYTE *)&clrColor)[1]*=(float)((BYTE *)&dwAlpha)[1]/256;
		((BYTE *)&clrColor)[2]*=(float)((BYTE *)&dwAlpha)[2]/256;
		((BYTE *)&clrColor)[0]+=((BYTE *)&dwColor)[0];
		((BYTE *)&clrColor)[1]+=((BYTE *)&dwColor)[1];
		((BYTE *)&clrColor)[2]+=((BYTE *)&dwColor)[2];*/

		/* blendedcolor =
		     ( ( forecolor * ( 1 - alpha ) ) >> 8 )
		   + ( ( backcolor * ( 256 - alpha ) ) >> 8 ) */
		((BYTE *)&clrColor)[0] =
			( ( ((BYTE *)&clrColor)[0] * ( ((BYTE *)&dwAlpha)[0] + 1 ) ) >> 8 )
			+ ( ( ((BYTE *)&dwColor)[0] * ( 256 - ((BYTE *)&dwAlpha)[0] ) ) >> 8 );
		((BYTE *)&clrColor)[1] =
			( ( ((BYTE *)&clrColor)[1] * ( ((BYTE *)&dwAlpha)[1] + 1 ) ) >> 8 )
			+ ( ( ((BYTE *)&dwColor)[1] * ( 256 - ((BYTE *)&dwAlpha)[1] ) ) >> 8 );
		((BYTE *)&clrColor)[2] =
			( ( ((BYTE *)&clrColor)[2] * ( ((BYTE *)&dwAlpha)[2] + 1 ) ) >> 8 )
			+ ( ( ((BYTE *)&dwColor)[2] * ( 256 - ((BYTE *)&dwAlpha)[2] ) ) >> 8 );
	}
	SetPixelV(hDC,X,Y,clrColor);
}

