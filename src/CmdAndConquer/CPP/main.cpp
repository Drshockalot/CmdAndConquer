#include <Windows.h>
#include "../Header/CmdAndConquer_MainWindow.h" 
#include "../Header/CmdAndConquer_Globals.h"


LPCTSTR CmdAndConquer_MainWindow::class_name = _T("CmdAndConquer");
ATOM CmdAndConquer_MainWindow::class_atom = 0;

HWND CmdAndConquer_MainWindow::hWnd_;
HWND CmdAndConquer_MainWindow::g_hwndTextView;
HWND g_hwndTextView;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	try
	{
		InitTextView();

		CmdAndConquer_MainWindow::registerWindowClass(hInstance);
		CmdAndConquer_MainWindow cAndC(hInstance, cmdShow, _T("CmdAndConquer"));
		g_hwndTextView = CmdAndConquer_MainWindow::getTextHWND();

		TCHAR szFileName[MAX_PATH];
		TCHAR szFileTitle[MAX_PATH];

		HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		for (;;)
		{
			MSG msg;

			BOOL bRet = GetMessage(&msg, 0, 0, 0);
			if (bRet == 0)
			{
				return msg.wParam;
			}
			else if (bRet == -1)
			{
				DWORD err_code = GetLastError();
				tstringstream sstr;
				sstr << _T("System Error: ") << err_code;
				MessageBox(NULL, sstr.str().c_str(), _T("Abnormal termination"), MB_OK);
				return -1;
			}
			else
			{
				if (!TranslateAccelerator(CmdAndConquer_MainWindow::getMainHWND(), hAccel, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
	catch (std::exception & e)
	{
		tstringstream sstr;
		sstr << _T("Unhandled exception: ") << e.what();
		MessageBox(NULL, sstr.str().c_str(), _T("Abnormal termination"), MB_OK);
	}
	catch (...)
	{
		MessageBox(NULL, _T("Unknown unhandled exception thrown"), _T("Abnormal termination"), MB_OK);
	}

	return -1;
}

