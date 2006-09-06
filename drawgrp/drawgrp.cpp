// drawgrp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*int qmain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	HINSTANCE hStorm = LoadStorm("Storm.dll");
	if (SFileDestroy!=0) SFileDestroy();
	LoadGrpApi();
	int RetVal = qmain(argc,argv);
	FreeGrpApi();
	if (hStorm!=0) FreeLibrary(hStorm);
	return RetVal;
}*/

struct BufferInfo {
	WORD nWidth;
	WORD nHeight;
	LPDWORD pBuffer;
};

COLORREF WINAPI ReadPixelFromBuffer(BufferInfo *pBI, int X, int Y)
{
	return pBI->pBuffer[(Y * pBI->nWidth) + X];
}

void WINAPI WritePixelToBuffer(BufferInfo *pBI, int X, int Y, COLORREF clrColor)
{
	pBI->pBuffer[(Y * pBI->nWidth) + X] = clrColor;
}

int main(int argc, char* argv[])
{
	HANDLE hMPQ=0,hMPQ2=0,hMPQ3=0;
	DWORD *dwPalette = (DWORD *)_alloca(1024);
	memset(dwPalette,0,1024);
	LoadStorm("SFMpq.dll");
	SetMpqDll("SFMpq.dll");
	if (SFileOpenArchive!=0) {
		char *buffer = (char *)_alloca(13);
		memcpy(buffer,"StarDat.mpq",12);
		if (SFileOpenArchive(buffer,1000,0,&hMPQ)==0) return 0;
		memcpy(buffer,"BrooDat.mpq",12);
		SFileOpenArchive(buffer,2000,0,&hMPQ2);
		memcpy(buffer,"Patch_rt.mpq",13);
		SFileOpenArchive(buffer,3000,0,&hMPQ3);
	}
	SetFunctionGetPixel((GETPIXELPROC)ReadPixelFromBuffer);
	SetFunctionSetPixel((SETPIXELPROC)WritePixelToBuffer);
	BufferInfo BI;
	LoadPalette("tileset\\Jungle.wpe",dwPalette);
	HANDLE hGrp;
	if (argc>1)
		hGrp = LoadGrp(argv[1]);
	else
		hGrp = LoadGrp("unit\\zerg\\ultra.grp");
	HDC hDC = GetDC(0);
	GRPHEADER GrpInfo;
	if (GetGrpInfo(hGrp,&GrpInfo)==0) {GrpInfo.nFrames=0;GrpInfo.wMaxWidth=0;GrpInfo.wMaxHeight=0;}
	BI.nWidth = GrpInfo.wMaxWidth;
	BI.nHeight = GrpInfo.wMaxHeight;
	BI.pBuffer = (DWORD *)malloc(BI.nWidth * BI.nHeight * sizeof(DWORD));
	WORD i,x,y;
	DWORD j;
	for (i = 0; i < BI.nWidth * BI.nHeight; i++)
		BI.pBuffer[i] = 0xFFFFFFFF;
	//for (DWORD j=0;j<16;j++){
	/*for (WORD i=0;i<GrpInfo.nFrames;i++) {
		DrawGrp(hGrp,hDC,0,0,i,dwPalette,ALPHA_BLEND,0x401020);
	}*/
	COLORREF clrPixel;
	RECT rect;
	for (i=13;i<400;i+=17) {
		rect.left = rect.top = i;
		rect.right = rect.left + BI.nWidth;
		rect.bottom = rect.top + BI.nHeight;
		for (x = 0; x < BI.nWidth * BI.nHeight; x++)
			BI.pBuffer[x] = 0xFFFFFFFF;
		for (j=0;j<32;j++) {
//			DrawGrp(hGrp,(HDC)&BI,i,0,i % (17*8),dwPalette,ALPHA_BLEND,0x401020);
			DrawGrp(hGrp,(HDC)&BI,0,0,i % (17*8),dwPalette,USE_INDEX,0x401020);
			for (x = 0; x < BI.nWidth; x++)
				for (y = 0; y < BI.nHeight; y++) {
					clrPixel = BI.pBuffer[(y * BI.nWidth) + x];
					if (clrPixel != 0xFFFFFFFF) SetPixelV(hDC, i + x, i + y, dwPalette[clrPixel]);
				}
		}
		FillRect(hDC, &rect, (HBRUSH) (COLOR_WINDOW+1));
	}
	for (i=4;i<400;i+=17) {
		rect.left = rect.top = 400-i;
		rect.right = rect.left + BI.nWidth;
		rect.bottom = rect.top + BI.nHeight;
		for (x = 0; x < BI.nWidth * BI.nHeight; x++)
			BI.pBuffer[x] = 0xFFFFFFFF;
		for (j=0;j<32;j++) {
//			DrawGrp(hGrp,(HDC)&BI,400-i,0,i % (17*8),dwPalette,HORIZONTAL_FLIP|ALPHA_BLEND,0x401020);
			DrawGrp(hGrp,(HDC)&BI,0,0,i % (17*8),dwPalette,HORIZONTAL_FLIP|USE_INDEX,0x401020);
			for (x = 0; x < BI.nWidth; x++)
				for (y = 0; y < BI.nHeight; y++) {
					clrPixel = BI.pBuffer[(y * BI.nWidth) + x];
					if (clrPixel != 0xFFFFFFFF) SetPixelV(hDC, 400-i + x, 400-i + y, dwPalette[clrPixel]);
				}
		}
		FillRect(hDC, &rect, (HBRUSH) (COLOR_WINDOW+1));
	}
	for (i = 0; i < BI.nWidth; i++)
		for (j = 0; j < BI.nHeight; j++) {
			clrPixel = BI.pBuffer[(j * BI.nWidth) + i];
			if (clrPixel != 0xFFFFFFFF) SetPixelV(hDC, 400 + i, 300 + j, dwPalette[clrPixel]);
		}
	//}
	ReleaseDC(0,hDC);
	free(BI.pBuffer);
    DestroyGrp(hGrp);
	if (SFileCloseArchive!=0) {
		if (hMPQ3!=0) SFileCloseArchive(hMPQ3);
		if (hMPQ2!=0) SFileCloseArchive(hMPQ2);
		if (hMPQ!=0) SFileCloseArchive(hMPQ);
	}
	return 0;
}

