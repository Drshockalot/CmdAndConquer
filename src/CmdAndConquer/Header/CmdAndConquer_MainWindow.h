#ifndef CMDANDCONQUER_MAINWINDOWH
#define CMDANDCONQUER_MAINWINDOWH

#include <Windows.h>
#include <tchar.h>
#include "../Header/resource.h"

#include <sstream>
#include <stdexcept>
#include <cassert>

typedef std::basic_stringstream<TCHAR> tstringstream;
typedef std::basic_string<TCHAR> ustring;
static WNDPROC oldEditProc;
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
	
	static ATOM class_atom;
	static LPCTSTR class_name;
	LRESULT CALLBACK actualWndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK editWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	int onCreate(const HWND hWnd, CREATESTRUCT *cs);
	HWND createEditBox(const HWND hParent, const HINSTANCE hInst, DWORD dwStyle, const RECT& rc, const int id, const ustring& caption);
};

#endif