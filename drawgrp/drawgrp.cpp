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

int main(int argc, char* argv[])
{
	HANDLE hMPQ=0,hMPQ2=0,hMPQ3=0;
	DWORD *dwPalette = (DWORD *)_alloca(1024);
	memset(dwPalette,0,1024);
	if (SFileOpenArchive!=0) {
		char *buffer = (char *)_alloca(13);
		memcpy(buffer,"StarDat.mpq",12);
		if (SFileOpenArchive(buffer,1000,0,&hMPQ)==0) return 0;
		memcpy(buffer,"BrooDat.mpq",12);
		SFileOpenArchive(buffer,2000,0,&hMPQ2);
		memcpy(buffer,"Patch_rt.mpq",13);
		SFileOpenArchive(buffer,3000,0,&hMPQ3);
	}
	LoadPalette("tileset\\Jungle.wpe",dwPalette);
	HANDLE hGrp;
	if (argc>1)
		hGrp = LoadGrp(argv[1]);
	else
		hGrp = LoadGrp("unit\\zerg\\ultra.grp");
	HDC hDC = GetDC(0);
	GRPHEADER GrpInfo;
	if (GetGrpInfo(hGrp,&GrpInfo)==0) {GrpInfo.nFrames=0;GrpInfo.wMaxWidth=0;GrpInfo.wMaxHeight=0;}
	//for (DWORD j=0;j<16;j++){
	/*for (WORD i=0;i<GrpInfo.nFrames;i++) {
		DrawGrp(hGrp,hDC,0,0,i,dwPalette,ALPHA_BLEND,0x401020);
	}*/
	for (WORD i=8;i<400;i+=17) {
		for (DWORD j=0;j<8;j++)
		DrawGrp(hGrp,hDC,i,0,i % (17*8),dwPalette,ALPHA_BLEND,0x401020);
	}
	for (i=8;i<400;i+=17) {
		for (DWORD j=0;j<8;j++)
		DrawGrp(hGrp,hDC,400-i,0,i % (17*8),dwPalette,HORIZONTAL_FLIP|ALPHA_BLEND,0x401020);
	}
	//}
	ReleaseDC(0,hDC);
    DestroyGrp(hGrp);
	if (SFileCloseArchive!=0) {
		if (hMPQ3!=0) SFileCloseArchive(hMPQ3);
		if (hMPQ2!=0) SFileCloseArchive(hMPQ2);
		if (hMPQ!=0) SFileCloseArchive(hMPQ);
	}
	return 0;
}

