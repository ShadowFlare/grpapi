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

' Palette is an array of 256 Longs.  Pass the first element of the array to these functions,
' rather than the actual array
' For LoadPalette and LoadGrp, lpFileName may be a file in an open mpq archive
' or a file not in an archive
Declare Function LoadPalette Lib "Grpapi.dll" (ByVal lpFileName As String, dwPalette As Long) As Boolean
Declare Function LoadGrp Lib "Grpapi.dll" (ByVal lpFileName As String) As Long
Declare Function DestroyGrp Lib "Grpapi.dll" (ByVal hGrp As Long) As Boolean
Declare Function DrawGrp Lib "Grpapi.dll" (ByVal hGrp As Long, ByVal hdcDest As Long, ByVal nXDest As Long, ByVal nYDest As Long, ByVal nFrame As Integer, dwPalette As Long, ByVal dwFlags As Long, ByVal dwAlpha As Long) As Boolean
Declare Function GetGrpInfo Lib "Grpapi.dll" (ByVal hGrp As Long, GrpInfo As GRPHEADER) As Boolean

Declare Function GetDC Lib "User32.dll" (ByVal hWnd As Long) As Long
Declare Function ReleaseDC Lib "User32.dll" (ByVal hWnd As Long, ByVal hDC As Long) As Long

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

