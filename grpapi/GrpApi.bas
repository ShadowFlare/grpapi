Attribute VB_Name = "GrpApi"
Option Explicit

'  ShadowFlare GRP Library. (c) ShadowFlare Software 2002-2006

'  Any comments or suggestions are accepted at blakflare@hotmail.com (ShadowFlare)

Type GRPHEADER
    nFrames As Integer
    wMaxWidth As Integer
    wMaxHeight As Integer
End Type

Public Const HORIZONTAL_FLIP As Long = &H1& ' Flips the graphic horizontally
Public Const VERTICAL_FLIP   As Long = &H2& ' Flips the graphic vertically
Public Const SHADOW_COLOR    As Long = &H4& ' Causes the graphic to be drawn in one color
                                            ' Second byte of flags is the red component of
                                            ' the shadow's color, third byte is green,
                                            ' fourth byte is blue; like this:
                                            ' 'SHADOW_COLOR Or &HBBGGRR00'
Public Const ALPHA_BLEND     As Long = &H8& ' Blends the graphic with what it is being drawn over.
                                            ' The dwAlpha parameter will only be used when this
                                            ' flag is specified.  dwAlpha is an RGB value
                                            ' (&HBBGGRR).
                                            ' Note: Because of the extra calculations required,
                                            ' alpha blended graphics take longer to draw
Public Const USE_INDEX      As Long = &H10& ' Only valid when used with a custom SetPixel function.
                                            ' This flag cannot be used in combination with
                                            ' ALPHA_BLEND or SHADOW_COLOR
                                            ' When this flag is used, the index to a color in the
                                            ' palette will be passed to your custom SetPixel
                                            ' function instead of the actual color.

' Palette is an array of 256 Longs.  Pass the first element of the array to these functions,
' rather than the actual array
' For LoadPalette and LoadGrp, lpFileName may be a file in an open mpq archive
' or a file not in an archive
Declare Function LoadPalette Lib "Grpapi.dll" (ByVal lpFileName As String, dwPalette As Long) As Boolean
Declare Function LoadGrp Lib "Grpapi.dll" (ByVal lpFileName As String) As Long
Declare Function DestroyGrp Lib "Grpapi.dll" (ByVal hGrp As Long) As Boolean
Declare Function DrawGrp Lib "Grpapi.dll" (ByVal hGrp As Long, ByVal hdcDest As Long, ByVal nXDest As Long, ByVal nYDest As Long, ByVal nFrame As Integer, dwPalette As Long, ByVal dwFlags As Long, ByVal dwAlpha As Long) As Boolean
Declare Function GetGrpInfo Lib "Grpapi.dll" (ByVal hGrp As Long, GrpInfo As GRPHEADER) As Boolean
Declare Function GetGrpFrameInfo Lib "Grpapi.dll" (ByVal hGrp As Long, ByVal nFrame As Integer, ByRef nLeft As Long, ByRef nTop As Long, ByRef nWidth As Long, ByRef nHeight As Long) As Boolean

Declare Function GetDC Lib "User32.dll" (ByVal hWnd As Long) As Long
Declare Function ReleaseDC Lib "User32.dll" (ByVal hWnd As Long, ByVal hDC As Long) As Long

' An array of the raw image data to encode should be passed to lpImageData.  To do this,
' pass the first byte of the array to the function.  The size of the buffer containing
' the data should be nFrames * wMaxWidth * wMaxHeight * 2
' and the values should be arranged row by row of the frame, with the top row first.
' After all the rows of a frame have been put into the buffer, the rows of the next frame
' go after it.  For transparent pixels, they should be set to -1.  All other pixels should
' have the high order byte set to zero, meaning that they should not be negative and the
' values should not exceed 255 (&HFF).  The values used for the colors are indexes into the
' color palette.
' Pass True to bNoCompress if you need an uncompressed GRP.
' Pass a variable to nGrpSize to receive the size in bytes of the resulting encoded GRP.
' The return value of this function is actually a pointer to the GRP data.  This is what your
' program should save to a file.  The size of this buffer is the value received by nGrpSize.
' When your program is done with the returned buffer, it should call DestroyGrp on the
' buffer that was returned by this function to free up the memory from it.
' The pointer returned by this function can also be directly used by DrawGrp or GetGrpInfo.
Declare Function CreateGrp Lib "Grpapi.dll" (ByRef lpImageData As Integer, ByVal nFrames As Integer, ByVal wMaxWidth As Integer, ByVal wMaxHeight As Integer, ByVal bNoCompress As Boolean, ByRef nGrpSize As Long) As Long

' Use this function to make a copy the memory at the location returned by CreateGrp.
' Pass the address ByVal to Source.  Pass either a byte array or a string to Destination.
' The size of the array or string must be at least the size of the GRP data before
' attempting to copy it to the array or string.
' To pass a byte array, pass the first byte of the array ByRef.  To pass a string,
' pass the string ByVal.
Declare Sub CopyMemory Lib "Kernel32.dll" _
    Alias "RtlMoveMemory" ( _
    ByRef Destination As Any, _
    ByRef Source As Any, _
    ByVal Length As Long)

' Call these to have DrawGrp use custom functions for reading and drawing pixels
' so that you can have it read from and write to a buffer, for example.
' Requires Visual Basic 5 or higher
'
' The functions must be in this form:
'
' Function GetPixelProc (ByRef value As any_type, X As Long, Y As Long) As Long
' Sub SetPixelProc (ByRef value As any_type, X As Long, Y As Long, clrColor As Long)
'
' or
'
' Function GetPixelProc (ByVal value As any_type, X As Long, Y As Long) As Long
' Sub SetPixelProc (ByVal value As any_type, X As Long, Y As Long, clrColor As Long)
'
' Replace "any_type" with whatever type you want.  This parameter gets the data from
' DrawGrp's hdcDest parameter.  You can either pass a number to DrawGrp and use the
' "ByVal" versions of the above functions, or you can use AddressOf to get a reference
' to a variable and use it for the "ByRef" versions.
' GetPixelProc should return an RGB color value.
Declare Sub SetFunctionGetPixel Lib "Grpapi.dll" (lpGetPixelProc As Long)
Declare Sub SetFunctionSetPixel Lib "Grpapi.dll" (lpSetPixelProc As Long)

' Call this to make a different Storm.dll-compatible MPQ library be used (like SFMPQ).
Declare Function SetMpqDll Lib "Grpapi.dll" (ByVal lpDllFileName As String) As Boolean

' These no longer need to be called
Declare Function LoadGrpApi Lib "Grpapi.dll" () As Boolean
Declare Sub FreeGrpApi Lib "Grpapi.dll" ()
