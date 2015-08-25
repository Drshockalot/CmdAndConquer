#include <Windows.h>
#include "../Header/CmdAndConquer_MainWindow.h" 
#include "../Header/CmdAndConquer_Globals.h"


LPCTSTR CmdAndConquer_MainWindow::class_name = _T("CmdAndConquer");
ATOM CmdAndConquer_MainWindow::class_atom = 0;

HWND g_hwndTextView;

TCHAR **GetArgvCommandLine(int *argc)
{
#ifdef UNICODE
	return CommandLineToArgvW(GetCommandLineW(), argc);
#else
	*argc = __argc;
	return __argv;
#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	try
	{
		InitTextView();
		CmdAndConquer_MainWindow cAndC(hInstance, cmdShow, _T("CmdAndConquer"));
		g_hwndMain = cAndC.getMainHWND();

		LoadRegSettings();
		

		HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		TCHAR **argv;
		int	argc;

		argv = GetArgvCommandLine(&argc);

		if (argv && argc >= 2)
		{
			//	Open specified file from the command line
			cAndC.OpenUnicodeFile(g_hwndMain, argv[1]);
			LoadFileData(argv[1], g_hwndMain);
		}
		else
		{
			// Automatically create new document when we start
			PostMessage(g_hwndMain, WM_COMMAND, IDM_FILE_NEW, 0);
		}

		ApplyRegSettings();

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
				if (!TranslateAccelerator(cAndC.getMainHWND(), hAccel, &msg))
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

	if (g_fSaveOnExit)
		SaveRegSettings();

	return 0;
}

