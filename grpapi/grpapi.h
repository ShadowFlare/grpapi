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
BOOL   GRPAPI WINAPI LoadPalette(LPCSTR lpFileName, DWORD *dwPaletteBuffer);
HANDLE GRPAPI WINAPI LoadGrp(LPCSTR lpFileName);
BOOL   GRPAPI WINAPI DestroyGrp(HANDLE hGrp);
BOOL   GRPAPI WINAPI DrawGrp(HANDLE hGrp, HDC hdcDest, int nXDest, int nYDest, WORD nFrame, DWORD *dwPalette, DWORD dwFlags, DWORD dwAlpha);
BOOL   GRPAPI WINAPI GetGrpInfo(HANDLE hGrp, GRPHEADER *GrpInfo);
BOOL   GRPAPI WINAPI GetGrpFrameInfo(HANDLE hGrp, WORD nFrame, DWORD *nLeft, DWORD *nTop, DWORD *nWidth, DWORD *nHeight);

// A pointer to the raw image data to encode should be passed to lpImageData.  The size of
// the buffer containing the data should be nFrames * wMaxWidth * wMaxHeight * sizeof(short)
// and the values should be arranged row by row of the frame, with the top row first.
// After all the rows of a frame have been put into the buffer, the rows of the next frame
// go after it.  For transparent pixels, they should be set to -1.  All other pixels should
// have the high order byte set to zero, meaning that they should not be negative and the
// values should not exceed 255 (0xFF).  The values used for the colors are indexes into the
// color palette.
// Pass TRUE to bNoCompress if you need an uncompressed GRP.
// Pass a pointer to a DWORD value to nGrpSize to receive the size in bytes of the resulting encoded GRP.
// The return value of this function is actually a pointer to the GRP data.  This is what your
// program should save to a file.  The size of this buffer is the value received by nGrpSize.
// When your program is done with the returned buffer, it should call DestroyGrp on the
// buffer that was returned by this function to free up the memory from it.
// The pointer returned by this function can also be directly used by DrawGrp or GetGrpInfo.
HANDLE GRPAPI WINAPI CreateGrp(signed short *lpImageData, WORD nFrames, WORD wMaxWidth, WORD wMaxHeight, BOOL bNoCompress, DWORD *nGrpSize);

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
void GRPAPI WINAPI SetFunctionGetPixel(GETPIXELPROC lpGetPixelProc); // Only used with ALPHA_BLEND
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
