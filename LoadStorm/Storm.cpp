/*
	Storm.cpp
*/

#include "storm.h"

struct STORMMODULE {
	STORMMODULE();
	~STORMMODULE();
} Storm;

HINSTANCE hStorm = 0;
BOOL AlreadyLoaded = FALSE;

// Storm Function Names
funcSFileCloseArchive SFileCloseArchive = 0;
funcSFileCloseFile SFileCloseFile = 0;
funcSFileDestroy SFileDestroy = 0;
funcSFileGetFileArchive SFileGetFileArchive = 0;
funcSFileGetFileSize SFileGetFileSize = 0;
funcSFileOpenArchive SFileOpenArchive = 0;
funcSFileOpenFile SFileOpenFile = 0;
funcSFileOpenFileEx SFileOpenFileEx = 0;
funcSFileReadFile SFileReadFile = 0;
funcSFileSetBasePath SFileSetBasePath = 0;
funcSFileSetFilePointer SFileSetFilePointer = 0;
funcSFileSetLocale SFileSetLocale = 0;
funcSFileGetBasePath SFileGetBasePath = 0;
funcSFileGetArchiveName SFileGetArchiveName = 0;
funcSFileGetFileName SFileGetFileName = 0;

STORMMODULE::STORMMODULE()
{
	if (hStorm!=0) return;
	const char STORM_DLL[] = "Storm.dll";
	hStorm = GetModuleHandle(STORM_DLL);
	if (hStorm == NULL) hStorm = LoadLibrary(STORM_DLL);
	else AlreadyLoaded=TRUE;

	if (hStorm != NULL)
	{
		SFileCloseArchive = (funcSFileCloseArchive)GetProcAddress(hStorm, (LPCSTR)0xFC);
		SFileCloseFile = (funcSFileCloseFile)GetProcAddress(hStorm, (LPCSTR)0xFD);
		SFileDestroy = (funcSFileDestroy)GetProcAddress(hStorm, (LPCSTR)0x106);
		SFileGetFileArchive = (funcSFileGetFileArchive)GetProcAddress(hStorm, (LPCSTR)0x108);
		SFileGetFileSize = (funcSFileGetFileSize)GetProcAddress(hStorm, (LPCSTR)0x109);
		SFileOpenArchive = (funcSFileOpenArchive)GetProcAddress(hStorm, (LPCSTR)0x10A);
		SFileOpenFile = (funcSFileOpenFile)GetProcAddress(hStorm, (LPCSTR)0x10B);
		SFileOpenFileEx = (funcSFileOpenFileEx)GetProcAddress(hStorm, (LPCSTR)0x10C);
		SFileReadFile = (funcSFileReadFile)GetProcAddress(hStorm, (LPCSTR)0x10D);
		SFileSetBasePath = (funcSFileSetBasePath)GetProcAddress(hStorm, (LPCSTR)0x10E);
		SFileSetFilePointer = (funcSFileSetFilePointer)GetProcAddress(hStorm, (LPCSTR)0x10F);
		SFileSetLocale = (funcSFileSetLocale)GetProcAddress(hStorm, (LPCSTR)0x110);
		SFileGetBasePath = (funcSFileGetBasePath)GetProcAddress(hStorm, (LPCSTR)0x111);
		SFileGetArchiveName = (funcSFileGetArchiveName)GetProcAddress(hStorm, (LPCSTR)0x113);
		SFileGetFileName = (funcSFileGetFileName)GetProcAddress(hStorm, (LPCSTR)0x114);
	}
	if (SFileDestroy && !AlreadyLoaded) SFileDestroy();
}

STORMMODULE::~STORMMODULE()
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

HINSTANCE LoadStorm(char * DllFileName)
{
	if (hStorm!=0) return 0;
	hStorm = GetModuleHandle(DllFileName);
	if (hStorm == NULL) hStorm = LoadLibrary(DllFileName);
	else AlreadyLoaded=TRUE;

	if (hStorm != NULL)
	{
		SFileCloseArchive = (funcSFileCloseArchive)GetProcAddress(hStorm, (LPCSTR)0xFC);
		SFileCloseFile = (funcSFileCloseFile)GetProcAddress(hStorm, (LPCSTR)0xFD);
		SFileDestroy = (funcSFileDestroy)GetProcAddress(hStorm, (LPCSTR)0x106);
		SFileGetFileArchive = (funcSFileGetFileArchive)GetProcAddress(hStorm, (LPCSTR)0x108);
		SFileGetFileSize = (funcSFileGetFileSize)GetProcAddress(hStorm, (LPCSTR)0x109);
		SFileOpenArchive = (funcSFileOpenArchive)GetProcAddress(hStorm, (LPCSTR)0x10A);
		SFileOpenFile = (funcSFileOpenFile)GetProcAddress(hStorm, (LPCSTR)0x10B);
		SFileOpenFileEx = (funcSFileOpenFileEx)GetProcAddress(hStorm, (LPCSTR)0x10C);
		SFileReadFile = (funcSFileReadFile)GetProcAddress(hStorm, (LPCSTR)0x10D);
		SFileSetBasePath = (funcSFileSetBasePath)GetProcAddress(hStorm, (LPCSTR)0x10E);
		SFileSetFilePointer = (funcSFileSetFilePointer)GetProcAddress(hStorm, (LPCSTR)0x10F);
		SFileSetLocale = (funcSFileSetLocale)GetProcAddress(hStorm, (LPCSTR)0x110);
		SFileGetBasePath = (funcSFileGetBasePath)GetProcAddress(hStorm, (LPCSTR)0x111);
		SFileGetArchiveName = (funcSFileGetArchiveName)GetProcAddress(hStorm, (LPCSTR)0x113);
		SFileGetFileName = (funcSFileGetFileName)GetProcAddress(hStorm, (LPCSTR)0x114);
	}
	if (SFileDestroy && !AlreadyLoaded) SFileDestroy();
	if (AlreadyLoaded==TRUE) return 0;
	return hStorm;
}

