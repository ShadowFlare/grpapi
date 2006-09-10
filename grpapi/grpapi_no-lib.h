/*

  ShadowFlare GRP Library. (c) ShadowFlare Software 2002

  Any comments or suggestions are accepted at blakflare@hotmail.com (ShadowFlare)
*/

#ifndef GRPAPI_INCLUDED
#define GRPAPI_INCLUDED

#include <windows.h>

#ifdef GRPAPI_EXPORTS
#define GRPAPI __declspec(dllexport)
#else
#define GRPAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// These no longer need to be called
extern void LoadGrpApiLib();
extern void FreeGrpApiLib();

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
                                   // This can be accomplished by using the left shift
                                   // operator like this:  SHADOW_COLOR|(color << 8)
#define ALPHA_BLEND     0x00000008 // Blends the graphic with what it is being drawn over.
                                   // The dwAlpha parameter will only be used when this
                                   // flag is specified.  dwAlpha is an RGB value
                                   // (0xBBGGRR).
                                   // Note: Because of the extra calculations required,
                                   // alpha blended graphics take longer to draw
#define USE_INDEX       0x00000010 // Only valid when used with a custom SetPixel function.
                                   // This flag cannot be used in combination with
                                   // ALPHA_BLEND or SHADOW_COLOR
                                   // When this flag is used, the index to a color in the
                                   // palette will be passed to your custom SetPixel
                                   // function instead of the actual color.

// Palette is an array of 256 DWORD's
// For LoadPalette and LoadGrp, lpFileName may be a file in an open mpq archive
// or a file not in an archive
typedef BOOL   (WINAPI* funcLoadPalette)(LPCSTR lpFileName, DWORD *dwPaletteBuffer);
typedef HANDLE (WINAPI* funcLoadGrp)(LPCSTR lpFileName);
typedef BOOL   (WINAPI* funcDestroyGrp)(HANDLE hGrp);
typedef BOOL   (WINAPI* funcDrawGrp)(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha);
typedef BOOL   (WINAPI* funcGetGrpInfo)(HANDLE hGrp, GRPHEADER *GrpInfo);
extern funcLoadPalette LoadPalette;
extern funcLoadGrp LoadGrp;
extern funcDestroyGrp DestroyGrp;
extern funcDrawGrp DrawGrp;
extern funcGetGrpInfo GetGrpInfo;

typedef HANDLE (WINAPI* funcCreateGrp)(signed short *lpImageData, WORD nFrames, WORD wMaxWidth, WORD wMaxHeight, BOOL bNoCompress, DWORD *nGrpSize);
extern funcCreateGrp CreateGrp;

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
typedef void (WINAPI* funcSetFunctionGetPixel)(GETPIXELPROC lpGetPixelProc);
typedef void (WINAPI* funcSetFunctionSetPixel)(SETPIXELPROC lpSetPixelProc);
extern funcSetFunctionGetPixel SetFunctionGetPixel; // Only used with ALPHA_BLEND
extern funcSetFunctionSetPixel SetFunctionSetPixel;

// Call this to make a different Storm.dll-compatible MPQ library be used (like SFMPQ).
typedef BOOL (WINAPI* funcSetMpqDll)(LPCSTR lpDllFileName);
extern funcSetMpqDll SetMpqDll;

// These no longer need to be called
typedef BOOL   (WINAPI* funcLoadGrpApi)();
typedef void   (WINAPI* funcFreeGrpApi)();
extern funcLoadGrpApi LoadGrpApi;
extern funcFreeGrpApi FreeGrpApi;

#ifdef __cplusplus
};  // extern "C" 
#endif

#endif

