#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <shellapi.h>
#include "../../CmdAndConquer/Resource/resource.h"
#include "../../CmdAndConquer/Header/CmdAndConquer_Globals.h"
#include "../../TextView/Header/TextView.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/foreach.hpp>
#include <vector>

#pragma comment(lib, "comctl32.lib")

#define CONTEXT_CMD_LOC _T("*\\shell\\Open with Neatpad\\command")
#define CONTEXT_APP_LOC _T("*\\shell\\Open with Neatpad")

#define IMAGEFILE_XOPT _T("Software\\Microsoft\\Windows NT\\CurrentVersion")\
					   _T("\\Image File Execution Options\\CmdAndConquer.exe")
#include <set>
#include <list>

BOOL CALLBACK FontOptionsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MiscOptionsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DisplayOptionsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG  g_nFontSize;
BOOL  g_fFontBold;
TCHAR g_szFontName[LF_FACESIZE];
LONG  g_nFontSmoothing;

LONG  g_nPaddingAbove;
LONG  g_nPaddingBelow;
LONG  g_fPaddingFlags;
BOOL  g_fLineNumbers;
BOOL  g_fLongLines;
BOOL  g_fSelMargin;
BOOL  g_fSaveOnExit;
int	  g_nLongLineLimit;
BOOL  g_nHLCurLine;
BOOL  g_fShowStatusbar;
BOOL  g_fAddToExplorer = 0;
BOOL  g_fReplaceNotepad = 0;

HFONT g_hFont;

namespace pt = boost::property_tree;
std::string settingsFileName;
std::vector<std::string> settings;

COLORREF g_rgbColourList[TXC_MAX_COLOURS];
COLORREF g_rgbCustColours[16];
extern COLORREF g_rgbAutoColourList[];

// Get a binary buffer from the registry
BOOL GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength)
{
	ZeroMemory(pBuffer, nLength);
	return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE *)pBuffer, (unsigned long*)&nLength);
}

// Get an integer value from the registry
BOOL GetSettingInt(HKEY hkey, TCHAR szKeyName[], LONG *pnReturnVal, LONG nDefault)
{
	ULONG len = sizeof(nDefault);

	*pnReturnVal = nDefault;

	return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE *)pnReturnVal, &len);
}

// Get a string buffer from the registry
BOOL GetSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR pszReturnStr[], DWORD nLength, TCHAR szDefault[])
{
	ULONG len = nLength * sizeof(TCHAR);

	lstrcpyn(pszReturnStr, szDefault, nLength);

	return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE *)pszReturnStr, &len);
}

// Write a binary value from the registry
BOOL WriteSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pData, ULONG nLength)
{
	return !RegSetValueEx(hkey, szKeyName, 0, REG_BINARY, (BYTE *)pData, nLength);
}

// Write an integer value from the registry
BOOL WriteSettingInt(HKEY hkey, TCHAR szKeyName[], LONG nValue)
{
	return !RegSetValueEx(hkey, szKeyName, 0, REG_DWORD, (BYTE *)&nValue, sizeof(nValue));
}

// Get a string buffer from the registry
BOOL WriteSettingStr(HKEY hkey, TCHAR szKeyName[], TCHAR szString[])
{
	return !RegSetValueEx(hkey, szKeyName, 0, REG_SZ, (BYTE *)szString, (lstrlen(szString) + 1) * sizeof(TCHAR));
}

//
//	Add or remove Neatpad from the Explorer context-menu
//
BOOL SetExplorerContextMenu(BOOL fAddToMenu)
{
	HRESULT hr;

	if (fAddToMenu)
	{
		TCHAR szAppPath[MAX_PATH];
		TCHAR szDefaultStr[MAX_PATH];

		GetModuleFileName(0, szAppPath, MAX_PATH);

		wsprintf(szDefaultStr, _T("\"%s\" \"%%1\""), szAppPath);

		hr = RegSetValue(HKEY_CLASSES_ROOT, CONTEXT_CMD_LOC, REG_SZ, szDefaultStr, lstrlen(szDefaultStr) * sizeof(TCHAR));
	}
	else
	{
		hr = RegDeleteKey(HKEY_CLASSES_ROOT, CONTEXT_CMD_LOC);

		if (hr == ERROR_SUCCESS)
			hr = RegDeleteKey(HKEY_CLASSES_ROOT, CONTEXT_APP_LOC);
	}

	return (hr == ERROR_SUCCESS) ? TRUE : FALSE;
}

