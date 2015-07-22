#include "../Header/CmdAndConquer_MainWindow.h"

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

	RECT lineNorc = { 0, 0, 15, height };
	RECT mainBoxrc = { 15, 0, width - 15, height };
	
	createEditBox(hWnd_, cs->hInstance, ES_MULTILINE, lineNorc, IDCE_MULTILINE, _T("1\n2\n3"));
	createEditBox(hWnd_, cs->hInstance, ES_MULTILINE | WS_VSCROLL, mainBoxrc, IDCE_MULTILINE, _T("Main box"));

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