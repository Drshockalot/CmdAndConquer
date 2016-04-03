#include "../Header/CmdAndConquer_MainWindow.h"
#include "../Header/CmdAndConquer_Globals.h"
#include "../../Batch/Headers/BatchSyntax.h"

HDC	ShowPrintDlg(HWND hwndParent);
TCHAR		g_szAppName[] = APP_TITLE;
HWND		g_hwndMain;

HWND		g_hwndTextView;
HWND		g_hwndBatchRunResults;
HWND		g_hwndStatusbar;
HWND		g_hwndSearchDlg;
HWND		g_hwndToolbar;
//HFONT		g_hFont;

TCHAR		g_szFileName[MAX_PATH];
TCHAR		g_szFileTitle[MAX_PATH];
BOOL		g_fFileChanged = FALSE;

TCHAR		*g_szEditMode[] = { _T("READ"), _T("INS"), _T("OVR") };

CmdAndConquer_MainWindow::CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText) : szAppName(APP_TITLE), hWnd_(NULL)
{
	//	Register the window class before creation
	registerWindowClass(hInstance);
	assert(HIWORD(class_atom) == 0);
	assert(class_atom);
	if (!class_atom)
		throw std::logic_error("Attempted to create window without registering class");
	HWND hWnd = CreateWindow(reinterpret_cast<LPCTSTR>(class_atom),
							 windowText,
							 WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT,
							 CW_USEDEFAULT,
							 CW_USEDEFAULT,
							 CW_USEDEFAULT,
							 NULL,
							 NULL,
							 hInstance,
							 this);

	if (!hWnd) throw std::runtime_error("Unable to create window");
	assert(hWnd == hWnd_);
	ShowWindow(hWnd_, cmdShow);
}

CmdAndConquer_MainWindow::~CmdAndConquer_MainWindow(){};

void CmdAndConquer_MainWindow::registerWindowClass(HINSTANCE hInstance)
{
	assert(!class_atom);
	if (class_atom)
		throw std::runtime_error("registerWindowClass() called multiple times");

	WNDCLASSEX wcx;
	
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = &CmdAndConquer_MainWindow::InitialWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)0;
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcx.lpszClassName = CmdAndConquer_MainWindow::class_name;
	wcx.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_CREATEDIBSECTION);
	wcx.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_CREATEDIBSECTION);

	class_atom = RegisterClassEx(&wcx);
	if (class_atom == 0) throw std::runtime_error("Unable to register window class");
}

ATOM CmdAndConquer_MainWindow::getClassATOM(void)
{
	assert(class_atom != 0);
	if (class_atom == 0) throw std::logic_error("Attempted to get class ATOM without registering class");
	return class_atom;
}

LPCTSTR CmdAndConquer_MainWindow::getClassName(void)
{
	return class_name;
}

HWND CmdAndConquer_MainWindow::getMainHWND()
{
	return hWnd_;
}

void CmdAndConquer_MainWindow::setImageList()
{
	HIMAGELIST hImgList = ImageList_LoadImage(
		GetModuleHandle(0),
		MAKEINTRESOURCE(IDB_BITMAP1),
		16, 0,
		RGB(255, 0, 255),
		IMAGE_BITMAP,
		LR_LOADTRANSPARENT | LR_CREATEDIBSECTION
		);

	TextView_SetImageList(CC_hwndTextView, hImgList);

	// highlight specific lines with image-index "1"
	/*TextView_SetLineImage(CC_hwndTextView, 16, 1);
	TextView_SetLineImage(CC_hwndTextView, 5, 1);
	TextView_SetLineImage(CC_hwndTextView, 36, 1);
	TextView_SetLineImage(CC_hwndTextView, 11, 1);*/
}