//
//	Replace/Restore Notepad (with Neatpad) as the default text editor, by
//  manipulating the Image-File-Execution-Options debugger setting for NOTEPAD.EXE
//
BOOL SetImageFileExecutionOptions(BOOL fReplaceWithCurrentApp)
{
	HKEY	hKey;
	HRESULT hr;
	TCHAR	szPath[MAX_PATH];

	// create an 'ImageFileExecutionOptions' entry for the standard Notepad app
	hr = RegCreateKeyEx(HKEY_LOCAL_MACHINE, IMAGEFILE_XOPT, 0, 0, 0, KEY_WRITE, 0, &hKey, 0);

	if (hr != ERROR_SUCCESS)
		return FALSE;

	// get path of current exe
	GetModuleFileName(0, szPath + 1, MAX_PATH);

	// enclose it in double-quotes
	szPath[0] = '\"';
	lstrcat(szPath, _T("\""));
	lstrcat(szPath, _T(" -ifeo"));

	// set the 'debugger' key so that whenever notepad.exe is executed, neatpad runs instead
	if (fReplaceWithCurrentApp)
		WriteSettingStr(hKey, _T("Debugger"), szPath);
	else
		RegDeleteValue(hKey, _T("Debugger"));

	RegCloseKey(hKey);
	return TRUE;
}

