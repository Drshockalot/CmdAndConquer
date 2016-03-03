#ifndef CMDANDCONQUER_MAINWINDOWH
#define CMDANDCONQUER_MAINWINDOWH

#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "../Resource/resource.h"
#include "../../TextView/Header/TextView.h"
#include "../../TextView/Header/TextViewInternal.h"


#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <stdio.h>
#include <strsafe.h>
#include "../../CMDLibrary/Header/CMDCommand.h"

typedef std::basic_stringstream<TCHAR> tstringstream;
typedef std::pair<HWND, HWND> HWNDPAIR;

#define ADDCMDWINDOW_CLASS _T("CmdAndConquer_AddCMDWindow")

class CmdAndConquer_MainWindow
{
public:
	//	Public Data Members
	HWND hWnd_;

	//	Construcors
	CmdAndConquer_MainWindow(HINSTANCE hInstance, int cmdShow, LPCTSTR windowText);
	~CmdAndConquer_MainWindow();
	
	// Public Data Functions
	static ATOM getClassATOM(void);		//	Necessary?
	static LPCTSTR getClassName(void);	//	Necessary?
	HWND getMainHWND();
	
private:
	
	//	Main edit control
	HWND CC_hwndTextView;

	//	Batch results text view
	HWND CC_hwndBatchRunResults;

	HWND CC_hwndAddCMDWindow;
	
	//	Class Registration
	void registerWindowClass(HINSTANCE hInstance);

	//	Private Data Members
	TCHAR *szAppName;
	TCHAR szFileName[MAX_PATH];
	TCHAR szFileTitle[MAX_PATH];
	static ATOM class_atom;
	static LPCTSTR class_name;

	//	Class Window Procedures
	/*
		InitialWndProc = Initialises Window Class
		StatiWndProc = Asserts the correct Window/User data
		WndProc = True Window Procedure
	*/
	LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int onCreate(const HWND hWnd, CREATESTRUCT *cs);
	void setImageList();
	BOOL ShowOpenFileDlg(HWND hwnd, TCHAR * pstrFileName, TCHAR * pstrTitleName);
	
	void ShowAboutDlg(HWND hwndParent);	
	UINT TextViewNotifyHandler(HWND hwnd, NMHDR *nmhdr);
	UINT NotifyHandler(HWND hwnd, NMHDR *nmhdr);
	
	UINT CommandHandler(HWND hwnd, UINT nCtrlId, UINT nCtrlCode, HWND hwndFrom);

	int addCMDWindowScrollPos;
	bool InitAddCMDWindow();
	HWND CreateAddCMDWindow(HWND parentHwnd);
	void ShowAddCMDWindow();
	void CleanCurrentSelectionWindows();
	void CleanUpLastCommandSelection();
	CMDCommand GenerateCommandFromOptions();
	std::wstring getCurrentlySelectedCommand();
	std::string GenerateStringFromCMD(CMDCommand cmd);
	void AddCommandToTextView(CMDCommand cmd);
	CMDOption GetDataFromAddCMDWindowOption(HWNDPAIR, CMDOption op, int opNo);
	static LRESULT CALLBACK addCMDWindowStaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK addCMDWindowWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	std::pair<HWND, HWND> CreateDropDownCMDControl(CMDOption op, RECT& rect, int opNo);
	std::pair<HWND, HWND> CreateTextBoxCMDControl(CMDOption op, RECT& rect, int opNo);
	std::pair<HWND, HWND> CreateCheckBoxCMDControl(CMDOption op, RECT& rect, int opNo);
	void CreateAddCMDWindowCompontentsForCommand(CMDCommand cmd_command);
	void UpdateAddCMDWindowComponentsForSelectedCommand();
	void ModifyDropDownCMDOp(CMDOption& op, int opNo);
	void ModifyTextBoxCMDOp(CMDOption& op, int opNo);
	void ModifyCheckBoxCMDOp(CMDOption& op, int opNo);
	int isOpCommand(int ref);
	LRESULT handleOpListCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool isOpIncluded(HWND hwnd, int opType);

	struct
	{
		HWND CommandList;
		HWND SubmitButton;
		std::vector<std::pair<HWND, HWND>> OptionList;
		std::vector<int> opKeyList;
	} AddCMDWindowComponents;
	enum
	{
		string,
		wstring
	};
	std::string convertWStringToString(std::wstring);
	std::wstring convertStringToWString(std::string);
	//	Toolbar Initialisation
	HWND initToolbar(HWND hWnd, CREATESTRUCT *cs);
};

#endif


