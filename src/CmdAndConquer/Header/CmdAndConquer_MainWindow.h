#ifndef CMDANDCONQUER_MAINWINDOWH
#define CMDANDCONQUER_MAINWINDOWH

#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "../Resource/resource.h"
#include "../../TextView/Header/TextView.h"
#include "../../TextView/Header/TextViewInternal.h"


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

#define APP_TITLE   _T("CmdAndConquer")
#define WEBSITE_STR _T("www.drshockalot.com")

enum
{
	IDCE_SINGLELINE = 200, 
	IDCE_MULTILINE,
};
class CmdAndConquer_MainWindow
{
public:
	TCHAR		*szAppName;
	static HWND hWnd_;

	CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText);
	~CmdAndConquer_MainWindow();
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static void registerWindowClass(HINSTANCE hInstance);
	static ATOM getClassATOM(void);
	static LPCTSTR getClassName(void);
	static HWND getMainHWND();
	static HWND getTextHWND();
	static void setImageList();
	BOOL DoOpenFile(HWND hWnd, TCHAR * szFileName, TCHAR * szFileTitle);
	void HandleDropFiles(HWND hWnd, HDROP hDrop);
private:
	
	static HWND g_hwndTextView;
	//HWND hwndLineNo;
	
	TCHAR szFileName[MAX_PATH];
	TCHAR szFileTitle[MAX_PATH];

	static ATOM class_atom;
	static LPCTSTR class_name;
	LRESULT CALLBACK actualWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	int onCreate(const HWND hWnd, CREATESTRUCT *cs);
	//void initToolbar(HWND hWnd, CREATESTRUCT *cs);
	BOOL ShowOpenFileDlg(HWND hwnd, TCHAR * pstrFileName, TCHAR * pstrTitleName);
	void SetWindowFileName(HWND hwnd, TCHAR * szFileName);
	void ShowAboutDlg(HWND hwndParent);
	int PointsToLogical(int nPointSize);
	HFONT EasyCreateFont(int nPointSize, BOOL fBold, TCHAR * szFace);
};

#endif


