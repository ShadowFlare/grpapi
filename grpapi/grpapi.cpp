#include "grpapi.h"
#include "../LoadStorm/storm.h"
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

GETPIXELPROC MyGetPixel = GetPixel;
SETPIXELPROC MySetPixel = (SETPIXELPROC)SetPixelV;

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
	if (!hStorm) return FALSE;
	else return TRUE;
}

void GRPAPI WINAPI FreeGrpApi()
{
}

BOOL GRPAPI WINAPI SetMpqDll(LPCSTR lpDllFileName)
{
	if (LoadStorm((char *)lpDllFileName)) return TRUE;
	else return FALSE;
}

BOOL GRPAPI WINAPI LoadPalette(LPCSTR lpFileName, DWORD *dwPaletteBuffer)
{
	if (!lpFileName || !dwPaletteBuffer) return FALSE;
	HANDLE hFile;
	if (SFileOpenFileEx && SFileGetFileSize
	 && SFileSetFilePointer && SFileReadFile
	 && SFileCloseFile) {
		if (!SFileOpenFileEx(0,lpFileName,1,&hFile)) return FALSE;
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
	if (!lpFileName) return (HANDLE)-1;
	HANDLE hFile;
	char *GrpFile;
	if (SFileOpenFileEx && SFileGetFileSize
	 && SFileSetFilePointer && SFileReadFile
	 && SFileCloseFile) {
		if (!SFileOpenFileEx(0,lpFileName,1,&hFile)) return (HANDLE)-1;
		DWORD fsz = SFileGetFileSize(hFile,0);
		if (fsz<6) {
			SFileCloseFile(hFile);
			return (HANDLE)-1;
		}
		GrpFile = (char *)VirtualAlloc(0,fsz,MEM_COMMIT,PAGE_READWRITE);
		if (GrpFile) {
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
		if (GrpFile) {
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
	if (!hGrp || hGrp==INVALID_HANDLE_VALUE) return FALSE;
	VirtualFree(hGrp,0,MEM_RELEASE);
	return TRUE;
}

BOOL GRPAPI WINAPI DrawGrp(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha)
{
	if (!hGrp || hGrp==INVALID_HANDLE_VALUE || hdcDest==0 || !dwPalette) return FALSE;
	GRPHEADER *GrpFile = (GRPHEADER *)hGrp;
	nFrame %= GrpFile->nFrames;
	FRAMEHEADER *GrpFrame = &((FRAMEHEADER *)(((char *)GrpFile)+6))[nFrame];
	FRAMEHEADER *GrpFrames = &((FRAMEHEADER *)(((char *)GrpFile)+6))[0];
	int FrameSize = 0xFFFFFF;
	DWORD Right,Bottom;
	if (dwFlags&HORIZONTAL_FLIP) Right = nXDest+GrpFile->wMaxWidth-1-GrpFrame->Left;
	if (dwFlags&VERTICAL_FLIP) Bottom = nYDest+GrpFile->wMaxHeight-1-GrpFrame->Top;
	nXDest += GrpFrame->Left;
	nYDest += GrpFrame->Top;
	WORD *GrpOffsets = ((WORD *)(((char *)GrpFile)+GrpFrame->Offset));
	BYTE *GrpRaw = (BYTE *)GrpOffsets;
	BYTE *RowData;
	WORD x,ofs;
	DWORD y;
	WORD i;
	int j;
	if (nFrame + 1 < GrpFile->nFrames) {
		for (i = 0; i + 1 < GrpFile->nFrames; i++) {
			j = GrpFrames[i].Offset - GrpFrame->Offset;
			if (j > 0 && j < FrameSize)
				FrameSize = j;
		}
	}
	if (FrameSize == 0xFFFFFF) {
		for (i = 0; i + 1 < GrpFile->nFrames; i++) {
			j = GrpFrames[i].Offset - GrpFrames[0].Offset;
			if (j > 0 && j < FrameSize)
				FrameSize = j;
		}
		if (FrameSize == GrpFrames[0].Width * GrpFrames[0].Height)
			FrameSize = GrpFrame->Width * GrpFrame->Height;
	}
	if (!(dwFlags&HORIZONTAL_FLIP) && !(dwFlags&VERTICAL_FLIP)) {
		if (FrameSize != GrpFrame->Width * GrpFrame->Height) {
			for (y=0;y<GrpFrame->Height;y++) {
				RowData = ((BYTE *)(((char *)GrpOffsets)+GrpOffsets[y]));
				x=0; ofs=0;
				while (x<GrpFrame->Width) {
					if (!(RowData[ofs] & 0x80)) {
						if (!(RowData[ofs] & 0x40)) {
							for (i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,nYDest+y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,nYDest+y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=2;
						}
					}
					else {
						x+=RowData[ofs]-128;
						ofs++;
					}
				}
			}
		}
		else {
			for (y=0;y<GrpFrame->Height;y++) {
				for (x=0;x<GrpFrame->Width;x++) {
					SetPix(hdcDest,nXDest+x,nYDest+y,dwPalette[GrpRaw[y * GrpFrame->Width + x]],dwFlags,dwAlpha);
				}
			}
		}
	}
	else if (dwFlags&HORIZONTAL_FLIP && !(dwFlags&VERTICAL_FLIP)) {
		if (FrameSize != GrpFrame->Width * GrpFrame->Height) {
			for (y=0;y<GrpFrame->Height;y++) {
				RowData = ((BYTE *)(((char *)GrpOffsets)+GrpOffsets[y]));
				x=0; ofs=0;
				while (x<GrpFrame->Width) {
					if (!(RowData[ofs] & 0x80)) {
						if (!(RowData[ofs] & 0x40)) {
							for (i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,nYDest+y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,nYDest+y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=2;
						}
					}
					else {
						x+=RowData[ofs]-128;
						ofs++;
					}
				}
			}
		}
		else {
			for (y=0;y<GrpFrame->Height;y++) {
				for (x=0;x<GrpFrame->Width;x++) {
					SetPix(hdcDest,Right-x,nYDest+y,dwPalette[GrpRaw[y * GrpFrame->Width + x]],dwFlags,dwAlpha);
				}
			}
		}
	}
	else if (!(dwFlags&HORIZONTAL_FLIP) && dwFlags&VERTICAL_FLIP) {
		if (FrameSize != GrpFrame->Width * GrpFrame->Height) {
			for (y=0;y<GrpFrame->Height;y++) {
				RowData = ((BYTE *)(((char *)GrpOffsets)+GrpOffsets[y]));
				x=0; ofs=0;
				while (x<GrpFrame->Width) {
					if (!(RowData[ofs] & 0x80)) {
						if (!(RowData[ofs] & 0x40)) {
							for (i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,Bottom-y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,Bottom-y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=2;
						}
					}
					else {
						x+=RowData[ofs]-128;
						ofs++;
					}
				}
			}
		}
		else {
			for (y=0;y<GrpFrame->Height;y++) {
				for (x=0;x<GrpFrame->Width;x++) {
					SetPix(hdcDest,nXDest+x,Bottom-y,dwPalette[GrpRaw[y * GrpFrame->Width + x]],dwFlags,dwAlpha);
				}
			}
		}
	}
	else {
		if (FrameSize != GrpFrame->Width * GrpFrame->Height) {
			for (y=0;y<GrpFrame->Height;y++) {
				RowData = ((BYTE *)(((char *)GrpOffsets)+GrpOffsets[y]));
				x=0; ofs=0;
				while (x<GrpFrame->Width) {
					if (!(RowData[ofs] & 0x80)) {
						if (!(RowData[ofs] & 0x40)) {
							for (i=1;i<=RowData[ofs] && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,Bottom-y,dwPalette[RowData[ofs+i]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,Bottom-y,dwPalette[RowData[ofs+1]],dwFlags,dwAlpha);
								x++;
							}
							ofs+=2;
						}
					}
					else {
						x+=RowData[ofs]-128;
						ofs++;
					}
				}
			}
		}
		else {
			for (y=0;y<GrpFrame->Height;y++) {
				for (x=0;x<GrpFrame->Width;x++) {
					SetPix(hdcDest,Right-x,Bottom-y,dwPalette[GrpRaw[y * GrpFrame->Width + x]],dwFlags,dwAlpha);
				}
			}
		}
	}
	return TRUE;
}

BOOL GRPAPI WINAPI GetGrpInfo(HANDLE hGrp, GRPHEADER *GrpInfo)
{
	if (!hGrp || hGrp==INVALID_HANDLE_VALUE || !GrpInfo) return FALSE;
	memcpy(GrpInfo,hGrp,6);
	return TRUE;
}

void GRPAPI WINAPI SetFunctionGetPixel(GETPIXELPROC lpGetPixelProc)
{
	MyGetPixel = lpGetPixelProc;
}

void GRPAPI WINAPI SetFunctionSetPixel(SETPIXELPROC lpSetPixelProc)
{
	MySetPixel = lpSetPixelProc;
}

void __inline SetPix(HDC hDC, int X, int Y, COLORREF clrColor, DWORD dwFlags, DWORD dwAlpha)
{
	if (dwFlags&SHADOW_COLOR) {
		clrColor = (dwFlags >> 8) & 0x00FFFFFF;
	}
	if (dwFlags&ALPHA_BLEND) {
		DWORD dwColor = MyGetPixel(hDC,X,Y);

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
	MySetPixel(hDC,X,Y,clrColor);
}
