/*
	Storm.cpp
*/
/* License information for this code is in license.txt */

#include "storm.h"

struct STORMMODULE {
	STORMMODULE();
	~STORMMODULE();
} Storm;

void LoadStormFunctions();
void FreeStorm();

HINSTANCE hStorm = 0;
BOOL AlreadyLoaded = FALSE;

// Storm Function Names
BOOL  (WINAPI* SFileOpenArchive)(LPCSTR lpFilename, DWORD dwPriority, DWORD dwFlags, MPQHANDLE *hMPQ) = 0;
BOOL  (WINAPI* SFileCloseArchive)(MPQHANDLE hMPQ) = 0;
BOOL  (WINAPI* SFileOpenFile)(LPCSTR lpFileName, MPQHANDLE *hFile) = 0;
BOOL  (WINAPI* SFileOpenFileEx)(MPQHANDLE hMPQ, LPCSTR lpFileName, DWORD dwSearchScope, MPQHANDLE *hFile) = 0;
BOOL  (WINAPI* SFileCloseFile)(MPQHANDLE hFile) = 0;
DWORD (WINAPI* SFileGetFileSize)(MPQHANDLE hFile, LPDWORD lpFileSizeHigh) = 0;
DWORD (WINAPI* SFileSetFilePointer)(MPQHANDLE hFile, long lDistanceToMove, PLONG lplDistanceToMoveHigh, DWORD dwMoveMethod) = 0;
BOOL  (WINAPI* SFileReadFile)(MPQHANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = 0;
LCID  (WINAPI* SFileSetLocale)(LCID nNewLocale) = 0;
BOOL  (WINAPI* SFileDestroy)() = 0;
BOOL  (WINAPI* SFileGetArchiveName)(MPQHANDLE hMPQ, LPCSTR lpBuffer, DWORD dwBufferLength) = 0;
BOOL  (WINAPI* SFileGetFileName)(MPQHANDLE hFile, LPCSTR lpBuffer, DWORD dwBufferLength) = 0;
BOOL  (WINAPI* SFileGetFileArchive)(MPQHANDLE hFile, MPQHANDLE *hMPQ) = 0;
BOOL  (WINAPI* SFileGetBasePath)(LPCSTR lpBuffer, DWORD dwBufferLength) = 0;
BOOL  (WINAPI* SFileSetBasePath)(LPCSTR lpNewBasePath) = 0;

STORMMODULE::STORMMODULE()
{
	if (hStorm!=0) return;
	const char STORM_DLL[] = "Storm.dll";
	hStorm = GetModuleHandle(STORM_DLL);
	if (hStorm == NULL) hStorm = LoadLibrary(STORM_DLL);
	else AlreadyLoaded=TRUE;

	LoadStormFunctions();
	if (SFileDestroy && !AlreadyLoaded) SFileDestroy();
}

STORMMODULE::~STORMMODULE()
{
	FreeStorm();
}

HINSTANCE LoadStorm(char * DllFileName)
{
	FreeStorm();
	if (hStorm!=0) return 0;
	hStorm = GetModuleHandle(DllFileName);
	if (hStorm == NULL) hStorm = LoadLibrary(DllFileName);
	else AlreadyLoaded=TRUE;

	LoadStormFunctions();
	if (SFileDestroy && !AlreadyLoaded) SFileDestroy();
	if (AlreadyLoaded==TRUE) return 0;
	return hStorm;
}

void LoadStormFunctions()
{
	if (hStorm != NULL)
	{
		*(FARPROC *)&SFileCloseArchive = GetProcAddress(hStorm, (LPCSTR)0xFC);
		*(FARPROC *)&SFileCloseFile = GetProcAddress(hStorm, (LPCSTR)0xFD);
		*(FARPROC *)&SFileDestroy = GetProcAddress(hStorm, (LPCSTR)0x106);
		*(FARPROC *)&SFileGetFileArchive = GetProcAddress(hStorm, (LPCSTR)0x108);
		*(FARPROC *)&SFileGetFileSize = GetProcAddress(hStorm, (LPCSTR)0x109);
		*(FARPROC *)&SFileOpenArchive = GetProcAddress(hStorm, (LPCSTR)0x10A);
		*(FARPROC *)&SFileOpenFile = GetProcAddress(hStorm, (LPCSTR)0x10B);
		*(FARPROC *)&SFileOpenFileEx = GetProcAddress(hStorm, (LPCSTR)0x10C);
		*(FARPROC *)&SFileReadFile = GetProcAddress(hStorm, (LPCSTR)0x10D);
		*(FARPROC *)&SFileSetBasePath = GetProcAddress(hStorm, (LPCSTR)0x10E);
		*(FARPROC *)&SFileSetFilePointer = GetProcAddress(hStorm, (LPCSTR)0x10F);
		*(FARPROC *)&SFileSetLocale = GetProcAddress(hStorm, (LPCSTR)0x110);
		*(FARPROC *)&SFileGetBasePath = GetProcAddress(hStorm, (LPCSTR)0x111);
		*(FARPROC *)&SFileGetArchiveName = GetProcAddress(hStorm, (LPCSTR)0x113);
		*(FARPROC *)&SFileGetFileName = GetProcAddress(hStorm, (LPCSTR)0x114);
	}
}

void FreeStorm()
{
	SFileCloseArchive = 0;
	SFileCloseFile = 0;
	SFileDestroy = 0;
	SFileGetFileArchive = 0;
	SFileGetFileSize = 0;
	SFileOpenArchive = 0;
	SFileOpenFile = 0;
	SFileOpenFileEx = 0;
	SFileReadFile = 0;
	SFileSetBasePath = 0;
	SFileSetFilePointer = 0;
	SFileSetLocale = 0;
	SFileGetBasePath = 0;
	SFileGetArchiveName = 0;
	SFileGetFileName = 0;

	if (hStorm==0) return;

	if (AlreadyLoaded==FALSE) FreeLibrary(hStorm);
	hStorm = 0;
}
