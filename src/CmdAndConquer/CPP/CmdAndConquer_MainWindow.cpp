#include "../Header/CmdAndConquer_MainWindow.h"
#include "../Header/CmdAndConquer_Globals.h"
HDC ShowPrintDlg(HWND hwndParent);

TCHAR		g_szAppName[] = APP_TITLE;
HWND		g_hwndMain;

TCHAR g_szFileName[MAX_PATH];
TCHAR g_szFileTitle[MAX_PATH];

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
	TextView_SetLineImage(CC_hwndTextView, 16, 1);
	TextView_SetLineImage(CC_hwndTextView, 5, 1);
	TextView_SetLineImage(CC_hwndTextView, 36, 1);
	TextView_SetLineImage(CC_hwndTextView, 11, 1);
}

BOOL CmdAndConquer_MainWindow::DoOpenFile(HWND hWnd, TCHAR *szFileName, TCHAR *szFileTitle)
{
	int fmt, fmtlook[] =
	{
		IDM_VIEW_ASCII, IDM_VIEW_UTF8, IDM_VIEW_UTF16, IDM_VIEW_UTF16BE
	};

	if (TextView_OpenFile(this->CC_hwndTextView, szFileName))
	{
		SetWindowFileName(hWnd, szFileTitle);

		fmt = TextView_GetFormat(g_hwndTextView);

		CheckMenuRadioItem(GetMenu(hWnd), IDM_VIEW_ASCII, IDM_VIEW_UTF16BE, fmtlook[fmt], MF_BYCOMMAND);

		return TRUE;
	}
	else
	{
		MessageBox(hWnd, _T("Error opening file"), APP_TITLE, MB_ICONEXCLAMATION);
		return FALSE;
	}
}

void CmdAndConquer_MainWindow::OpenUnicodeFile(HWND hWnd, TCHAR *szFile)
{
	TCHAR *name;

	//	save current file's position
	SaveFileData(g_szFileName, hWnd);

	_tcscpy_s(g_szFileName, szFile);

	name = _tcsrchr(g_szFileName, '\\');
	_tcscpy_s(g_szFileTitle, name ? name + 1 : szFile);

	DoOpenFile(hWnd, g_szFileName, g_szFileTitle);
}

void CmdAndConquer_MainWindow::HandleDropFiles(HWND hWnd, HDROP hDrop)
{
	TCHAR buf[MAX_PATH];

	if (DragQueryFile(hDrop, 0, buf, MAX_PATH))
	{
		OpenUnicodeFile(hWnd, buf);
	}

	DragFinish(hDrop);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static int width, height;

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
			this->HandleDropFiles(hWnd, (HDROP)wParam);
			return 0;

		case WM_DESTROY:
			SaveFileData(g_szFileName, hWnd);
			PostQuitMessage(0);
			DeleteObject(g_hFont);
			return 0;

		case WM_INITMENU:
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_LINENUMBERS, g_fLineNumbers);
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_LONGLINES, g_fLongLines);
			CheckMenuCommand((HMENU)wParam, IDM_VIEW_SAVEEXIT, g_fSaveOnExit);
			return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDM_FILE_NEW:
					this->SetWindowFileName(hWnd, _T("Untitled"));
					TextView_Clear(this->CC_hwndTextView);
					return 0;

				case IDM_FILE_OPEN:
					if (this->ShowOpenFileDlg(hWnd, szFileName, szFileTitle))
						this->DoOpenFile(hWnd, szFileName, szFileTitle);
					return 0;

				case IDM_FILE_PRINT:
					DeleteDC(ShowPrintDlg(hWnd));
					return 0;
				
				case IDM_VIEW_FONT:
					ShowProperties(hWnd);
					return 0;

				case IDM_VIEW_LINENUMBERS:
					g_fLineNumbers = !g_fLineNumbers;
					TextView_SetStyleBool(this->CC_hwndTextView, TXS_LINENUMBERS, g_fLineNumbers);
					return 0;

				case IDM_VIEW_LONGLINES:
					g_fLongLines = !g_fLongLines;
					TextView_SetStyleBool(this->CC_hwndTextView, TXS_LONGLINES, g_fLongLines);
					return 0;

				case IDM_VIEW_SAVEEXIT:
					g_fSaveOnExit = !g_fSaveOnExit;
					return 0;

				case IDM_VIEW_SAVENOW:
					SaveRegSettings();
					return 0;
				
				case IDM_HELP_ABOUT:
					this->ShowAboutDlg(hWnd);
					return 0;
			}
			return 0;
		}

		case WM_SETFOCUS:
			SetFocus(this->CC_hwndTextView);
			return 0;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_SIZE:
			width = (short)LOWORD(lParam);
			height = (short)HIWORD(lParam);

			MoveWindow(this->CC_hwndTextView, 0, 0 /*29 for toolbar*/, width, height /*- 30 for toolbar*/, TRUE);
			return 0;
		
		default:
			return DefWindowProc(hWnd_, Msg, wParam, lParam);
	}

	return 0;
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

void CmdAndConquer_MainWindow::SetWindowFileName(HWND hwnd, TCHAR *szFileName)
{
	TCHAR ach[MAX_PATH + sizeof(szAppName) + 4];

	wsprintf(ach, _T("%s - %s"), szFileName, szAppName);
	SetWindowText(hwnd, ach);
}

void CmdAndConquer_MainWindow::ShowAboutDlg(HWND hwndParent)
{
	MessageBox(hwndParent,
		APP_TITLE _T("\r\n\r\n")  WEBSITE_STR,
		APP_TITLE,
		MB_OK | MB_ICONINFORMATION
		);
}