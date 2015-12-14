#include <Windows.h>
#include "../Header/CmdAndConquer_MainWindow.h" 
#include "../Header/CmdAndConquer_Globals.h"


LPCTSTR CmdAndConquer_MainWindow::class_name = _T("CmdAndConquer");
ATOM CmdAndConquer_MainWindow::class_atom = 0;

// support 'satellite' resource modules
HINSTANCE	g_hResourceModule;

TCHAR **GetArgvCommandLine(int *argc)
{
#ifdef UNICODE
	return CommandLineToArgvW(GetCommandLineW(), argc);
#else
	*argc = __argc;
	return __argv;
#endif
}

TCHAR * GetArg(TCHAR *ptr, TCHAR *buf, int len)
{
	int i = 0;
	int ch;

	// make sure there's something to parse
	if (ptr == 0 || *ptr == '\0')
	{
		*buf = '\0';
		return 0;
	}

	ch = *ptr++;

	// skip leading whitespace
	while (ch == ' ' || ch == '\t')
		ch = *ptr++;

	// quoted filenames
	if (ch == '\"')
	{
		ch = *ptr++;
		while (i < len - 1 && ch && ch != '\"')
		{
			buf[i++] = ch;
			if (ch = *ptr) *ptr++;
		}
	}
	// grab a token
	else
	{
		while (i < len - 1 && ch && ch != ' ' && ch != '\t')
		{
			buf[i++] = ch;
			if (ch = *ptr) *ptr++;
		}
	}

	buf[i] = '\0';

	// skip trailing whitespace
	while (*ptr == ' ' || *ptr == '\t')
		ptr++;

	return ptr;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	try
	{

		TCHAR **argv;
		int	argc;
		TCHAR arg[MAX_PATH];

		TCHAR *pszCmdline = GetArg(GetCommandLineW(), arg, MAX_PATH);
		argv = GetArgvCommandLine(&argc);

		// check if we have any options
		if (pszCmdline && *pszCmdline == '-')
		{
			pszCmdline = GetArg(pszCmdline, arg, MAX_PATH);

			// do the user-account-control thing
			if (lstrcmpi(arg, _T("-uac")) == 0)
			{
				//return 0;
			}
			// image-file-execute-options
			else if (lstrcmpi(arg, _T("-ifeo")) == 0)
			{
				// skip notepad.exe
				pszCmdline = GetArg(pszCmdline, arg, MAX_PATH);
			}
			// unrecognised option
			else
			{
			}
		}


		// has a prior instance elevated us to admin in order to 
		// modify some system-wide settings in the registry?
		if (argv && argc == 4 && lstrcmpi(argv[1], _T("-uac")) == 0)
		{
			g_fAddToExplorer = _ttoi(argv[2]);
			g_fReplaceNotepad = _ttoi(argv[3]);

			if (SetExplorerContextMenu(g_fAddToExplorer) &&
				SetImageFileExecutionOptions(g_fReplaceNotepad))
			{
				SaveRegSysSettings();
				return 0;
			}
			else
			{
				return ERROR_ACCESS_DENIED;
			}
		}

		g_hResourceModule = hInstance;

		OleInitialize(0);

		InitTextView();
		InitBatchResultWindow();
		CmdAndConquer_MainWindow cAndC(hInstance, cmdShow, _T("CmdAndConquer"));
		g_hwndMain = cAndC.getMainHWND();

		settingsFileName = "CmdAndConquerOptions.xml";

		LoadRegSettings();
		

		HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		// open file specified on commmand line
		if (pszCmdline && *pszCmdline)
		{
			// check to see if it's a quoted filename
			if (*pszCmdline == '\"')
			{
				GetArg(pszCmdline, arg, MAX_PATH);
				pszCmdline = arg;
			}

			NeatpadOpenFile(g_hwndMain, pszCmdline);
			LoadFileData(pszCmdline, g_hwndMain);
		}
		// automatically create new document if none specified
		else
		{
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
					if ((!IsWindow(g_hwndSearchDlg) || !IsDialogMessage(g_hwndSearchDlg, &msg)))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
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

	OleUninitialize();
	ExitProcess(0);
	return 0;
}

