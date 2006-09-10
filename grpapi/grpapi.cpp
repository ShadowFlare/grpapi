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

typedef struct {
	WORD *lpRowOffsets;
	WORD *lpRowSizes;
	LPBYTE *lpRowData;
} FRAMEDATA;

GETPIXELPROC MyGetPixel = GetPixel;
SETPIXELPROC MySetPixel = (SETPIXELPROC)SetPixelV;

void __inline SetPix(HDC hDC, int X, int Y, COLORREF clrColor, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha);
void EncodeFrameData(signed short *lpImageData, WORD nFrame, GRPHEADER *lpGrpHeader, FRAMEHEADER *lpFrameHeader, FRAMEDATA *lpFrameData);
BOOL VerifyRow(signed short *lpRawRow, int nWidth, LPBYTE lpEncRow, int nSize);

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
		GrpFile = (char *)malloc(fsz);
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
		GrpFile = (char *)malloc(fsz);
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
	free(hGrp);
	return TRUE;
}

BOOL GRPAPI WINAPI DrawGrp(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha)
{
	if (!hGrp || hGrp==INVALID_HANDLE_VALUE || hdcDest==0 || (!dwPalette && !(dwFlags&USE_INDEX))) return FALSE;
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
								SetPix(hdcDest,nXDest+x,nYDest+y,RowData[ofs+i],dwPalette,dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,nYDest+y,RowData[ofs+1],dwPalette,dwFlags,dwAlpha);
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
					SetPix(hdcDest,nXDest+x,nYDest+y,GrpRaw[y * GrpFrame->Width + x],dwPalette,dwFlags,dwAlpha);
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
								SetPix(hdcDest,Right-x,nYDest+y,RowData[ofs+i],dwPalette,dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,nYDest+y,RowData[ofs+1],dwPalette,dwFlags,dwAlpha);
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
					SetPix(hdcDest,Right-x,nYDest+y,GrpRaw[y * GrpFrame->Width + x],dwPalette,dwFlags,dwAlpha);
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
								SetPix(hdcDest,nXDest+x,Bottom-y,RowData[ofs+i],dwPalette,dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,nXDest+x,Bottom-y,RowData[ofs+1],dwPalette,dwFlags,dwAlpha);
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
					SetPix(hdcDest,nXDest+x,Bottom-y,GrpRaw[y * GrpFrame->Width + x],dwPalette,dwFlags,dwAlpha);
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
								SetPix(hdcDest,Right-x,Bottom-y,RowData[ofs+i],dwPalette,dwFlags,dwAlpha);
								x++;
							}
							ofs+=RowData[ofs]+1;
						}
						else {
							for (i=0;i<RowData[ofs]-64 && x<GrpFrame->Width;i++) {
								SetPix(hdcDest,Right-x,Bottom-y,RowData[ofs+1],dwPalette,dwFlags,dwAlpha);
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
					SetPix(hdcDest,Right-x,Bottom-y,GrpRaw[y * GrpFrame->Width + x],dwPalette,dwFlags,dwAlpha);
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
	if (!lpGetPixelProc)
		MyGetPixel = GetPixel;
	else
		MyGetPixel = lpGetPixelProc;
}

void GRPAPI WINAPI SetFunctionSetPixel(SETPIXELPROC lpSetPixelProc)
{
	if (!lpSetPixelProc)
		MySetPixel = (SETPIXELPROC)SetPixelV;
	else
		MySetPixel = lpSetPixelProc;
}

void __inline SetPix(HDC hDC, int X, int Y, COLORREF clrColor, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha)
{
	if (!(dwFlags&USE_INDEX)) {
		if (dwFlags&SHADOW_COLOR) {
			clrColor = (dwFlags >> 8) & 0x00FFFFFF;
		}
		else {
			clrColor = dwPalette[clrColor];
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
	}
	MySetPixel(hDC,X,Y,clrColor);
}

HANDLE GRPAPI WINAPI CreateGrp(signed short *lpImageData, WORD nFrames, WORD wMaxWidth, WORD wMaxHeight, BOOL bNoCompress, DWORD *nGrpSize)
{
	GRPHEADER GrpHeader;
	FRAMEHEADER *lpFrameHeaders;
	FRAMEDATA *lpFrameData;
	LPBYTE lpGrpData;
	int i, j, x, y, x1, x2, y1, y2;
	DWORD nLastOffset;

	if (!lpImageData || !nGrpSize) return (HANDLE)-1;

	GrpHeader.nFrames = nFrames;
	GrpHeader.wMaxWidth = wMaxWidth;
	GrpHeader.wMaxHeight = wMaxHeight;
	lpFrameHeaders = (FRAMEHEADER *)malloc(nFrames * sizeof(FRAMEHEADER));
	lpFrameData = (FRAMEDATA *)malloc(nFrames * sizeof(FRAMEDATA));

	for (i = 0; i < nFrames; i++) {
		// Search for duplicate frames
		for (j = 0; j < i; j++) {
			if (memcmp(&lpImageData[i * wMaxWidth * wMaxHeight],
			           &lpImageData[j * wMaxWidth * wMaxHeight],
					   wMaxWidth * wMaxHeight * sizeof(short)) == 0)
				break;
		}

		if (j < i) {
			memcpy(&lpFrameHeaders[i], &lpFrameHeaders[j], sizeof(FRAMEHEADER));
			lpFrameData[i].lpRowOffsets = 0;
			continue;
		}

		if (i == 0) {
			lpFrameHeaders[i].Offset = sizeof(GRPHEADER) + nFrames * sizeof(FRAMEHEADER);
		}
		else {
			lpFrameHeaders[i].Offset = nLastOffset;
		}

		// Scan frame to find dimensions of used part
		x1 = y1 = 0x10000;
		x2 = y2 = -1;
		for (y = 0; y < wMaxHeight; y++) {
			for (x = 0; x < wMaxWidth; x++) {
				if (lpImageData[i * wMaxWidth * wMaxHeight + y * wMaxWidth + x] >= 0) {
					if (x < x1) x1 = x;
					if (x > x2) x2 = x;
					if (y < y1) y1 = y;
					if (y > y2) y2 = y;
				}
			}
		}
		lpFrameHeaders[i].Left = x1;
		lpFrameHeaders[i].Top = y1;
		lpFrameHeaders[i].Width = x2 - x1 + 1;
		lpFrameHeaders[i].Height = y2 - y1 + 1;

		if (!bNoCompress) {
			EncodeFrameData(lpImageData, i, &GrpHeader, &lpFrameHeaders[i], &lpFrameData[i]);

			y = lpFrameHeaders[i].Height;
			if (y > 0) {
				y--;
				nLastOffset = lpFrameHeaders[i].Offset + lpFrameData[i].lpRowOffsets[y] + lpFrameData[i].lpRowSizes[y];
			}
			else {
				nLastOffset = lpFrameHeaders[i].Offset;
			}
		}
		else {
			nLastOffset = lpFrameHeaders[i].Offset + lpFrameHeaders[i].Width * lpFrameHeaders[i].Height;
		}
	}

	lpGrpData = (LPBYTE)malloc(nLastOffset);

	// Write completed GRP to buffer
	memcpy(lpGrpData, &GrpHeader, sizeof(GRPHEADER));
	memcpy(lpGrpData + sizeof(GRPHEADER), lpFrameHeaders, nFrames * sizeof(FRAMEHEADER));

	for (i = 0; i < nFrames; i++) {
		if (lpFrameData[i].lpRowOffsets) {
			if (!bNoCompress) {
				memcpy(lpGrpData + lpFrameHeaders[i].Offset, lpFrameData[i].lpRowOffsets, lpFrameHeaders[i].Height * sizeof(WORD));

				for (y = 0; y < lpFrameHeaders[i].Height; y++) {
					if (lpFrameData[i].lpRowData[y]) {
						memcpy(lpGrpData + lpFrameHeaders[i].Offset + lpFrameData[i].lpRowOffsets[y], lpFrameData[i].lpRowData[y], lpFrameData[i].lpRowSizes[y]);
						free(lpFrameData[i].lpRowData[y]);
					}
				}

				free(lpFrameData[i].lpRowOffsets);
				free(lpFrameData[i].lpRowSizes);
				free(lpFrameData[i].lpRowData);
			}
			else {
				for (y = 0; y < lpFrameHeaders[i].Height; y++) {
					for (x = 0; x < lpFrameHeaders[i].Width; x++) {
						lpGrpData[lpFrameHeaders[i].Offset + y * lpFrameHeaders[i].Width + x] =
							lpImageData[i * wMaxWidth * wMaxHeight + (lpFrameHeaders[i].Top + y) * wMaxWidth + lpFrameHeaders[i].Left + x];
					}
				}
			}
		}
	}

	free(lpFrameHeaders);
	free(lpFrameData);

	*nGrpSize = nLastOffset;
	return (HANDLE)lpGrpData;
}

void EncodeFrameData(signed short *lpImageData, WORD nFrame, GRPHEADER *lpGrpHeader, FRAMEHEADER *lpFrameHeader, FRAMEDATA *lpFrameData)
{
	int x, y, i, j, nBufPos, nRepeat;
	LPBYTE lpRowBuf;
	WORD nLastOffset;

	lpFrameData->lpRowOffsets = (WORD *)malloc(lpFrameHeader->Height * sizeof(WORD));
	lpFrameData->lpRowSizes = (WORD *)malloc(lpFrameHeader->Height * sizeof(WORD));
	lpFrameData->lpRowData = (LPBYTE *)malloc(lpFrameHeader->Height * sizeof(LPBYTE));
	lpRowBuf = (LPBYTE)malloc(lpFrameHeader->Width * 2);

	for (y = 0; y < lpFrameHeader->Height; y++) {
		i = nFrame * lpGrpHeader->wMaxWidth * lpGrpHeader->wMaxHeight + (lpFrameHeader->Top + y) * lpGrpHeader->wMaxWidth;

		/* Fails verification when using this.  Bug in encoder or decoder?  Doesn't provide much benefit.
		// Search for duplicate rows (experimental)
		for (x = 0; x < y; x++) {
			j = nFrame * lpGrpHeader->wMaxWidth * lpGrpHeader->wMaxHeight + (lpFrameHeader->Top + x) * lpGrpHeader->wMaxWidth;
			if (memcmp(&lpImageData[i+lpFrameHeader->Left],
			           &lpImageData[j+lpFrameHeader->Left],
			           lpGrpHeader->wMaxWidth * sizeof(short)) == 0)
				break;
		}

		if (x < y) {
			lpFrameData->lpRowOffsets[y] = lpFrameData->lpRowOffsets[x];
			lpFrameData->lpRowSizes[y] = 0;
			lpFrameData->lpRowData[y] = 0;

#ifdef _DEBUG
			if (!VerifyRow(&lpImageData[i+lpFrameHeader->Left], lpFrameHeader->Width, lpFrameData->lpRowData[x], lpFrameData->lpRowSizes[x])) {
				nBufPos = nBufPos;
			}
#endif

			continue;
		}
		*/

		nBufPos = 0;
		if (lpFrameHeader->Width > 0) {
			for (x = lpFrameHeader->Left; x < lpFrameHeader->Left + lpFrameHeader->Width; x++) {
				if (x < lpFrameHeader->Left + lpFrameHeader->Width - 1) {
					if (lpImageData[i+x] < 0) {
						lpRowBuf[nBufPos] = 0x80;
						for (; lpImageData[i+x] < 0 && x < lpFrameHeader->Left + lpFrameHeader->Width && lpRowBuf[nBufPos] < 0xFF; x++) {
							lpRowBuf[nBufPos]++;
						}
						x--;
						nBufPos++;
						continue;
					}

					// Count repeating pixels, nRepeat = number of pixels - 1, ignore if there are less than 4 duplicates
					for (nRepeat = 0; lpImageData[i+x+nRepeat] == lpImageData[i+x+nRepeat+1] && x+nRepeat < lpFrameHeader->Left + lpFrameHeader->Width - 1 && nRepeat < 0x3E; nRepeat++) {}

					if (nRepeat > 2) {
						lpRowBuf[nBufPos] = 0x41 + nRepeat;
						lpRowBuf[nBufPos+1] = (BYTE)lpImageData[i+x];
						x += nRepeat;
						nBufPos += 2;
					}
					else {
						lpRowBuf[nBufPos] = 0;
						for (; lpImageData[i+x] >= 0 && x < lpFrameHeader->Left + lpFrameHeader->Width && lpRowBuf[nBufPos] < 0x3F; x++) {
							// Count repeating pixels, ignore if there are less than 4 duplicates
							for (nRepeat = 0; lpImageData[i+x+nRepeat] == lpImageData[i+x+nRepeat+1] && x+nRepeat < lpFrameHeader->Left + lpFrameHeader->Width - 1 && nRepeat < 3; nRepeat++) {}
							if (nRepeat > 2) break;

							lpRowBuf[nBufPos]++;
							lpRowBuf[nBufPos+lpRowBuf[nBufPos]] = (BYTE)lpImageData[i+x];
						}
						if (lpImageData[i+x] >= 0 && x == lpFrameHeader->Left + lpFrameHeader->Width - 1 && lpRowBuf[nBufPos] < 0x3F) {
							lpRowBuf[nBufPos]++;
							lpRowBuf[nBufPos+lpRowBuf[nBufPos]] = (BYTE)lpImageData[i+x];
						}
						x--;
						nBufPos += 1 + lpRowBuf[nBufPos];
					}
				}
				else {
					if (lpImageData[i+x] < 0) {
						lpRowBuf[nBufPos] = 0x81;
						nBufPos++;
					}
					else {
						lpRowBuf[nBufPos] = 1;
						lpRowBuf[nBufPos+1] = (BYTE)lpImageData[i+x];
						nBufPos += 2;
					}
				}
			}
		}

#ifdef _DEBUG
		if (!VerifyRow(&lpImageData[i+lpFrameHeader->Left], lpFrameHeader->Width, lpRowBuf, nBufPos)) {
			nBufPos = nBufPos;
		}
#endif

		if (y == 0) {
			lpFrameData->lpRowOffsets[y] = lpFrameHeader->Height * sizeof(WORD);
		}
		else {
			lpFrameData->lpRowOffsets[y] = nLastOffset;
		}

		nLastOffset = lpFrameData->lpRowOffsets[y] + nBufPos;

		lpFrameData->lpRowSizes[y] = nBufPos;
		lpFrameData->lpRowData[y] = (LPBYTE)malloc(nBufPos);
		memcpy(lpFrameData->lpRowData[y], lpRowBuf, nBufPos);
	}

	free(lpRowBuf);
}

#ifdef _DEBUG
BOOL VerifyRow(signed short *lpRawRow, int nWidth, LPBYTE lpEncRow, int nSize)
{
	int i,x=0,ofs=0;
	while (x < nWidth && ofs < nSize) {
		if (!(lpEncRow[ofs] & 0x80)) {
			if (!(lpEncRow[ofs] & 0x40)) {
				for (i=1;i<=lpEncRow[ofs] && x<nWidth;i++) {
					if (lpEncRow[ofs+i] != (BYTE)lpRawRow[x]) return FALSE;
					x++;
				}
				ofs+=lpEncRow[ofs]+1;
			}
			else {
				for (i=0;i<lpEncRow[ofs]-64 && x<nWidth;i++) {
					if (lpEncRow[ofs+1] != (BYTE)lpRawRow[x]) return FALSE;
					x++;
				}
				ofs+=2;
			}
		}
		else {
			for (i=0;i<lpEncRow[ofs]-128 && x<nWidth;i++) {
				if (lpRawRow[x] >= 0) return FALSE;
			}
			x+=lpEncRow[ofs]-128;
			ofs++;
		}
	}

	if (x != nWidth || ofs != nSize) return FALSE;

	return TRUE;
}
#endif
