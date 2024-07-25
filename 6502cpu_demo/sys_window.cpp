/*
* BASIC Windows Win32 Code. 
* 
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non - commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain.We make this dedication for the benefit
of the public at largeand to the detriment of our heirsand
successors.We intend this dedication to be an overt act of
relinquishment in perpetuity of all presentand future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to < http://unlicense.org/>
*/


#include <windows.h>
#include "sys_gl.h"
#include "sys_log.h"
#include "sys_window.h"
#include "sys_rawinput.h"
#include <stdio.h>
#include <stdlib.h>
#include "asteroid.h"

#pragma warning (disable : 4996)

//Globals

HWND hWnd;
RECT MyWindow;

//Default Window Size
int WinWidth = 1024;
int WinHeight = 768;


// Function Declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//========================================================================
// Popup a Windows Error Message, Allegro Style
//========================================================================
void allegro_message(const char* title, const char* message)
{
	MessageBoxA(NULL, message, title, MB_ICONEXCLAMATION | MB_OK);
}


int KeyCheck(int keynum)
{
	int i;
	static int hasrun = 0;
	static int keys[256];
	//Init
	if (hasrun == 0) { for (i = 0; i < 256; i++) { keys[i] = 0; }	hasrun = 1; }

	if (!keys[keynum] && key[keynum]) //Return True if not in queue
	{
		keys[keynum] = 1;	return 1;
	}
	else if (keys[keynum] && !key[keynum]) //Return False if in queue
		keys[keynum] = 0;
	return 0;
}

HWND win_get_window()
{
	return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Simple, generic window init //
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int iCmdShow)
{
	WNDCLASS wc;

	MSG msg;
	bool Quit = FALSE;
	DWORD       dwExStyle;                      // Window Extended Style
	DWORD       dwStyle;                        // Window Style

	//  window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"CPU_6502";
	///RegisterClass(&wc);
	if (0 == RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Can't Register the Window Class!"), L"CPU_6502", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	dwExStyle = WS_EX_APPWINDOW;									   // Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW | WS_THICKFRAME;                     // Windows Style
	RECT WindowRect;                                                   // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                                         // Set Left Value To 0
	WindowRect.right = (long)WinWidth;                                 // Set Right Value To Requested Width
	WindowRect.top = (long)0;                                          // Set Top Value To 0
	WindowRect.bottom = (long)WinHeight;                               // Set Bottom Value To Requested Height
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);        // Adjust Window To True Requested Size

	// Create The Window
	//if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
	hWnd = CreateWindowEx(dwExStyle,
		L"CPU_6502",						// Class Name
		L"6502 CPU Demo Usage Code",		// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		CW_USEDEFAULT, 0,   				// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);						        // Dont Pass Anything To WM_CREATE

	if (NULL == hWnd)
	{
		MessageBox(NULL, TEXT("Unable to Create the Main Window!"), L"ERROR", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	//********** Program Initializations *************
	//Enable Logging
	LogOpen("demo_log.txt");
	// enable OpenGL for the window
	CreateGLContext();
	//Basic Window Init
	WRLOG("Starting Program");
	ShowWindow(hWnd, SW_SHOW);                                         // Show The Window
	SetForegroundWindow(hWnd);									    // Slightly Higher Priority
	SetFocus(hWnd);													// Sets Keyboard Focus To The Window
	ReSizeGLScene(WinWidth, WinHeight);										    // Set Up Our Perspective GL Screen
	//Get the Supported OpenGl Version
	CheckGLVersionSupport();
	//Fill in the Window Rect;
	GetClientRect(hWnd, &MyWindow);
	//Set the OpenGl View
	ViewOrtho(WinWidth, WinHeight);
	//Enable vSync
	SetVSync(1);

	//Enable RawInput
	HRESULT i = RawInput_Initialize(hWnd);
	if (i != S_OK)
	{
		MessageBox(NULL, TEXT("Unable to attach Rawinput devices to the Main Window!"), L"ERROR", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
		

	asteroid_init();
	// ********** Program Main Loop **********

	while (!Quit) {
		// check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// handle or dispatch messages
			if (msg.message == WM_QUIT) {
				Quit = TRUE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			// ********** OpenGL drawing code, very simple, just to show off the font. **********

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			asteroid_run();

			GlSwap();
		}
	}

	// ********** Cleanup and exit gracefully. **********
	// shutdown OpenGL

	asteroid_end();
	DeleteGLContext();
	LogClose();
	// destroy the window
	DestroyWindow(hWnd);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return S_OK;

	case WM_SIZE:
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	
	case WM_INPUT: {return RawInput_ProcessInput(hWnd, wParam, lParam); return 0; }

	case WM_DESTROY:
		return 0;

	case WM_SYSCOMMAND:
	{
		switch (wParam & 0xfff0)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
		{
			return 0;
		}
		/*
		case SC_CLOSE:
		{
			//I can add a close hook here to trap close button
			quit = 1;
			PostQuitMessage(0);
			break;
		}
		*/
		// User trying to access application menu using ALT?
		case SC_KEYMENU:
			return 0;
		}
		DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}