void LoadRegSettings()
{
	HKEY hKey, hColKey;

	// open registry location for reading
	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_READ, 0, &hKey, 0);

	GetSettingInt(hKey, _T("FontSize"), &g_nFontSize, 10);
	GetSettingInt(hKey, _T("FontBold"), (long*)&g_fFontBold, FALSE);
	GetSettingStr(hKey, _T("FontName"), g_szFontName, LF_FACESIZE, _T("Courier New"));
	GetSettingInt(hKey, _T("FontSmooth"), &g_nFontSmoothing, DEFAULT_QUALITY);

	GetSettingInt(hKey, _T("PaddingAbove"), &g_nPaddingAbove, 0);
	GetSettingInt(hKey, _T("PaddingBelow"), &g_nPaddingBelow, 1);
	GetSettingInt(hKey, _T("PaddingFlags"), &g_fPaddingFlags, COURIERNEW | LUCIDACONS);

	GetSettingInt(hKey, _T("SelMargin"), (long*)&g_fSelMargin, TRUE);
	GetSettingInt(hKey, _T("LineNumbers"), (long*)&g_fLineNumbers, FALSE);
	GetSettingInt(hKey, _T("LongLines"), (long*)&g_fLongLines, TRUE);
	GetSettingInt(hKey, _T("LongLineLimit"), (long*)&g_nLongLineLimit, 80);
	GetSettingInt(hKey, _T("SaveOnExit"), (long*)&g_fSaveOnExit, TRUE);
	GetSettingInt(hKey, _T("HLCurLine"), (long*)&g_nHLCurLine, FALSE);

	GetSettingInt(hKey, _T("AddExplorer"), (long*)&g_fAddToExplorer, FALSE);
	GetSettingInt(hKey, _T("ReplaceNotepad"), (long*)&g_fReplaceNotepad, FALSE);
	GetSettingInt(hKey, _T("ShowStatusbar"), (long*)&g_fShowStatusbar, FALSE);

	// read the display colours
	RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_READ, 0, &hColKey, 0);

	GetSettingInt(hColKey, _T("Foreground"), (long*)&g_rgbColourList[TXC_FOREGROUND], g_rgbAutoColourList[TXC_FOREGROUND]);
	GetSettingInt(hColKey, _T("Background"), (long*)&g_rgbColourList[TXC_BACKGROUND], g_rgbAutoColourList[TXC_BACKGROUND]);
	GetSettingInt(hColKey, _T("SelFG"), (long*)&g_rgbColourList[TXC_HIGHLIGHTTEXT], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT]);
	GetSettingInt(hColKey, _T("SelBG"), (long*)&g_rgbColourList[TXC_HIGHLIGHT], g_rgbAutoColourList[TXC_HIGHLIGHT]);
	GetSettingInt(hColKey, _T("SelFG2"), (long*)&g_rgbColourList[TXC_HIGHLIGHTTEXT2], g_rgbAutoColourList[TXC_HIGHLIGHTTEXT2]);
	GetSettingInt(hColKey, _T("SelBG2"), (long*)&g_rgbColourList[TXC_HIGHLIGHT2], g_rgbAutoColourList[TXC_HIGHLIGHT2]);
	GetSettingInt(hColKey, _T("Margin1"), (long*)&g_rgbColourList[TXC_SELMARGIN1], g_rgbAutoColourList[TXC_SELMARGIN1]);
	GetSettingInt(hColKey, _T("Margin2"), (long*)&g_rgbColourList[TXC_SELMARGIN2], g_rgbAutoColourList[TXC_SELMARGIN2]);
	GetSettingInt(hColKey, _T("LinenoText"), (long*)&g_rgbColourList[TXC_LINENUMBERTEXT], g_rgbAutoColourList[TXC_LINENUMBERTEXT]);
	GetSettingInt(hColKey, _T("Lineno"), (long*)&g_rgbColourList[TXC_LINENUMBER], g_rgbAutoColourList[TXC_LINENUMBER]);
	GetSettingInt(hColKey, _T("LongLineText"), (long*)&g_rgbColourList[TXC_LONGLINETEXT], g_rgbAutoColourList[TXC_LONGLINETEXT]);
	GetSettingInt(hColKey, _T("LongLine"), (long*)&g_rgbColourList[TXC_LONGLINE], g_rgbAutoColourList[TXC_LONGLINE]);
	GetSettingInt(hColKey, _T("CurlineText"), (long*)&g_rgbColourList[TXC_CURRENTLINETEXT], g_rgbAutoColourList[TXC_CURRENTLINETEXT]);
	GetSettingInt(hColKey, _T("Curline"), (long*)&g_rgbColourList[TXC_CURRENTLINE], g_rgbAutoColourList[TXC_CURRENTLINE]);

	GetSettingBin(hColKey, _T("Custom"), g_rgbCustColours, sizeof(g_rgbCustColours));

	RegCloseKey(hColKey);
	RegCloseKey(hKey);
}

void LoadXMLSettings()
{
	pt::ptree tree;
	pt::read_xml(settingsFileName, tree);
	pt::ptree ptreeSet = tree.get_child("settings");
	BOOST_FOREACH(auto const &v, tree.get_child("settings"))
	{
		settings.push_back(v.second.data());
	}


}

void LoadRegSysSettings()
{
	HKEY hKey;

	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_READ, 0, &hKey, 0);
	GetSettingInt(hKey, _T("AddExplorer"), (long*)&g_fAddToExplorer, FALSE);
	GetSettingInt(hKey, _T("ReplaceNotepad"), (long*)&g_fReplaceNotepad, FALSE);
	RegCloseKey(hKey);
}

void SaveRegSysSettings()
{
	HKEY hKey;

	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_WRITE, 0, &hKey, 0);
	WriteSettingInt(hKey, _T("AddExplorer"), g_fAddToExplorer);
	WriteSettingInt(hKey, _T("ReplaceNotepad"), g_fReplaceNotepad);
	RegCloseKey(hKey);
}

