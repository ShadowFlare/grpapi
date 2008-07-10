// drawgrp.cpp : Defines the entry point for the console application.
//

#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
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
	signed short *pBuffer;
	WORD nFrame;
};

COLORREF WINAPI ReadPixelFromBuffer(BufferInfo *pBI, int X, int Y)
{
	if (pBI->nFrame == 0xFFFF)
		return pBI->pBuffer[(Y * pBI->nWidth) + X];
	else
		return pBI->pBuffer[(pBI->nFrame * pBI->nWidth * pBI->nHeight) + (Y * pBI->nWidth) + X];
}

void WINAPI WritePixelToBuffer(BufferInfo *pBI, int X, int Y, COLORREF clrColor)
{
	if (pBI->nFrame == 0xFFFF)
		pBI->pBuffer[(Y * pBI->nWidth) + X] = (signed short)clrColor;
	else
		//if (clrColor < 1 || clrColor > 13)
		//	pBI->pBuffer[(pBI->nFrame * pBI->nWidth * pBI->nHeight) + (Y * pBI->nWidth) + X] = (signed short)((BYTE)-clrColor);
		//else
			pBI->pBuffer[(pBI->nFrame * pBI->nWidth * pBI->nHeight) + (Y * pBI->nWidth) + X] = (signed short)clrColor;
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
	BufferInfo BI;
	LoadPalette("tileset\\Platform.wpe",dwPalette);
	HANDLE hGrp, hGrp2;
	if (argc>1)
		hGrp = LoadGrp(argv[1]);
	else
		hGrp = LoadGrp("unit\\zerg\\zergling.grp");
	HDC hDC = GetDC(0);
	GRPHEADER GrpInfo;
	if (GetGrpInfo(hGrp,&GrpInfo)==0) {GrpInfo.nFrames=0;GrpInfo.wMaxWidth=0;GrpInfo.wMaxHeight=0;}
	BI.nWidth = GrpInfo.wMaxWidth;
	BI.nHeight = GrpInfo.wMaxHeight;
	//BI.nWidth = 255;
	//BI.nHeight = 255;
	BI.pBuffer = (signed short *)malloc(GrpInfo.nFrames * BI.nWidth * BI.nHeight * sizeof(short));
	WORD i,x,y;
	DWORD j, nGrpSize;
	unsigned int u,v;
	memset(BI.pBuffer, 0xFF, GrpInfo.nFrames * BI.nWidth * BI.nHeight * sizeof(short));
	//for (DWORD j=0;j<16;j++){
	/*for (WORD i=0;i<GrpInfo.nFrames;i++) {
		DrawGrp(hGrp,hDC,0,0,i,dwPalette,ALPHA_BLEND,0x401020);
	}*/
	signed short clrPixel;
	RECT rect;
	/*for (i=0;i<GrpInfo.nFrames;i++) {
		for (j=0;j<16;j++)
		{
			rand_s(&u);
			rand_s(&v);
			u = u % 800;
			v = v % 600;
			DrawGrp(hGrp,hDC,u,v,i,dwPalette,ALPHA_BLEND,0x404040);
		}
	}*/
	SetFunctionGetPixel((GETPIXELPROC)ReadPixelFromBuffer);
	SetFunctionSetPixel((SETPIXELPROC)WritePixelToBuffer);
	for (i=0;i<GrpInfo.nFrames;i++) {
		BI.nFrame = i;
		u = (BI.nWidth - GrpInfo.wMaxWidth) / 2;
		v = (BI.nHeight - GrpInfo.wMaxHeight) / 2;
		for (y = 0; y < BI.nHeight; y++) {
			for (x = 0; x < BI.nWidth; x++) {
				WritePixelToBuffer(&BI, x, y, -1);
			}
		}
		DrawGrp(hGrp,(HDC)&BI,u,v,i,0,USE_INDEX,0);
	}
	hGrp2 = hGrp;
	hGrp = CreateGrp(BI.pBuffer, GrpInfo.nFrames, BI.nWidth, BI.nHeight, FALSE, &nGrpSize);
	/*HANDLE hFile;
	hFile = CreateFile("generated zergling.grp", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, hGrp, nGrpSize, &j, 0);
		CloseHandle(hFile);
	}*/
	BI.nFrame = 0xFFFF;
	j=0;
	
	for (i=0;i<GrpInfo.nFrames;i+=1) {
		rect.left = rect.top = 0;
		rect.right = rect.left + BI.nWidth;
		rect.bottom = rect.top + BI.nHeight;
		for (x = 0; x < BI.nWidth * BI.nHeight; x++)
			BI.pBuffer[x] = -1;
		//for (j=0;j<(1 * 1);j++) {
//			DrawGrp(hGrp,(HDC)&BI,i,0,i % (17*8),dwPalette,ALPHA_BLEND,0x401020);
			//DrawGrp(hGrp2,(HDC)&BI,0,0,i,0,USE_INDEX,0x401020);
			//u = memcmp(BI.pBuffer, &BI.pBuffer[i * BI.nWidth * BI.nHeight], BI.nWidth * BI.nHeight * sizeof(short));
			DrawGrp(hGrp,(HDC)&BI,0,0,i,0,USE_INDEX,0x401020);
			u = memcmp(BI.pBuffer, &BI.pBuffer[i * BI.nWidth * BI.nHeight], BI.nWidth * BI.nHeight * sizeof(short));
			for (x = 0; x < BI.nWidth; x++)
				for (y = 0; y < BI.nHeight; y++) {
					clrPixel = BI.pBuffer[(y * BI.nWidth) + x];
					if (clrPixel != -1) SetPixelV(hDC, x, y, dwPalette[clrPixel]);
				}
		//}
		if (u) {
			printf("Output of re-encoded graphic for frame %d does not match original!  Total %d\n", i, ++j);
			//break;
		}
		FillRect(hDC, &rect, (HBRUSH) (COLOR_WINDOW+1));
	}
	for (i=4;i<400;i+=17) {
		rect.left = rect.top = 400-i;
		rect.right = rect.left + BI.nWidth;
		rect.bottom = rect.top + BI.nHeight;
		for (x = 0; x < BI.nWidth * BI.nHeight; x++)
			BI.pBuffer[x] = -1;
		for (j=0;j<32;j++) {
//			DrawGrp(hGrp,(HDC)&BI,400-i,0,i % (17*8),dwPalette,HORIZONTAL_FLIP|ALPHA_BLEND,0x401020);
			DrawGrp(hGrp,(HDC)&BI,0,0,i % (17*8),0,HORIZONTAL_FLIP|USE_INDEX,0x401020);
			for (x = 0; x < BI.nWidth; x++)
				for (y = 0; y < BI.nHeight; y++) {
					clrPixel = BI.pBuffer[(y * BI.nWidth) + x];
					if (clrPixel != -1) SetPixelV(hDC, 400-i + x, 400-i + y, dwPalette[clrPixel]);
				}
		}
		FillRect(hDC, &rect, (HBRUSH) (COLOR_WINDOW+1));
	}
	for (i = 0; i < BI.nWidth; i++)
		for (j = 0; j < BI.nHeight; j++) {
			clrPixel = BI.pBuffer[(j * BI.nWidth) + i];
			if (clrPixel != -1) SetPixelV(hDC, 400 + i, 300 + j, dwPalette[clrPixel]);
		}
	//}
	
	ReleaseDC(0,hDC);
	free(BI.pBuffer);
    DestroyGrp(hGrp);
    DestroyGrp(hGrp2);
	if (SFileCloseArchive!=0) {
		if (hMPQ3!=0) SFileCloseArchive(hMPQ3);
		if (hMPQ2!=0) SFileCloseArchive(hMPQ2);
		if (hMPQ!=0) SFileCloseArchive(hMPQ);
	}
	return 0;
}

