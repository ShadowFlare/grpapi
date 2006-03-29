/*

  ShadowFlare GRP Library. (c) ShadowFlare Software 2002-2006

  Any comments or suggestions are accepted at blakflare@hotmail.com (ShadowFlare)
*/

#ifndef GRPAPI_INCLUDED
#define GRPAPI_INCLUDED

#include <windows.h>

#ifndef GRPAPI_STATIC

#ifdef GRPAPI_EXPORTS
#define GRPAPI __declspec(dllexport)
#else
#define GRPAPI __declspec(dllimport)
#endif

#else
#define GRPAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	WORD nFrames;
	WORD wMaxWidth;
	WORD wMaxHeight;
} GRPHEADER;

#define HORIZONTAL_FLIP 0x00000001 // Flips the graphic horizontally
#define VERTICAL_FLIP   0x00000002 // Flips the graphic vertically
#define SHADOW_COLOR    0x00000004 // Causes the graphic to be drawn in one color
                                   // Second byte of flags is the red component of
                                   // the shadow's color, third byte is green,
                                   // fourth byte is blue; like this:
                                   // SHADOW_COLOR|0xBBGGRR00
#define ALPHA_BLEND     0x00000008 // Blends the graphic with what it is being drawn over.
                                   // The dwAlpha parameter will only be used when this
                                   // flag is specified.  dwAlpha is an RGB value
                                   // (0xBBGGRR).
                                   // Note: Because of the extra calculations required,
                                   // alpha blended graphics take longer to draw

// Palette is an array of 256 DWORD's
// For LoadPalette and LoadGrp, lpFileName may be a file in an open mpq archive
// or a file not in an archive
BOOL   GRPAPI WINAPI LoadPalette(LPCSTR lpFileName, DWORD *dwPaletteBuffer);
HANDLE GRPAPI WINAPI LoadGrp(LPCSTR lpFileName);
BOOL   GRPAPI WINAPI DestroyGrp(HANDLE hGrp);
BOOL   GRPAPI WINAPI DrawGrp(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha);
BOOL   GRPAPI WINAPI GetGrpInfo(HANDLE hGrp, GRPHEADER *GrpInfo);

typedef COLORREF (WINAPI* GETPIXELPROC)(
  HDC hDC, // same value as hdcDest from DrawGrp,
	       // does not need to be used as an HDC,
	       // can be used for any other type of pointer
  int X,   // x-coordinate of pixel
  int Y    // y-coordinate of pixel
);
typedef void (WINAPI* SETPIXELPROC)(
	HDC hDC,          // same value as hdcDest from DrawGrp,
	                  // does not need to be used as an HDC,
	                  // can be used for any other type of pointer
	int X,            // x-coordinate of pixel
	int Y,            // y-coordinate of pixel
	COLORREF clrColor // new pixel color
);

// Call these to have DrawGrp use custom functions for reading and drawing pixels
// so that you can have it read from and write to a buffer, for example.
void GRPAPI WINAPI SetFunctionGetPixel(GETPIXELPROC lpGetPixelProc);
void GRPAPI WINAPI SetFunctionSetPixel(SETPIXELPROC lpSetPixelProc);

// Call this to make a different Storm.dll-compatible MPQ library be used (like SFMPQ).
BOOL GRPAPI WINAPI SetMpqDll(LPCSTR lpDllFileName);

// These no longer need to be called
BOOL   GRPAPI WINAPI LoadGrpApi();
void   GRPAPI WINAPI FreeGrpApi();

#ifdef __cplusplus
};  // extern "C" 
#endif

#endif

