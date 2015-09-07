#include "../Header/CmdAndConquer_MainWindow.h"
#include "../Header/CmdAndConquer_Globals.h"
HDC	ShowPrintDlg(HWND hwndParent);
TCHAR		g_szAppName[] = APP_TITLE;
HWND		g_hwndMain;

HWND		g_hwndTextView;
HWND		g_hwndStatusbar;
HWND		g_hwndSearchDlg;
HFONT		g_hFont;

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

			hdwp = BeginDeferWindowPos(3);

			if (g_fShowStatusbar)
			{
				DeferWindowPos(hdwp, g_hwndStatusbar, 0, 0, height - heightsb, width, heightsb, SWP_SHOWWINDOW);
				//	MoveWindow(g_hwndStatusbar, 0, height - heightsb, width, heightsb, TRUE);
				height -= heightsb;
			}

			DeferWindowPos(hdwp, g_hwndTextView, 0, 0, 0, width, height, SWP_SHOWWINDOW);
			//MoveWindow(g_hwndTextView, 0, 0, width, height, TRUE);

			EndDeferWindowPos(hdwp);

			SetStatusBarParts(g_hwndStatusbar);

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
	g_hwndTextView = this->CC_hwndTextView;

	this->setImageList();
	//initToolbar(hWnd, cs);

	return 0;
}

//void CmdAndConquer_MainWindow::initToolbar(HWND hWnd, CREATESTRUCT *cs)
//{
//	INITCOMMONCONTROLSEX InitCtrlEx;
//
//	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
//	InitCtrlEx.dwICC = ICC_BAR_CLASSES;
//	InitCommonControlsEx(&InitCtrlEx);
//
//	TBBUTTON tbrButtons[7];
//
//	tbrButtons[0].iBitmap = 0;
//	tbrButtons[0].idCommand = IDM_FILE_NEW;
//	tbrButtons[0].fsState = TBSTATE_ENABLED;
//	tbrButtons[0].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[0].dwData = 0L;
//	tbrButtons[0].iBitmap = 0;
//	tbrButtons[0].iString = 0;
//
//	tbrButtons[1].iBitmap = 1;
//	tbrButtons[1].idCommand = IDM_FILE_OPEN;
//	tbrButtons[1].fsState = TBSTATE_ENABLED;
//	tbrButtons[1].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[1].dwData = 0L;
//	tbrButtons[1].iString = 0;
//
//	tbrButtons[2].iBitmap = 0;
//	tbrButtons[2].idCommand = IDM_FILE_EXIT;
//	tbrButtons[2].fsState = TBSTATE_ENABLED;
//	tbrButtons[2].fsStyle = TBSTYLE_SEP;
//	tbrButtons[2].dwData = 0L;
//	tbrButtons[2].iString = 0;
//
//	tbrButtons[3].iBitmap = 2;
//	tbrButtons[3].idCommand = IDM_FILE_OPEN;
//	tbrButtons[3].fsState = TBSTATE_ENABLED;
//	tbrButtons[3].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[3].dwData = 0L;
//	tbrButtons[3].iString = 0;
//
//	tbrButtons[4].iBitmap = 3;
//	tbrButtons[4].idCommand = IDM_FILE_OPEN;
//	tbrButtons[4].fsState = TBSTATE_ENABLED;
//	tbrButtons[4].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[4].dwData = 0L;
//	tbrButtons[4].iString = 0;
//
//	tbrButtons[5].iBitmap = 4;
//	tbrButtons[5].idCommand = IDM_FILE_OPEN;
//	tbrButtons[5].fsState = TBSTATE_ENABLED;
//	tbrButtons[5].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[5].dwData = 0L;
//	tbrButtons[5].iString = 0;
//
//	tbrButtons[6].iBitmap = 5;
//	tbrButtons[6].idCommand = IDM_FILE_OPEN;
//	tbrButtons[6].fsState = TBSTATE_ENABLED;
//	tbrButtons[6].fsStyle = TBSTYLE_BUTTON;
//	tbrButtons[6].dwData = 0L;
//	tbrButtons[6].iString = 0;
//
//	HWND hWndToolbar;
//	hWndToolbar = CreateToolbarEx(hWnd,
//		WS_VISIBLE | WS_CHILD | WS_BORDER,
//		IDR_TOOLBAR3,
//		7,
//		cs->hInstance,
//		IDR_TOOLBAR3,
//		tbrButtons,
//		7,
//		16, 16, 16, 16,
//		sizeof(TBBUTTON));
//
//	UpdateWindow(hWnd);
//}

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
	case IDM_FILE_NEW:

		// reset to an empty file
		SetWindowFileName(hwnd, _T("Untitled"), FALSE);
		TextView_Clear(g_hwndTextView);

		g_szFileTitle[0] = '\0';
		g_fFileChanged = FALSE;
		return 0;

	case IDM_FILE_OPEN:

		// get a filename to open
		if (ShowOpenFileDlg(hwnd, g_szFileName, g_szFileTitle))
		{
			DoOpenFile(hwnd, g_szFileName, g_szFileTitle);
		}

		return 0;

	case IDM_FILE_SAVE:
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

	case IDM_FILE_PRINT:

		DeleteDC(
			ShowPrintDlg(hwnd)
			);

		return 0;

	case IDM_FILE_EXIT:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return 0;

	case IDM_EDIT_UNDO:	case WM_UNDO:
		SendMessage(g_hwndTextView, WM_UNDO, 0, 0);
		return 0;

	case IDM_EDIT_REDO:
		SendMessage(g_hwndTextView, TXM_REDO, 0, 0);
		return 0;

	case IDM_EDIT_COPY: case WM_COPY:
		SendMessage(g_hwndTextView, WM_COPY, 0, 0);
		return 0;

	case IDM_EDIT_CUT: case WM_CUT:
		SendMessage(g_hwndTextView, WM_CUT, 0, 0);
		return 0;

	case IDM_EDIT_PASTE: case WM_PASTE:
		SendMessage(g_hwndTextView, WM_PASTE, 0, 0);
		return 0;

	case IDM_EDIT_DELETE: case WM_CLEAR:
		SendMessage(g_hwndTextView, WM_CLEAR, 0, 0);
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
		TextView_SelectAll(g_hwndTextView);
		return 0;

	case IDM_VIEW_OPTIONS:
		ShowOptions(hwnd);
		return 0;

	case IDM_VIEW_LINENUMBERS:
		g_fLineNumbers = !g_fLineNumbers;
		TextView_SetStyleBool(g_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
		return 0;

	case IDM_VIEW_LONGLINES:
		g_fLongLines = !g_fLongLines;
		TextView_SetStyleBool(g_hwndTextView, TXS_LONGLINES, g_fLongLines);
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

	default:
		return 0;
	}
}