LRESULT CALLBACK CmdAndConquer_MainWindow::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static int width, height, heightsb;
	RECT rect;
	HDWP hdwp;
	NMHDR *nmhdr;
	TCHAR msgstr[MAX_PATH + 200];

	switch (Msg)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			void * lpCreateParam = create_struct->lpCreateParams;
			CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(lpCreateParam);
			assert(this_window == this);

			this->onCreate(hWnd_, reinterpret_cast<CREATESTRUCT*>(lParam));

			return 0;
		}

		case WM_DROPFILES:
			HandleDropFiles(hWnd, (HDROP)wParam);
			return 0;

		case WM_DESTROY:
			SaveFileData(g_szFileName, hWnd);
			PostQuitMessage(0);
			DeleteObject(g_hFont);
			return 0;

			//case WM_NCCALCSIZE:
			//	return NcCalcSize(hwnd, wParam, lParam);

		case WM_INITMENU:
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_LINENUMBERS, g_fLineNumbers);
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_LONGLINES, g_fLongLines);
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_SAVEEXIT, g_fSaveOnExit);
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_STATUSBAR, g_fShowStatusbar);
			//CheckMenuCommand((HMENU)wParam, IDM_VIEW_SEARCHBAR,		g_hwndSearchBar ? TRUE : FALSE);

			EnableMenuCommand((HMENU)wParam, IDM_EDIT_UNDO, TextView_CanUndo(g_hwndTextView));
			EnableMenuCommand((HMENU)wParam, IDM_EDIT_REDO, TextView_CanRedo(g_hwndTextView));
			EnableMenuCommand((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TEXT));
			EnableMenuCommand((HMENU)wParam, IDM_EDIT_COPY, TextView_GetSelSize(g_hwndTextView));
			EnableMenuCommand((HMENU)wParam, IDM_EDIT_CUT, TextView_GetSelSize(g_hwndTextView));
			EnableMenuCommand((HMENU)wParam, IDM_EDIT_DELETE, TextView_GetSelSize(g_hwndTextView));

			return 0;

			//case WM_USER:
			//	wsprintf(msgstr, _T("%s\n\nThis file has been modified outside of Neatpad.")
			//					 _T("Do you wish to reload it?"), g_szFileName);
			//	MessageBox(hwnd, msgstr, _T("Neatpad"), MB_ICONQUESTION|MB_YESNO);
			//
			//	return 0;

		case WM_ENABLE:

			// keep the modeless find/replace dialog in the same enabled state as the main window
			EnableWindow(g_hwndSearchDlg, (BOOL)wParam);
			return 0;

		case WM_MENUSELECT:
			StatusBarMenuSelect(hWnd, g_hwndStatusbar, wParam, lParam);
			return 0;

		case WM_NOTIFY:
			nmhdr = (NMHDR *)lParam;

			if (nmhdr->hwndFrom == g_hwndTextView)
				return TextViewNotifyHandler(hWnd, nmhdr);
			else
				return NotifyHandler(hWnd, nmhdr);

		case WM_COMMAND:
			return CommandHandler(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

		case WM_SETFOCUS:
			SetFocus(g_hwndTextView);
			return 0;

		case WM_CLOSE:

			// does the file need saving?
			if (g_fFileChanged)
			{
				wsprintf(msgstr, _T("Do you want to save changes to\r\n%s?"), g_szFileName);

				switch (MessageBox(hWnd, msgstr, APP_TITLE, MB_YESNOCANCEL | MB_ICONQUESTION))
				{
					case IDYES:
						DoSaveFile(hWnd, g_szFileName, g_szFileTitle);
						break;

					case IDCANCEL:
						return 0;
						break;

					case IDNO:
					default:
						break;
				}
			}

			DestroyWindow(hWnd);
			return 0;

		case WM_SIZE:

			// resize the TextView and StatusBar to fit within the main window's client area
			width = (short)LOWORD(lParam);
			height = (short)HIWORD(lParam);

			GetWindowRect(g_hwndStatusbar, &rect);
			heightsb = rect.bottom - rect.top;

			hdwp = BeginDeferWindowPos(7);

			if (g_fShowStatusbar)
			{
				DeferWindowPos(hdwp, g_hwndStatusbar, 0, 0, height - heightsb, width, heightsb, SWP_SHOWWINDOW);
				//	MoveWindow(g_hwndStatusbar, 0, height - heightsb, width, heightsb, TRUE);
				height -= heightsb;
			}

			if (g_fShowBatchResultsWindow)
			{
				DeferWindowPos(hdwp, g_hwndBatchRunResults, 0, 0, height - 175, width, 175, SWP_SHOWWINDOW);
				DeferWindowPos(hdwp, this->CC_hwndBatchRunResultsHeader, 0, 0, height - 200, width - 25, 25, SWP_SHOWWINDOW);
				DeferWindowPos(hdwp, this->CC_hwndBatchRunResultsHeaderExitButton, 0, width - 25, height - 200, 25, 25, SWP_SHOWWINDOW);
				height = height - 200;
			}

			//Remove height futher for the toolbar
			height -= 32;
			
			if (g_fShowAddCMDWindow)
			{
				DeferWindowPos(hdwp, g_hwndAddCMDWindow, 0, width - 300, 57, 300, height - 25, SWP_SHOWWINDOW);
				DeferWindowPos(hdwp, this->CC_hwndAddCMDWindowHeader, 0, width - 300, 32, 275, 25, SWP_SHOWWINDOW);
				DeferWindowPos(hdwp, this->CC_hwndAddCMDWindowHeaderExitButton, 0, width - 25, 32, 25, 25, SWP_SHOWWINDOW);
				width -= 300;
			}

			DeferWindowPos(hdwp, g_hwndTextView, 0, 0, 32, width, height, SWP_SHOWWINDOW);
			//MoveWindow(g_hwndTextView, 0, 0, width, height, TRUE);

			EndDeferWindowPos(hdwp);

			SetStatusBarParts(g_hwndStatusbar);
			::SendMessage(g_hwndToolbar, WM_SIZE, 0, 0);
			SetWindowPos(this->CC_hwndBatchRunResultsHeaderExitButton, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(this->CC_hwndBatchRunResultsHeader, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			return 0;

	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_NCCREATE)
	{
		LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		void * lpCreateParam = create_struct->lpCreateParams;
		CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(lpCreateParam);
		assert(this_window->hWnd_ == 0); // this should be the first (and only) time
		// WM_NCCREATE is processed
		this_window->hWnd_ = hWnd;
		SetWindowLongPtr(hWnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(this_window));
		SetWindowLongPtr(hWnd,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(&CmdAndConquer_MainWindow::StaticWndProc)); 
		return this_window->WndProc(hWnd, Msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(user_data);
	assert(this_window);
	assert(hWnd == this_window->hWnd_);
	return this_window->WndProc(hWnd, Msg, wParam, lParam);
}

int CmdAndConquer_MainWindow::onCreate(HWND hWnd, CREATESTRUCT *cs)
{	
	PostMessage(hWnd, WM_COMMAND, IDM_FILE_NEW, 0);

	DragAcceptFiles(hWnd, TRUE);

	g_hwndStatusbar = CreateStatusBar(hWnd);
	this->CC_hwndTextView = CreateTextView(hWnd_);
	InitAddCMDWindow();
	g_hwndMain = hWnd_;
	this->CC_hwndBatchRunResults = CreateBatchScriptResultWindow(hWnd_);
	this->CC_hwndBatchRunResultsHeader = CreateBatchScriptResultWindowHeader(hWnd_);
	this->CC_hwndBatchRunResultsHeaderExitButton = CreateBatchScriptResultWindowHeaderExitButton(hWnd_);
	this->CC_hwndAddCMDWindow = CreateAddCMDWindow(hWnd_);
	this->CC_hwndAddCMDWindowHeader = CreateAddCMDWindowHeader(hWnd_);
	this->CC_hwndAddCMDWindowHeaderExitButton = CreateAddCMDWindowHeaderExitButton(hWnd_);
	g_hwndAddCMDWindow = this->CC_hwndAddCMDWindow;
	g_hwndBatchRunResults = this->CC_hwndBatchRunResults;
	g_hwndTextView = this->CC_hwndTextView;

	this->setImageList();
	g_hwndToolbar = initToolbar(hWnd, cs);

	return 0;
}

HWND CmdAndConquer_MainWindow::initToolbar(HWND hWnd, CREATESTRUCT *cs)
{
	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&InitCtrlEx);

	TBADDBITMAP tbab;
	TBBUTTON tbb[13];

	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT | TBSTYLE_FLAT, 0, 0, 0, 0, hWnd, 0, cs->hInstance, NULL);

	::SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	::SendMessage(hWndToolbar, TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(20, 20));

	tbab.hInst = cs->hInstance;

	tbab.nID = IDB_BITMAP12;	// NEW DOCUMENT

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP11;	//OPEN DOCUMENT

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP17;	//SAVE DOCUMENT

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP15;	//CLOSE DOCUMENT

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP18;	//PRINT DOCUMENT

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP14;	//CUT SELECTION

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP13;	//COPY SELECTION

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP16;	//PASTE

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP20;	//REDO

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	tbab.nID = IDB_BITMAP19;	//UNDO

	::SendMessage(hWndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

	ZeroMemory(tbb, sizeof(tbb));

	tbb[0].iBitmap = 0;
	tbb[0].idCommand = TOOLBARCOMMAND_NEW;
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_BUTTON;

	tbb[1].iBitmap = 1;
	tbb[1].idCommand = TOOLBARCOMMAND_OPEN;
	tbb[1].fsState = TBSTATE_ENABLED;
	tbb[1].fsStyle = TBSTYLE_BUTTON;

	tbb[2].iBitmap = 2;
	tbb[2].idCommand = TOOLBARCOMMAND_SAVE;
	tbb[2].fsState = TBSTATE_ENABLED;
	tbb[2].fsStyle = TBSTYLE_BUTTON;

	tbb[3].iBitmap = 3;
	tbb[3].idCommand = TOOLBARCOMMAND_CLOSE;
	tbb[3].fsState = TBSTATE_ENABLED;
	tbb[3].fsStyle = TBSTYLE_BUTTON;

	tbb[4].fsStyle = TBSTYLE_SEP;

	tbb[5].iBitmap = 4;
	tbb[5].idCommand = TOOLBARCOMMAND_PRINT;
	tbb[5].fsState = TBSTATE_ENABLED;
	tbb[5].fsStyle = TBSTYLE_BUTTON;

	tbb[6].fsStyle = TBSTYLE_SEP;

	tbb[7].iBitmap = 5;
	tbb[7].idCommand = TOOLBARCOMMAND_CUT;
	tbb[7].fsState = TBSTATE_ENABLED;
	tbb[7].fsStyle = TBSTYLE_BUTTON;

	tbb[8].iBitmap = 6;
	tbb[8].idCommand = TOOLBARCOMMAND_COPY;
	tbb[8].fsState = TBSTATE_ENABLED;
	tbb[8].fsStyle = TBSTYLE_BUTTON;

	tbb[9].iBitmap = 7;
	tbb[9].idCommand = TOOLBARCOMMAND_PASTE;
	tbb[9].fsState = TBSTATE_ENABLED;
	tbb[9].fsStyle = TBSTYLE_BUTTON;

	tbb[10].fsStyle = TBSTYLE_SEP;

	tbb[11].iBitmap = 8;
	tbb[11].idCommand = TOOLBARCOMMAND_UNDO;
	tbb[11].fsState = TBSTATE_ENABLED;
	tbb[11].fsStyle = TBSTYLE_BUTTON;

	tbb[12].iBitmap = 9;
	tbb[12].idCommand = TOOLBARCOMMAND_REDO;
	tbb[12].fsState = TBSTATE_ENABLED;
	tbb[12].fsStyle = TBSTYLE_BUTTON;

	::SendMessage(hWndToolbar, TB_ADDBUTTONS, 13, (LPARAM)&tbb);

	::SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_UNDO, (LPARAM)FALSE);
	::SendMessage(hWndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_REDO, (LPARAM)FALSE);

	ShowWindow(hWndToolbar, SW_SHOW);
	return hWndToolbar;
}

BOOL CmdAndConquer_MainWindow::ShowOpenFileDlg(HWND hwnd, TCHAR *pstrFileName, TCHAR *pstrTitleName)
{
	TCHAR *szFilter = _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");

	OPENFILENAME ofn = { sizeof(ofn) };

	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter = szFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;

	ofn.nFilterIndex = 1;
	ofn.nMaxFile = _MAX_PATH;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;

	// flags to control appearance of open-file dialog
	ofn.Flags = OFN_EXPLORER |
		OFN_ENABLESIZING |
		OFN_ALLOWMULTISELECT |
		OFN_FILEMUSTEXIST;

	return GetOpenFileName(&ofn);
}

//
//	Set the main window filename
//
void SetWindowFileName(HWND hwnd, TCHAR *szFileName, BOOL fModified)
{
	TCHAR ach[MAX_PATH + sizeof(g_szAppName) + 4];
	TCHAR mod[4] = _T("");

	if (fModified)
		lstrcpy(mod, _T(" *"));

	wsprintf(ach, _T("%s - %s%s"), szFileName, g_szAppName, mod);
	SetWindowText(hwnd, ach);
}

void CmdAndConquer_MainWindow::ShowAboutDlg(HWND hwndParent)
{
	DialogBoxParam(0, MAKEINTRESOURCE(IDD_ABOUT), hwndParent, &AboutDlgProc, 0);
}

//
//	About dialog-proc
//
INT_PTR CALLBACK CmdAndConquer_MainWindow::AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HICON	hIcon;
	HFONT	hFont;
	RECT	rect;
	HWND	hwndUrl;
	HWND	hwndStatic;

	switch (msg)
	{
	case WM_INITDIALOG:

		SendMessage(hwnd, WM_SETFONT, (WPARAM)g_hFont, 0);

		CenterWindow(hwnd);

		//
		//	Set the dialog-icon 
		//
		hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 48, 48, 0);
		SendDlgItemMessage(hwnd, IDC_HEADER2, STM_SETIMAGE, IMAGE_ICON, (WPARAM)hIcon);

		//
		//	Get the current font for the dialog and create a BOLD version,
		//	set this as the AppName static-label's font
		//
		hFont = CreateBoldFontFromHwnd(hwnd);
		SendDlgItemMessage(hwnd, IDC_ABOUT_APPNAME, WM_SETFONT, (WPARAM)hFont, 0);

		//
		//	Locate the existing static-control which displays our homepage
		//	Create a SysLink control right over the top of it (assuming current
		//	version of Windows supports it)
		//
		hwndStatic = GetDlgItem(hwnd, IDC_ABOUT_URL);
		GetClientRect(hwndStatic, &rect);
		MapWindowPoints(hwndStatic, hwnd, (POINT *)&rect, 2);

		hwndUrl = CreateWindow(WC_LINK, SYSLINK_STR,
			WS_TABSTOP | WS_CHILD | WS_VISIBLE,
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hwnd, 0, 0, 0
			);

		if (hwndUrl)
		{
			SendMessage(hwndUrl, WM_SETFONT, (WPARAM)hFont, 0);
			ShowWindow(hwndStatic, SW_HIDE);
		}

		return TRUE;

	case WM_NOTIFY:

		// Spawn the default web-browser when the SysLink control is clicked
		switch (((NMHDR *)lParam)->code)
		{
		case NM_CLICK: case NM_RETURN:
			ShellExecute(hwnd, _T("open"), WEBSITE_URL, 0, 0, SW_SHOWNORMAL);
			return 0;
		}

		break;

	case WM_CLOSE:

		EndDialog(hwnd, TRUE);
		return TRUE;

	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			EndDialog(hwnd, TRUE);

		break;
	}

	return FALSE;
}

