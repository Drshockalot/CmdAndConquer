//
//
//
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <shellapi.h>
#include "../../CmdAndConquer/Resource/resource.h"
#include "../../CmdAndConquer/Header/CmdAndConquer_Globals.h"
#include "../../TextView/Header/TextView.h"

#pragma comment(lib, "comctl32.lib")

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

HFONT g_hFont;

COLORREF g_rgbColourList[TXC_MAX_COLOURS];
COLORREF g_rgbCustColours[16];
extern COLORREF g_rgbAutoColourList[];

// Get a binary buffer from the registry
BOOL GetSettingBin(HKEY hkey, TCHAR szKeyName[], PVOID pBuffer, LONG nLength)
{
	ZeroMemory(pBuffer, nLength);
	return !RegQueryValueEx(hkey, szKeyName, 0, 0, (BYTE *)pBuffer, &nLength);
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

void LoadRegSettings()
{
	HKEY hKey, hColKey;

	// open registry location for reading
	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_READ, 0, &hKey, 0);

	GetSettingInt(hKey, _T("FontSize"),		&g_nFontSize, 10);
	GetSettingInt(hKey, _T("FontBold"),		&g_fFontBold, FALSE);
	GetSettingStr(hKey, _T("FontName"),		g_szFontName, LF_FACESIZE, _T("Courier New"));
	GetSettingInt(hKey, _T("FontSmooth"),	&g_nFontSmoothing, DEFAULT_QUALITY);

	GetSettingInt(hKey, _T("PaddingAbove"), &g_nPaddingAbove, 0);
	GetSettingInt(hKey, _T("PaddingBelow"), &g_nPaddingBelow, 1);
	GetSettingInt(hKey, _T("PaddingFlags"), &g_fPaddingFlags, COURIERNEW|LUCIDACONS);

	GetSettingInt(hKey, _T("SelMargin"),	 &g_fSelMargin, TRUE);
	GetSettingInt(hKey, _T("LineNumbers"),   &g_fLineNumbers, FALSE);
	GetSettingInt(hKey, _T("LongLines"),	 &g_fLongLines, TRUE);
	GetSettingInt(hKey, _T("LongLineLimit"), &g_nLongLineLimit, 80);
	GetSettingInt(hKey, _T("SaveOnExit"),	 &g_fSaveOnExit, TRUE);
	
	// read the display colours
	RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_READ, 0, &hColKey, 0);

	GetSettingInt(hColKey, _T("Foreground"),	&g_rgbColourList[TXC_FOREGROUND],		g_rgbAutoColourList[TXC_FOREGROUND]		);
	GetSettingInt(hColKey, _T("Background"),	&g_rgbColourList[TXC_BACKGROUND],		g_rgbAutoColourList[TXC_BACKGROUND]		);	
	GetSettingInt(hColKey, _T("SelFG"),			&g_rgbColourList[TXC_HIGHLIGHTTEXT],	g_rgbAutoColourList[TXC_HIGHLIGHTTEXT]	);
	GetSettingInt(hColKey, _T("SelBG"),			&g_rgbColourList[TXC_HIGHLIGHT],		g_rgbAutoColourList[TXC_HIGHLIGHT]		);
	GetSettingInt(hColKey, _T("SelFG2"),		&g_rgbColourList[TXC_HIGHLIGHTTEXT2],   g_rgbAutoColourList[TXC_HIGHLIGHTTEXT2]	);
	GetSettingInt(hColKey, _T("SelBG2"),		&g_rgbColourList[TXC_HIGHLIGHT2],		g_rgbAutoColourList[TXC_HIGHLIGHT2]		);
	GetSettingInt(hColKey, _T("Margin1"),		&g_rgbColourList[TXC_SELMARGIN1],		g_rgbAutoColourList[TXC_SELMARGIN1]		);
	GetSettingInt(hColKey, _T("Margin2"),		&g_rgbColourList[TXC_SELMARGIN2],		g_rgbAutoColourList[TXC_SELMARGIN2]		);
	GetSettingInt(hColKey, _T("LinenoText"),	&g_rgbColourList[TXC_LINENUMBERTEXT],	g_rgbAutoColourList[TXC_LINENUMBERTEXT]	);
	GetSettingInt(hColKey, _T("Lineno"),		&g_rgbColourList[TXC_LINENUMBER],		g_rgbAutoColourList[TXC_LINENUMBER]		);
	GetSettingInt(hColKey, _T("LongLineText"),	&g_rgbColourList[TXC_LONGLINETEXT],		g_rgbAutoColourList[TXC_LONGLINETEXT]	);
	GetSettingInt(hColKey, _T("LongLine"),		&g_rgbColourList[TXC_LONGLINE],			g_rgbAutoColourList[TXC_LONGLINE]		);
	
	GetSettingBin(hColKey, _T("Custom"),		g_rgbCustColours, sizeof(g_rgbCustColours)); 

	RegCloseKey(hColKey);
	RegCloseKey(hKey);
}