void SaveRegSettings()
{
	HKEY hKey, hColKey;

	// open registry location for writing
	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_WRITE, 0, &hKey, 0);

	WriteSettingInt(hKey, _T("FontSize"), g_nFontSize);
	WriteSettingInt(hKey, _T("FontBold"), g_fFontBold);
	WriteSettingStr(hKey, _T("FontName"), g_szFontName);
	WriteSettingInt(hKey, _T("FontSmooth"), g_nFontSmoothing);

	WriteSettingInt(hKey, _T("PaddingAbove"), g_nPaddingAbove);
	WriteSettingInt(hKey, _T("PaddingBelow"), g_nPaddingBelow);
	WriteSettingInt(hKey, _T("PaddingFlags"), g_fPaddingFlags);

	WriteSettingInt(hKey, _T("SelMargin"), g_fSelMargin);
	WriteSettingInt(hKey, _T("LineNumbers"), g_fLineNumbers);
	WriteSettingInt(hKey, _T("LongLines"), g_fLongLines);
	WriteSettingInt(hKey, _T("SaveOnExit"), g_fSaveOnExit);
	WriteSettingInt(hKey, _T("LongLineLimit"), g_nLongLineLimit);
	WriteSettingInt(hKey, _T("HLCurLine"), g_nHLCurLine);

	WriteSettingInt(hKey, _T("AddExplorer"), g_fAddToExplorer);
	WriteSettingInt(hKey, _T("ReplaceNotepad"), g_fReplaceNotepad);
	WriteSettingInt(hKey, _T("ShowStatusbar"), g_fShowStatusbar);

	// write the display colours
	RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_WRITE, 0, &hColKey, 0);

	WriteSettingInt(hColKey, _T("Foreground"), g_rgbColourList[TXC_FOREGROUND]);
	WriteSettingInt(hColKey, _T("Background"), g_rgbColourList[TXC_BACKGROUND]);
	WriteSettingInt(hColKey, _T("SelFG"), g_rgbColourList[TXC_HIGHLIGHTTEXT]);
	WriteSettingInt(hColKey, _T("SelBG"), g_rgbColourList[TXC_HIGHLIGHT]);
	WriteSettingInt(hColKey, _T("SelFG2"), g_rgbColourList[TXC_HIGHLIGHTTEXT2]);
	WriteSettingInt(hColKey, _T("SelBG2"), g_rgbColourList[TXC_HIGHLIGHT2]);

	WriteSettingInt(hColKey, _T("Margin1"), g_rgbColourList[TXC_SELMARGIN1]);
	WriteSettingInt(hColKey, _T("Margin2"), g_rgbColourList[TXC_SELMARGIN2]);
	WriteSettingInt(hColKey, _T("LinenoText"), g_rgbColourList[TXC_LINENUMBERTEXT]);
	WriteSettingInt(hColKey, _T("Lineno"), g_rgbColourList[TXC_LINENUMBER]);
	WriteSettingInt(hColKey, _T("LongLineText"), g_rgbColourList[TXC_LONGLINETEXT]);
	WriteSettingInt(hColKey, _T("LongLine"), g_rgbColourList[TXC_LONGLINE]);

	WriteSettingInt(hColKey, _T("CurlineText"), g_rgbColourList[TXC_CURRENTLINETEXT]);
	WriteSettingInt(hColKey, _T("Curline"), g_rgbColourList[TXC_CURRENTLINE]);


	WriteSettingBin(hColKey, _T("Custom"), g_rgbCustColours, sizeof(g_rgbCustColours));

	RegCloseKey(hColKey);
	RegCloseKey(hKey);
}