//
//	WM_NOTIFY handler for the TextView notification messages
//
UINT CmdAndConquer_MainWindow::TextViewNotifyHandler(HWND hwnd, NMHDR *nmhdr)
{
	switch (nmhdr->code)
	{
		// document has changed due to text input / undo / redo, update
		// the main window-title to show an asterisk next to the filename
	case TVN_CHANGED:

		if (g_szFileTitle[0])
		{
			BOOL fModified = TextView_CanUndo(g_hwndTextView);

			if (fModified != g_fFileChanged)
			{
				SetWindowFileName(hwnd, g_szFileTitle, fModified);
				g_fFileChanged = fModified;
			}
		}
		break;

		// cursor position has changed, update the statusbar info
	case TVN_CURSOR_CHANGE:

		SetStatusBarText(g_hwndStatusbar, 1, 0, _T(" Ln %d, Col %d"),
			TextView_GetCurLine(g_hwndTextView) + 1,
			TextView_GetCurCol(g_hwndTextView) + 1);

		break;

		// edit/insert mode changed, update statusbar info
	case TVN_EDITMODE_CHANGE:

		SetStatusBarText(g_hwndStatusbar, 2, 0,
			g_szEditMode[TextView_GetEditMode(g_hwndTextView)]);

		break;

	default:
		break;
	}

	return 0;
}