void SaveRegSettings()
{
	HKEY hKey, hColKey;

	// open registry location for writing
	RegCreateKeyEx(HKEY_CURRENT_USER, REGLOC, 0, 0, 0, KEY_WRITE, 0, &hKey, 0);

	WriteSettingInt(hKey, _T("FontSize"),	g_nFontSize);
	WriteSettingInt(hKey, _T("FontBold"),	g_fFontBold);
	WriteSettingStr(hKey, _T("FontName"),	g_szFontName);
	WriteSettingInt(hKey, _T("FontSmooth"),	g_nFontSmoothing);

	WriteSettingInt(hKey, _T("PaddingAbove"), g_nPaddingAbove);
	WriteSettingInt(hKey, _T("PaddingBelow"), g_nPaddingBelow);
	WriteSettingInt(hKey, _T("PaddingFlags"), g_fPaddingFlags);

	WriteSettingInt(hKey, _T("SelMargin"),	  g_fSelMargin);
	WriteSettingInt(hKey, _T("LineNumbers"),  g_fLineNumbers);
	WriteSettingInt(hKey, _T("LongLines"),	  g_fLongLines);
	WriteSettingInt(hKey, _T("SaveOnExit"),	  g_fSaveOnExit);
	WriteSettingInt(hKey, _T("LongLineLimit"),g_nLongLineLimit);

	
	// write the display colours
	RegCreateKeyEx(hKey, _T("Colours"), 0, 0, 0, KEY_WRITE, 0, &hColKey, 0);

	WriteSettingInt(hColKey, _T("Foreground"),	g_rgbColourList[TXC_FOREGROUND]); 
	WriteSettingInt(hColKey, _T("Background"),	g_rgbColourList[TXC_BACKGROUND]); 
	WriteSettingInt(hColKey, _T("SelFG"),		g_rgbColourList[TXC_HIGHLIGHTTEXT]); 
	WriteSettingInt(hColKey, _T("SelBG"),		g_rgbColourList[TXC_HIGHLIGHT]); 
	WriteSettingInt(hColKey, _T("SelFG2"),		g_rgbColourList[TXC_HIGHLIGHTTEXT2]); 
	WriteSettingInt(hColKey, _T("SelBG2"),		g_rgbColourList[TXC_HIGHLIGHT2]); 

	WriteSettingInt(hColKey, _T("Margin1"),		g_rgbColourList[TXC_SELMARGIN1]); 
	WriteSettingInt(hColKey, _T("Margin2"),		g_rgbColourList[TXC_SELMARGIN2]); 
	WriteSettingInt(hColKey, _T("LinenoText"),	g_rgbColourList[TXC_LINENUMBERTEXT]); 
	WriteSettingInt(hColKey, _T("Lineno"),		g_rgbColourList[TXC_LINENUMBER]); 
	WriteSettingInt(hColKey, _T("LongLineText"),g_rgbColourList[TXC_LONGLINETEXT]); 
	WriteSettingInt(hColKey, _T("LongLine"),	g_rgbColourList[TXC_LONGLINE]); 

	WriteSettingInt(hColKey, _T("CurlineText"),	g_rgbColourList[TXC_LONGLINETEXT]); 
	WriteSettingInt(hColKey, _T("Curline"),		g_rgbColourList[TXC_LONGLINE]); 


	WriteSettingBin(hColKey, _T("Custom"),		g_rgbCustColours, sizeof(g_rgbCustColours)); 

	RegCloseKey(hColKey);
	RegCloseKey(hKey);
}

void ApplyRegSettings()
{
	int i;

	if(g_hFont)
		DeleteObject(g_hFont);

	g_hFont = EasyCreateFont(g_nFontSize, g_fFontBold, g_nFontSmoothing, g_szFontName);

	TextView_SetLineSpacing(g_hwndTextView, g_nPaddingAbove, g_nPaddingBelow);

	TextView_SetStyleBool(g_hwndTextView, TXS_SELMARGIN,	g_fSelMargin);
	TextView_SetStyleBool(g_hwndTextView, TXS_LINENUMBERS,	g_fLineNumbers);
	TextView_SetStyleBool(g_hwndTextView, TXS_LONGLINES,	g_fLongLines);

	TextView_SetStyleBool(g_hwndTextView, TXS_HIGHLIGHTCURLINE,	FALSE);

	TextView_SetCaretWidth(g_hwndTextView, 2);
	TextView_SetLongLine(g_hwndTextView, g_nLongLineLimit);
	
	SendMessage(g_hwndTextView, WM_SETFONT, (WPARAM)g_hFont, 0);

	for(i = 0; i < TXC_MAX_COLOURS; i++)
	{
		TextView_SetColor(g_hwndTextView, i, g_rgbColourList[i]);
	}
}

void ShowProperties(HWND hwndParent)
{
	PROPSHEETHEADER psh    = {   sizeof(psh)   };
	PROPSHEETPAGE   psp[3] = {  { sizeof(psp[0]) },  
								{ sizeof(psp[1]) }, 
								{ sizeof(psp[2]) }, 
							};

	CoInitialize(0);
	
	// configure property sheet
	psh.dwFlags			= PSH_PROPSHEETPAGE;
	psh.hwndParent		= hwndParent;
	psh.nPages			= sizeof(psp) / sizeof(psp[0]);
	psh.ppsp			= psp;
	psh.pszCaption		= _T("Options");

	// configure property sheet page(1)
	psp[0].dwFlags		= PSP_USETITLE;
	psp[0].hInstance	= GetModuleHandle(0);
	psp[0].pfnDlgProc	= FontOptionsDlgProc;
	psp[0].pszTemplate	= MAKEINTRESOURCE(IDD_FONT);
	psp[0].pszTitle		= _T("Font");

	// configure property sheet page(2)
	psp[1].dwFlags		= PSP_USETITLE;
	psp[1].hInstance	= GetModuleHandle(0);
	psp[1].pfnDlgProc	= DisplayOptionsDlgProc;
	psp[1].pszTemplate	= MAKEINTRESOURCE(IDD_DISPLAY);
	psp[1].pszTitle		= _T("Display");

	// configure property sheet page(2)
	psp[2].dwFlags		= PSP_USETITLE;
	psp[2].hInstance	= GetModuleHandle(0);
	psp[2].pfnDlgProc	= MiscOptionsDlgProc;
	psp[2].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS);
	psp[2].pszTitle		= _T("Settings");

	if(PropertySheet(&psh))
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