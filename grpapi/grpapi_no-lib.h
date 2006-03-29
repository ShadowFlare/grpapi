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
#define ALPHA_BLEND     0x00000008 // Blends the graphic with what it is being drawn over.
                                   // The dwAlpha parameter will only be used when this
                                   // flag is specified.  dwAlpha is an RGB value
                                   // (0xBBGGRR).
                                   // Note: Because of the extra calculations required,
                                   // alpha blended graphics take longer to draw

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

// These no longer need to be called
typedef BOOL   (WINAPI* funcLoadGrpApi)();
typedef void   (WINAPI* funcFreeGrpApi)();
extern funcLoadGrpApi LoadGrpApi;
extern funcFreeGrpApi FreeGrpApi;

#ifdef __cplusplus
};  // extern "C" 
#endif

#endif

