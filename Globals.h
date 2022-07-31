#pragma once
#include "resource.h"
#include "framework.h"

#define MAX_LOADSTRING 100

HWND hWnd;                                      // handle of window
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HDC hDC;                                        // paint space
HGLRC hRC;                                      // resources for opengl
POINT wnSize;                                   // size of window
POINT posCursor;                                // position of cursor

VOID Render();
VOID DrawInterface();
