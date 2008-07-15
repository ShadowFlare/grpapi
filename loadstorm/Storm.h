/* License information for this code is in license.txt */

#ifndef STORM_H_INCLUDED
#define STORM_H_INCLUDED

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// This no longer needs to be called
extern HINSTANCE LoadStorm(char * DllFileName);

#define SFILE_OPEN_HARD_DISK_FILE 0x0000 //Open archive without regard to the drive type it resides on
#define SFILE_OPEN_CD_ROM_FILE    0x0001 //Open the archive only if it is on a CD-ROM

#define SFILE_SEARCH_CURRENT_ONLY 0x00 //Used with SFileOpenFileEx; only the archive with the handle specified will be searched for the file
#define SFILE_SEARCH_ALL_OPEN     0x01 //SFileOpenFileEx will look through all open archives for the file

#define MPQHANDLE HANDLE

// Storm Function Prototypes
extern BOOL  (WINAPI* SFileOpenArchive)(LPCSTR lpFilename, DWORD dwPriority, DWORD dwFlags, MPQHANDLE *hMPQ);
extern BOOL  (WINAPI* SFileCloseArchive)(MPQHANDLE hMPQ);
extern BOOL  (WINAPI* SFileOpenFile)(LPCSTR lpFileName, MPQHANDLE *hFile);
extern BOOL  (WINAPI* SFileOpenFileEx)(MPQHANDLE hMPQ, LPCSTR lpFileName, DWORD dwSearchScope, MPQHANDLE *hFile);
extern BOOL  (WINAPI* SFileCloseFile)(MPQHANDLE hFile);
extern DWORD (WINAPI* SFileGetFileSize)(MPQHANDLE hFile, LPDWORD lpFileSizeHigh);
extern DWORD (WINAPI* SFileSetFilePointer)(MPQHANDLE hFile, long lDistanceToMove, PLONG lplDistanceToMoveHigh, DWORD dwMoveMethod);
extern BOOL  (WINAPI* SFileReadFile)(MPQHANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
extern LCID  (WINAPI* SFileSetLocale)(LCID nNewLocale);
extern BOOL  (WINAPI* SFileDestroy)();
extern BOOL  (WINAPI* SFileGetArchiveName)(MPQHANDLE hMPQ, LPCSTR lpBuffer, DWORD dwBufferLength);
extern BOOL  (WINAPI* SFileGetFileName)(MPQHANDLE hFile, LPCSTR lpBuffer, DWORD dwBufferLength);
extern BOOL  (WINAPI* SFileGetFileArchive)(MPQHANDLE hFile, MPQHANDLE *hMPQ);
extern BOOL  (WINAPI* SFileGetBasePath)(LPCSTR lpBuffer, DWORD dwBufferLength);
extern BOOL  (WINAPI* SFileSetBasePath)(LPCSTR lpNewBasePath);

#ifdef __cplusplus
};  // extern "C" 
#endif

#endif
