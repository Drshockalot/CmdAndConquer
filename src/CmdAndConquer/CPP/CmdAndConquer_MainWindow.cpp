#include "../Header/CmdAndConquer_MainWindow.h"

DWORD g_BytesTransferred = 0;

CmdAndConquer_MainWindow::CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText) : szAppName(APP_TITLE)
{
	hWnd_ = NULL;
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

	WNDCLASS wc = {
		CS_HREDRAW | CS_VREDRAW,
		&CmdAndConquer_MainWindow::InitialWndProc,
		0,
		sizeof(CmdAndConquer_MainWindow *),
		hInstance,
		NULL,
		NULL,
		reinterpret_cast<HBRUSH>(COLOR_BACKGROUND),
		NULL,
		CmdAndConquer_MainWindow::class_name
	};
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	class_atom = RegisterClass(&wc);
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

BOOL CmdAndConquer_MainWindow::DoOpenFile(HWND hWnd, TCHAR *szFileName, TCHAR *szFileTitle)
{
	if (TextView_OpenFile(hwndTextView, szFileName))
	{
		SetWindowFileName(hWnd, szFileTitle);
		return TRUE;
	}
	else
	{
		MessageBox(hWnd, _T("Error opening file"), APP_TITLE, MB_ICONEXCLAMATION);
		return FALSE;
	}
}

void CmdAndConquer_MainWindow::HandleDropFiles(HWND hWnd, HDROP hDrop)
{
	TCHAR buf[MAX_PATH];
	TCHAR *name;

	if (DragQueryFile(hDrop, 0, buf, MAX_PATH))
	{
		strcpy_s(szFileName, buf);

		name = strrchr(szFileName, '\\');
		strcpy_s(szFileTitle, name ? name + 1 : buf);

		DoOpenFile(hWnd, szFileName, szFileTitle);
	}

	DragFinish(hDrop);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::actualWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	
	static int width, height;
	HFONT hFont;

	switch (Msg)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			void * lpCreateParam = create_struct->lpCreateParams;
			CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(lpCreateParam);
			assert(this_window == this);

			hFont = EasyCreateFont(10, FALSE, "Courier New");
			SendMessage(hwndTextView, WM_SETFONT, (WPARAM)hFont, 0);

			//hFont = EasyCreateFont(10, FALSE, "Lucida Console");
			//hFont = EasyCreateFont(16, FALSE, "Arial");
			//TextView_AddFont(hwndTextView, hFont);

			PostMessage(hWnd, WM_COMMAND, ID_FILE_NEW, 0);

			DragAcceptFiles(hWnd, TRUE);

			onCreate(hWnd_, reinterpret_cast<CREATESTRUCT*>(lParam));
			return 0;
		}

		case WM_DROPFILES:
			HandleDropFiles(hWnd, (HDROP)wParam);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_FILE_NEW:
					SetWindowFileName(hWnd, _T("Untitled"));
					TextView_Clear(hwndTextView);
					return 0;
				case ID_FILE_OPEN:
					if (ShowOpenFileDlg(hWnd, szFileName, szFileTitle))
					{
						DoOpenFile(hWnd, szFileName, szFileTitle);
					}
					return 0;
				case ID_HELP_ABOUTUS:
					ShowAboutDlg(hWnd);
					return 0;
			}
			return 0;
		}

		case WM_SETFOCUS:
			SetFocus(hwndTextView);
			return 0;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_SIZE:
			width = (short)LOWORD(lParam);
			height = (short)HIWORD(lParam);

			//need 
			MoveWindow(hwndTextView, 15, 29, width - 15, height - 30, TRUE);
			MoveWindow(hwndLineNo, 0, 29, 15, height - 30, TRUE);
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
		return this_window->actualWndProc(hWnd, Msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(user_data);
	assert(this_window);
	assert(hWnd == this_window->hWnd_);
	return this_window->actualWndProc(hWnd, Msg, wParam, lParam);
}