//
//	Generic WM_NOTIFY handler for all other messages
//
UINT CmdAndConquer_MainWindow::NotifyHandler(HWND hwnd, NMHDR *nmhdr)
{
	NMMOUSE *nmmouse;
	UINT	 nMode;

	switch (nmhdr->code)
	{
	case NM_DBLCLK:

		// statusbar is the only window at present which sends double-clicks
		nmmouse = (NMMOUSE *)nmhdr;

		// toggle the Readonly/Insert/Overwrite mode
		if (nmmouse->dwItemSpec == 2)
		{
			nMode = TextView_GetEditMode(g_hwndTextView);
			nMode = (nMode + 1) % 3;

			TextView_SetEditMode(g_hwndTextView, nMode);

			SetStatusBarText(g_hwndStatusbar, 2, 0, g_szEditMode[nMode]);
		}

		break;

	default:
		break;
	}

	return 0;
}

//
//	WM_COMMAND message handler for main window
//
UINT CmdAndConquer_MainWindow::CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom)
{
	RECT rect;

	switch (nCtrlId)
	{
	case IDM_FILE_NEW: case TOOLBARCOMMAND_NEW:

		// reset to an empty file
		SetWindowFileName(hwnd, _T("Untitled"), FALSE);
		TextView_Clear(this->CC_hwndTextView);

		g_szFileTitle[0] = '\0';
		g_fFileChanged = FALSE;
		return 0;

	case IDM_FILE_OPEN: case TOOLBARCOMMAND_OPEN:

		// get a filename to open
		if (ShowOpenFileDlg(hwnd, g_szFileName, g_szFileTitle))
		{
			DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
		}

		return 0;

	case IDM_FILE_SAVE: case TOOLBARCOMMAND_SAVE:
		if(!DoSaveFile(hwnd, g_szFileName, g_szFileTitle))
		{
			MessageBox(hwnd, _T("Save Failed"), APP_TITLE, MB_ICONINFORMATION);
		}
		return 0;

	case IDM_FILE_SAVEAS:

		// does nothing yet
		if (ShowSaveFileDlg(hwnd, g_szFileName, g_szFileTitle))
		{
			if (!DoSaveFile(hwnd, g_szFileName, g_szFileTitle))
			{
				MessageBox(hwnd, _T("Save Failed"), APP_TITLE, MB_ICONINFORMATION);
			}
		}

		return 0;

	case IDM_FILE_PRINT: case TOOLBARCOMMAND_PRINT:

		DeleteDC(
			ShowPrintDlg(hwnd)
			);

		return 0;

	case IDM_FILE_EXIT: case TOOLBARCOMMAND_CLOSE:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return 0;

	case IDM_EDIT_UNDO:	case WM_UNDO: case TOOLBARCOMMAND_UNDO:
		SendMessage(this->CC_hwndTextView, WM_UNDO, 0, 0);
		SendMessage(g_hwndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_UNDO, (LPARAM)TextView_CanUndo(g_hwndTextView));
		SendMessage(g_hwndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_REDO, (LPARAM)TextView_CanRedo(g_hwndTextView));
		return 0;

	case IDM_EDIT_REDO: case TOOLBARCOMMAND_REDO:
		SendMessage(this->CC_hwndTextView, TXM_REDO, 0, 0);
		SendMessage(g_hwndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_UNDO, (LPARAM)TextView_CanUndo(g_hwndTextView));
		SendMessage(g_hwndToolbar, TB_ENABLEBUTTON, (WPARAM)TOOLBARCOMMAND_REDO, (LPARAM)TextView_CanRedo(g_hwndTextView));
		return 0;

	case IDM_EDIT_COPY: case WM_COPY: case TOOLBARCOMMAND_COPY:
		SendMessage(this->CC_hwndTextView, WM_COPY, 0, 0);
		return 0;

	case IDM_EDIT_CUT: case WM_CUT: case TOOLBARCOMMAND_CUT:
		SendMessage(this->CC_hwndTextView, WM_CUT, 0, 0);
		return 0;

	case IDM_EDIT_PASTE: case WM_PASTE: case TOOLBARCOMMAND_PASTE:
		SendMessage(this->CC_hwndTextView, WM_PASTE, 0, 0);
		return 0;

	case IDM_EDIT_DELETE: case WM_CLEAR:
		SendMessage(this->CC_hwndTextView, WM_CLEAR, 0, 0);
		return 0;

	case IDM_EDIT_FIND:
		ShowFindDlg(hwnd, FIND_PAGE);
		return 0;

	case IDM_EDIT_REPLACE:
		ShowFindDlg(hwnd, REPLACE_PAGE);
		return 0;

	case IDM_EDIT_GOTO:
		ShowFindDlg(hwnd, GOTO_PAGE);
		return 0;


	case IDM_EDIT_SELECTALL:
		TextView_SelectAll(this->CC_hwndTextView);
		return 0;

	case IDM_VIEW_OPTIONS:
		ShowOptions(hwnd);
		return 0;

	case IDM_VIEW_LINENUMBERS:
		g_fLineNumbers = !g_fLineNumbers;
		TextView_SetStyleBool(this->CC_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
		return 0;

	case IDM_VIEW_LONGLINES:
		g_fLongLines = !g_fLongLines;
		TextView_SetStyleBool(this->CC_hwndTextView, TXS_LONGLINES, g_fLongLines);
		return 0;

	case IDM_VIEW_STATUSBAR:
		g_fShowStatusbar = !g_fShowStatusbar;
		ShowWindow(g_hwndStatusbar, SW_HIDE);
		GetClientRect(hwnd, &rect);
		PostMessage(hwnd, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
		return 0;

	case IDM_VIEW_SAVEEXIT:
		g_fSaveOnExit = !g_fSaveOnExit;
		return 0;

	case IDM_VIEW_SAVENOW:
		SaveRegSettings();
		return 0;

	case IDM_HELP_ABOUT:
		ShowAboutDlg(hwnd);
		return 0;

	case IDM_BATCH_RUN:
		TextView_RunFileAsBatch(this->CC_hwndTextView);
		ShowWindow(g_hwndBatchRunResults, SW_HIDE);
		GetClientRect(hwnd, &rect);
		PostMessage(hwnd, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
		return 0;
	case IDM_CMD_ADDCMD:
		ShowAddCMDWindow();
		ShowWindow(g_hwndSearchBar, SW_HIDE);
		GetClientRect(hwnd, &rect);
		PostMessage(hwnd, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
		return 0;
	case ID_FORLOOPS_FOR_Files:
		TextView_AddForStatement(g_hwndTextView, FORFiles);
		return 0;
	case ID_FORLOOPS_FOR_FileRootedAtPath:
		TextView_AddForStatement(g_hwndTextView, FORFilesRootedAtPath);
		return 0;
	case ID_FORLOOPS_FOR_Folders:
		TextView_AddForStatement(g_hwndTextView, FORFolders);
		return 0;
	case ID_FORLOOPS_FOR_ListOfNumbers:
		TextView_AddForStatement(g_hwndTextView, FORListOfNumbers);
		return 0;
	case ID_FORLOOPS_FOR_FileListContents:
		TextView_AddForStatement(g_hwndTextView, FORFileSetContents);
		return 0;
	case ID_FORLOOPS_FOR_FileTextStringContents:
		TextView_AddForStatement(g_hwndTextView, FORFileTextStringContents);
		return 0;
	case ID_FORLOOPS_FOR_CommandResults:
		TextView_AddForStatement(g_hwndTextView, FORCommandResults);
		return 0;
	case ID_IFSTATEMENTS_IFNOTEXIST:
		TextView_AddIFStatement(g_hwndTextView, IFExist);
		return 0;
	case ID_IFSTATEMENTS_IFNOTEXISTELSE:
		TextView_AddIFStatement(g_hwndTextView, IFExistElse);
		return 0;
	case ID_IFSTATEMENTS_IFEQUALS:
		TextView_AddIFStatement(g_hwndTextView, IFEquals);
		return 0;
	case ID_IFSTATEMENTS_IFCOMPARISON:
		TextView_AddIFStatement(g_hwndTextView, IFComparison);
		return 0;
	case ID_IFSTATEMENTS_IFCOMPARISONELSE:
		TextView_AddIFStatement(g_hwndTextView, IFComparisonElse);
		return 0;
	case ID_IFSTATEMENTS_IFNOTDEFINED:
		TextView_AddIFStatement(g_hwndTextView, IFNotDefined);
		return 0;
	case ID_IFSTATEMENTS_IFNOTERRORLEVEL:
		TextView_AddIFStatement(g_hwndTextView, IFErrorLevel);
		return 0;
	case BATCH_RESULTS_WINDOW_EXIT_KEY:
	{
		switch (nCtrlCode)
		{
			case BN_CLICKED:
			{
				g_fShowBatchResultsWindow = FALSE;
				ShowWindow(this->CC_hwndBatchRunResultsHeader, SW_HIDE);
				ShowWindow(this->CC_hwndBatchRunResultsHeaderExitButton, SW_HIDE);
				ShowWindow(this->CC_hwndBatchRunResults, SW_HIDE);
				RECT rect;
				GetClientRect(g_hwndMain, &rect);
				PostMessage(g_hwndMain, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
				return 0;
			}
		}
	}
	case ADD_CMD_WINDOW_EXIT_KEY:
	{
		switch (nCtrlCode)
		{
			case BN_CLICKED:
			{
				g_fShowAddCMDWindow = FALSE;
				ShowWindow(this->CC_hwndAddCMDWindowHeader, SW_HIDE);
				ShowWindow(this->CC_hwndAddCMDWindowHeaderExitButton, SW_HIDE);
				ShowWindow(this->CC_hwndAddCMDWindow, SW_HIDE);
				RECT rect;
				GetClientRect(g_hwndMain, &rect);
				PostMessage(g_hwndMain, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
				return 0;
			}
		}
	}
	default:
		return 0;
	}
}

HWND CmdAndConquer_MainWindow::CreateBatchScriptResultWindowHeader(HWND hWnd)
{
	return CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_STATIC, _T(""),
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		hWnd,
		0,
		GetModuleHandle(0),
		0);
}

HWND CmdAndConquer_MainWindow::CreateBatchScriptResultWindowHeaderExitButton(HWND hWnd)
{
	return CreateWindowEx(0, WC_BUTTON, _T("X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON ,
		0, 0, 0, 0,
		hWnd,
		(HMENU)BATCH_RESULTS_WINDOW_EXIT_KEY,
		GetModuleHandle(0),
		0);
}

HWND CmdAndConquer_MainWindow::CreateAddCMDWindowHeader(HWND hWnd)
{
	return CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_STATIC, _T(""),
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		hWnd,
		0,
		GetModuleHandle(0),
		0);
}

HWND CmdAndConquer_MainWindow::CreateAddCMDWindowHeaderExitButton(HWND hWnd)
{
	return CreateWindowEx(0, WC_BUTTON, _T("X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, 0, 0,
		hWnd,
		(HMENU)ADD_CMD_WINDOW_EXIT_KEY,
		GetModuleHandle(0),
		0);
}