void ApplyRegSettings()
{
	int i;

	if (g_hFont)
		DeleteObject(g_hFont);

	g_hFont = EasyCreateFont(g_nFontSize, g_fFontBold, g_nFontSmoothing, g_szFontName);

	TextView_SetLineSpacing(g_hwndTextView, g_nPaddingAbove, g_nPaddingBelow);

	TextView_SetStyleBool(g_hwndTextView, TXS_SELMARGIN, g_fSelMargin);
	TextView_SetStyleBool(g_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
	TextView_SetStyleBool(g_hwndTextView, TXS_LONGLINES, g_fLongLines);

	TextView_SetStyleBool(g_hwndTextView, TXS_HIGHLIGHTCURLINE, g_nHLCurLine);

	TextView_SetCaretWidth(g_hwndTextView, 2);
	TextView_SetLongLine(g_hwndTextView, g_nLongLineLimit);

	SendMessage(g_hwndTextView, WM_SETFONT, (WPARAM)g_hFont, 0);

	for (i = 0; i < TXC_MAX_COLOURS; i++)
	{
		TextView_SetColor(g_hwndTextView, i, g_rgbColourList[i]);
	}

	//
	//	System-wide options require Administrator access. On Vista we
	//	need to elevate using the UAC prompt. Only do this if the settings have actually
	//	changed
	//
	//SetExplorerContextMenu(g_fAddToExplorerContextMenu);
	//SetImageFileExecutionOptions(g_fReplaceNotepad);
}

void ShowOptions(HWND hwndParent)
{
	PROPSHEETHEADER psh = { sizeof(psh) };
	PROPSHEETPAGE   psp[3] = { { sizeof(psp[0]) },
	{ sizeof(psp[1]) },
	{ sizeof(psp[2]) },
	};

	CoInitialize(0);

	// configure property sheet
	psh.dwFlags = PSH_PROPSHEETPAGE;
	psh.hwndParent = hwndParent;
	psh.nPages = sizeof(psp) / sizeof(psp[0]);
	psh.ppsp = psp;
	psh.pszCaption = _T("Options");

	// configure property sheet page(1)
	psp[0].dwFlags = PSP_USETITLE;
	psp[0].hInstance = g_hResourceModule;//GetModuleHandle(0);
	psp[0].pfnDlgProc = FontOptionsDlgProc;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_FONT);
	psp[0].pszTitle = _T("Font");

	// configure property sheet page(2)
	psp[1].dwFlags = PSP_USETITLE;
	psp[1].hInstance = g_hResourceModule;//GetModuleHandle(0);
	psp[1].pfnDlgProc = DisplayOptionsDlgProc;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DISPLAY);
	psp[1].pszTitle = _T("Display");

	// configure property sheet page(3)
	psp[2].dwFlags = PSP_USETITLE;
	psp[2].hInstance = g_hResourceModule;//GetModuleHandle(0);
	psp[2].pfnDlgProc = MiscOptionsDlgProc;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_OPTIONS);
	psp[2].pszTitle = _T("Settings");

	if (PropertySheet(&psh))
	{
		ApplyRegSettings();
	}

	CoUninitialize();
}

int PointsToLogical(int nPointSize)
{
	HDC hdc = GetDC(0);
	int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(0, hdc);

	return nLogSize;
}

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR *szFace)
{
	return CreateFont(PointsToLogical(nPointSize),
		0, 0, 0,
		fBold ? FW_BOLD : 0,
		0, 0, 0, 0, 0, 0,
		dwQuality,
		0,
		szFace);
}

//
//	Dialogbox procedure for the FONT pane
//
BOOL CALLBACK DisplayOptionsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PSHNOTIFY *pshn;

	switch (msg)
	{
	case WM_INITDIALOG:

		SendDlgItemMessage(hwnd, IDC_LONGLINEMODE, CB_ADDSTRING, 0, (LPARAM)_T("No Highlighting"));
		SendDlgItemMessage(hwnd, IDC_LONGLINEMODE, CB_ADDSTRING, 0, (LPARAM)_T("Highlight long lines"));

		SendDlgItemMessage(hwnd, IDC_LONGLINEMODE, CB_SETCURSEL, g_fLongLines, 0);

		SetDlgItemInt(hwnd, IDC_LONGLINELIM, g_nLongLineLimit, FALSE);
		EnableDlgItem(hwnd, IDC_LONGLINELIM, g_fLongLines);

		CheckDlgButton(hwnd, IDC_LINENOS, g_fLineNumbers);
		CheckDlgButton(hwnd, IDC_SELMARGIN, g_fSelMargin);

		CheckDlgButton(hwnd, IDC_HIGHLIGHTCURLINE, g_nHLCurLine);

		return TRUE;

	case WM_CLOSE:
		return TRUE;

	case WM_NOTIFY:

		pshn = (PSHNOTIFY *)lParam;

		if (pshn->hdr.code == PSN_APPLY)
		{
			g_fLineNumbers = IsDlgButtonChecked(hwnd, IDC_LINENOS);
			g_fSelMargin = IsDlgButtonChecked(hwnd, IDC_SELMARGIN);
			g_fLongLines = SendDlgItemMessage(hwnd, IDC_LONGLINEMODE, CB_GETCURSEL, 0, 0);
			g_nLongLineLimit = GetDlgItemInt(hwnd, IDC_LONGLINELIM, 0, FALSE);
			g_nHLCurLine = IsDlgButtonChecked(hwnd, IDC_HIGHLIGHTCURLINE);

			return TRUE;
		}

		return FALSE;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDC_LONGLINEMODE:

			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				int idx = SendDlgItemMessage(hwnd, IDC_LONGLINEMODE, CB_GETCURSEL, 0, 0);
				EnableDlgItem(hwnd, IDC_LONGLINELIM, idx);
			}

			return TRUE;

		case IDCANCEL:
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

