Attribute VB_Name = "SDL2"
Option Explicit

Public Declare Function SDL_Init Lib "SDL2.dll" (ByVal flags As Long) As Long
Public Declare Sub SDL_Quit Lib "SDL2.dll" ()
Public Declare Function SDL_GetError Lib "SDL2.dll" () As String

