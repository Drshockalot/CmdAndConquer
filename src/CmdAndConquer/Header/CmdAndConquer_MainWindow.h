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

class CmdAndConquer_MainWindow
{
public:
	//	Public Data Members
	HWND hWnd_;

	//	Construcors
	CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText);
	~CmdAndConquer_MainWindow();
	
	// Public Data Functions
	static ATOM getClassATOM(void);		//	Necessary?
	static LPCTSTR getClassName(void);	//	Necessary?
	HWND getMainHWND();

private:
	
	//	Main edit control
	HWND CC_hwndTextView;

	//	Batch results text view
	HWND CC_hwndBatchRunResults;
	
	//	Class Registration
	void registerWindowClass(HINSTANCE hInstance);

	//	Private Data Members
	TCHAR *szAppName;
	TCHAR szFileName[MAX_PATH];
	TCHAR szFileTitle[MAX_PATH];
	static ATOM class_atom;
	static LPCTSTR class_name;

	//	Class Window Procedures
	/*
		InitialWndProc = Initialises Window Class
		StatiWndProc = Asserts the correct Window/User data
		WndProc = True Window Procedure
	*/
	LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int onCreate(const HWND hWnd, CREATESTRUCT *cs);
	void setImageList();
	BOOL ShowOpenFileDlg(HWND hwnd, TCHAR * pstrFileName, TCHAR * pstrTitleName);
	
	void ShowAboutDlg(HWND hwndParent);	
	UINT TextViewNotifyHandler(HWND hwnd, NMHDR *nmhdr);
	UINT NotifyHandler(HWND hwnd, NMHDR *nmhdr);
	UINT CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom);
	//	Toolbar Initialisation
	//void initToolbar(HWND hWnd, CREATESTRUCT *cs);
};

#endif