BOOL ElevateToAdmin(HWND hwnd, BOOL fChecked1, BOOL fChecked2)//TCHAR *szParams)
{
	//// http://codefromthe70s.org/vistatutorial.asp
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	TCHAR szFile[MAX_PATH];
	TCHAR szParams[32];

	BOOL  success = FALSE;

	wsprintf(szParams, _T("-uac %d %d"), fChecked1, fChecked2);

	GetModuleFileName(0, szFile, MAX_PATH);

	sei.hwnd = hwnd;
	sei.lpVerb = L"runas";
	sei.lpFile = szFile;
	sei.lpParameters = szParams;//L"oof";
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.nShow = SW_SHOWNORMAL;

	if (ShellExecuteEx(&sei))
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
		GetExitCodeProcess(sei.hProcess, (unsigned long*)&success);
		CloseHandle(sei.hProcess);
		success = !success;
	}

	return success;
}

void ApplyAdminSettings(HWND hwnd)
{
	BOOL fChecked1;
	BOOL fChecked2;

	fChecked1 = IsDlgButtonChecked(hwnd, IDC_ADDCONTEXT);
	fChecked2 = IsDlgButtonChecked(hwnd, IDC_REPLACENOTEPAD);

	// do we need to elevate?
	if (g_fAddToExplorer != fChecked1 || g_fReplaceNotepad != fChecked2)
	{
		// spawn ourselves to set the HKLM registry keys
		ElevateToAdmin(hwnd, fChecked1, fChecked2);

		// the spawned process will do a SaveRegSysSettings if
		// it was successful - so do a 'Load' to refresh our own state
		LoadRegSysSettings();
	}
}

//
//	Dialogbox procedure for the FONT pane
//
BOOL CALLBACK MiscOptionsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PSHNOTIFY *pshn;
	HICON hShield;

	switch (msg)
	{

	case WM_INITDIALOG:

		//oof(hwnd);

		// load the 'vista shield icon'
		hShield = (HICON__*)LoadImage(0, MAKEINTRESOURCE(106), IMAGE_ICON, 32, 32, LR_CREATEDIBSECTION);//IDI_SHIELD));

																							  // display the next-best-thing if not running on Vista
		if (hShield == 0)
		{
			hShield = LoadIcon(0, MAKEINTRESOURCE(IDI_INFORMATION));
		}

		SendDlgItemMessage(hwnd, IDC_SHIELD, STM_SETICON, (WPARAM)hShield, 0);

		CheckDlgButton(hwnd, IDC_ADDCONTEXT, g_fAddToExplorer);
		CheckDlgButton(hwnd, IDC_REPLACENOTEPAD, g_fReplaceNotepad);

		// disable 'replace notepad' option for Win9x
		EnableDlgItem(hwnd, IDC_REPLACENOTEPAD, g_fReplaceNotepad);
		//EnableDlgItem(hwnd, IDC_REPLACENOTEPAD, (GetVersion() & 0x80000000) ? FALSE : TRUE);
		return TRUE;

	case WM_CLOSE:
		return TRUE;

	case WM_NOTIFY:

		pshn = (PSHNOTIFY *)lParam;

		if (pshn->hdr.code == PSN_APPLY)
		{
			ApplyAdminSettings(hwnd);
			return TRUE;
		}

		return FALSE;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}