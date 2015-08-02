#ifndef CMDANDCONQUER_MAINWINDOWH
#define CMDANDCONQUER_MAINWINDOWH

#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "../Resource/resource.h"


#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <stdio.h>
#include <strsafe.h>

typedef std::basic_stringstream<TCHAR> tstringstream;
typedef std::basic_string<TCHAR> ustring;
static WNDPROC oldEditProc;

VOID CALLBACK FileIOCompletionRoutine(
	__in DWORD dwErrorCode,
	__in DWORD dwNumberOfBytesTransfered,
	__in LPOVERLAPPED lpOverlapped);

void DisplayError(LPTSTR lpszFunction);



enum
{
	IDCE_SINGLELINE = 200, 
	IDCE_MULTILINE,
};
class CmdAndConquer_MainWindow
{
public:
	CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText, HMENU hMenu);
	~CmdAndConquer_MainWindow();
	void WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static void registerWindowClass(HINSTANCE hInstance);
	static ATOM getClassATOM(void);
	static LPCTSTR getClassName(void);
private:
	HWND hWnd_;
	HWND mainEditHwnd;
	HWND lineNoHwnd;
	
	static ATOM class_atom;
	static LPCTSTR class_name;
	LRESULT CALLBACK actualWndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK editWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	int onCreate(const HWND hWnd, CREATESTRUCT *cs);
	HWND createEditBox(const HWND hParent, const HINSTANCE hInst, DWORD dwStyle, const RECT& rc, const int id, const ustring& caption);
	void openFile();
};

#endif