#include "../Header/CmdAndConquer_MainWindow.h"

DWORD g_BytesTransferred = 0;

CmdAndConquer_MainWindow::CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText, HMENU hMenu) : hWnd_(NULL)
{
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
							 hMenu,
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

LRESULT CALLBACK CmdAndConquer_MainWindow::actualWndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	
	switch (Msg)
	{
		case WM_COMMAND:
		{
			DWORD wmId = LOWORD(wParam);
			switch (wmId)
			{
				case ID_FILE_NEW:
					openFile();
					break;
			}
			break;

		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
		{
			LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			void * lpCreateParam = create_struct->lpCreateParams;
			CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(lpCreateParam);
			assert(this_window == this);

			onCreate(hWnd_, reinterpret_cast<CREATESTRUCT*>(lParam));

			return DefWindowProc(hWnd_, Msg, wParam, lParam);
			break;
		}
		default:
			return DefWindowProc(hWnd_, Msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CmdAndConquer_MainWindow::editWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_KEYDOWN:
		MessageBox(NULL, _T("help"), _T("help"), 0);
		break;
	default:
		return CallWindowProc(oldEditProc, hWnd, Msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CmdAndConquer_MainWindow::InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg = WM_NCCREATE)
	{
		LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		void * lpCreateParam = create_struct->lpCreateParams;
		CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(lpCreateParam);
		assert(this_window->hWnd_ == 0);

		this_window->hWnd_ = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CmdAndConquer_MainWindow::StaticWndProc));
		return this_window->actualWndProc(Msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK CmdAndConquer_MainWindow::StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(user_data);
	assert(this_window);
	assert(hWnd == this_window->hWnd_);
	return this_window->actualWndProc(Msg, wParam, lParam);
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
	
	lineNoHwnd = createEditBox(hWnd_, cs->hInstance, ES_MULTILINE, lineNorc, IDCE_MULTILINE, _T("1\n2\n3"));
	mainEditHwnd = createEditBox(hWnd_, cs->hInstance, ES_MULTILINE | WS_VSCROLL, mainBoxrc, IDCE_MULTILINE, _T("Main box"));

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

	return 0;
}

HWND CmdAndConquer_MainWindow::createEditBox(HWND hParent, HINSTANCE hInst, DWORD dwStyle, const RECT& rc, const int id, const ustring& caption)
{   
	dwStyle|= WS_CHILD | WS_VISIBLE;

	HWND hwnd = CreateWindowEx(WS_EX_WINDOWEDGE,
		_T("edit"),
		caption.c_str(),
		dwStyle,
		rc.left,
		rc.top,
		rc.right,
		rc.bottom,
		hParent,
		reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
		hInst,
		0);

	oldEditProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)editWndProc);

	return hwnd;
}

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
}

void DisplayError(LPTSTR lpszFunction)
// Routine Description:
// Retrieve and output the system error message for the last-error code
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	lpDisplayBuf =
		(LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf)
		+ lstrlen((LPCTSTR)lpszFunction)
		+ 40) // account for format string
		* sizeof(TCHAR));

	if (FAILED(StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error code %d as follows:\n%s"),
		lpszFunction,
		dw,
		lpMsgBuf)))
	{
		printf("FATAL ERROR: Unable to output error code.\n");
	}

	_tprintf(TEXT("ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}


void CmdAndConquer_MainWindow::openFile()
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "C:\\Windows";
	size_t size = strlen(szFileName) + 1;
	wchar_t* wtext = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, wtext, size, szFileName, strlen(szFileName - 1));
	LPWSTR ptr = wtext;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd_;
	ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
	ofn.lpstrFile = ptr;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("txt");

	if (GetOpenFileName(&ofn))
	{

	}
}