int CmdAndConquer_MainWindow::onCreate(HWND hWnd, CREATESTRUCT *cs)
{	
	int width, height;
	RECT wndRECT;
	if (GetClientRect(hWnd, &wndRECT))
	{
		width = wndRECT.right - wndRECT.left;
		height = wndRECT.bottom - wndRECT.top;
	}

	RECT lineNorc = { 0, 29, 15, height };
	RECT mainBoxrc = { 15, 29, width - 15, height };
	
	hwndLineNo = CreateTextView(hWnd_, cs->hInstance, lineNorc);
	hwndTextView = CreateTextView(hWnd_, cs->hInstance, mainBoxrc);

	initToolbar(hWnd, cs);

	return 0;
}

void CmdAndConquer_MainWindow::initToolbar(HWND hWnd, CREATESTRUCT *cs)
{
	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&InitCtrlEx);

	TBBUTTON tbrButtons[7];

	tbrButtons[0].iBitmap = 0;
	tbrButtons[0].idCommand = ID_FILE_NEW;
	tbrButtons[0].fsState = TBSTATE_ENABLED;
	tbrButtons[0].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[0].dwData = 0L;
	tbrButtons[0].iBitmap = 0;
	tbrButtons[0].iString = 0;

	tbrButtons[1].iBitmap = 1;
	tbrButtons[1].idCommand = ID_FILE_OPEN;
	tbrButtons[1].fsState = TBSTATE_ENABLED;
	tbrButtons[1].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[1].dwData = 0L;
	tbrButtons[1].iString = 0;

	tbrButtons[2].iBitmap = 0;
	tbrButtons[2].idCommand = ID_FILE_EXIT;
	tbrButtons[2].fsState = TBSTATE_ENABLED;
	tbrButtons[2].fsStyle = TBSTYLE_SEP;
	tbrButtons[2].dwData = 0L;
	tbrButtons[2].iString = 0;

	tbrButtons[3].iBitmap = 2;
	tbrButtons[3].idCommand = ID_FILE_OPEN;
	tbrButtons[3].fsState = TBSTATE_ENABLED;
	tbrButtons[3].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[3].dwData = 0L;
	tbrButtons[3].iString = 0;

	tbrButtons[4].iBitmap = 3;
	tbrButtons[4].idCommand = ID_FILE_OPEN;
	tbrButtons[4].fsState = TBSTATE_ENABLED;
	tbrButtons[4].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[4].dwData = 0L;
	tbrButtons[4].iString = 0;

	tbrButtons[5].iBitmap = 4;
	tbrButtons[5].idCommand = ID_FILE_OPEN;
	tbrButtons[5].fsState = TBSTATE_ENABLED;
	tbrButtons[5].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[5].dwData = 0L;
	tbrButtons[5].iString = 0;

	tbrButtons[6].iBitmap = 5;
	tbrButtons[6].idCommand = ID_FILE_OPEN;
	tbrButtons[6].fsState = TBSTATE_ENABLED;
	tbrButtons[6].fsStyle = TBSTYLE_BUTTON;
	tbrButtons[6].dwData = 0L;
	tbrButtons[6].iString = 0;

	HWND hWndToolbar;
	hWndToolbar = CreateToolbarEx(hWnd,
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		IDR_TOOLBAR3,
		7,
		cs->hInstance,
		IDR_TOOLBAR3,
		tbrButtons,
		7,
		16, 16, 16, 16,
		sizeof(TBBUTTON));

	UpdateWindow(hWnd);
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

int CmdAndConquer_MainWindow::PointsToLogical(int nPointSize)
{
	HDC hdc = GetDC(0);
	int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(0, hdc);

	return nLogSize;
}

HFONT CmdAndConquer_MainWindow::EasyCreateFont(int nPointSize, BOOL fBold, TCHAR *szFace)
{
	return CreateFont(PointsToLogical(nPointSize),
						0, 0, 0,
						fBold ? FW_BOLD : 0,
						0, 0, 0, 0, 0, 0, 0, 0,
						szFace);
}