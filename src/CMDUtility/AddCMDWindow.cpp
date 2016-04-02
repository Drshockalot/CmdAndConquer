#include "../CmdAndConquer/Header/CmdAndConquer_Globals.h"
#include "../CmdAndConquer/Header/CmdAndConquer_MainWindow.h"
#include "../CMDLibrary/Header/CMDCommandList.h"
#include "../TextView/Header/TextViewInternal.h"
#include "../TextView/Header/TextView.h"
#include <codecvt>

BOOL g_fShowAddCMDWindow;
HWND g_hwndAddCMDWindow;

void CmdAndConquer_MainWindow::UpdateAddCMDWindowComponentsForSelectedCommand()
{
	int sel = SendMessage(AddCMDWindowComponents.CommandList, CB_GETCURSEL, 0, 0);

	CMDCommandList* cmdList = CMDCommandList::instance();
	CMDCommand currentCMD = cmdList->at(sel);

	CreateAddCMDWindowCompontentsForCommand(currentCMD);
}

void CmdAndConquer_MainWindow::ModifyDropDownCMDOp(CMDOption& op, int opNo)
{
	int sel = SendMessage(AddCMDWindowComponents.OptionList[opNo].second, CB_GETCURSEL, 0, 0);
	TCHAR buf[128];
	SendMessage(AddCMDWindowComponents.OptionList[opNo].second, CB_GETLBTEXT, sel, (LPARAM)buf);
	std::wstring wBuf = buf;
	std::string aBuf = convertWStringToString(wBuf);
	op.AddUserDropDownVal(aBuf);
}

void CmdAndConquer_MainWindow::ModifyTextBoxCMDOp(CMDOption& op, int opNo)
{
	TCHAR buf[1024];
	GetWindowText(AddCMDWindowComponents.OptionList[opNo].second, buf, 1024);
	std::wstring wBuf = buf;
	std::string aBuf = wBuf != _T("") ? convertWStringToString(wBuf) : "";
	op.AddUserTextBoxVal(aBuf);
}

void CmdAndConquer_MainWindow::ModifyCheckBoxCMDOp(CMDOption& op, int opNo)
{
	auto boxState = SendMessage(AddCMDWindowComponents.OptionList[opNo].second, BM_GETCHECK, 0, 0);
	op.AddUserCheckBoxVal((bool)boxState);
}

void CmdAndConquer_MainWindow::CreateAddCMDWindowCompontentsForCommand(CMDCommand currentCMD)
{
	auto opList = currentCMD.getAvailableOptions();
	int opNo = 0;
	//starting point for controls
	RECT rect = {10, 25, 0, 0};

	for(auto op : opList)
	{
		auto opType = op.getType();

		switch(opType)
		{
		case DropDown:
			AddCMDWindowComponents.OptionList.push_back(CreateDropDownCMDControl(op, rect, opNo));
			break;
		case TextBox:
			AddCMDWindowComponents.OptionList.push_back(CreateTextBoxCMDControl(op, rect, opNo));
			break;
		case CheckBox:
			AddCMDWindowComponents.OptionList.push_back(CreateCheckBoxCMDControl(op, rect, opNo));
			break;
		}
		opNo++;
	}

	if (opNo == 0)
		rect.top += 20;

	AddCMDWindowComponents.SubmitButton = CreateWindow(WC_BUTTON, _T("OK"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		rect.left, rect.top, 50, 25,
		g_hwndAddCMDWindow,
		(HMENU)ADD_CMD_WINDOW_SUBMIT_KEY,
		GetModuleHandle(0),
		0);

	rect.left += 60;

	AddCMDWindowComponents.CancelButton = CreateWindow(WC_BUTTON, _T("CANCEL"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		rect.left, rect.top, 60, 25,
		g_hwndAddCMDWindow,
		(HMENU)ADD_CMD_WINDOW_CANCEL_KEY,
		GetModuleHandle(0),
		0);

	AddCMDWindowComponents.ExitButton = CreateWindow(WC_BUTTON, _T("X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		260, 10, 25, 25,
		g_hwndAddCMDWindow,
		(HMENU)ADD_CMD_WINDOW_EXIT_KEY,
		GetModuleHandle(0),
		0);
}

std::pair<HWND, HWND> CmdAndConquer_MainWindow::CreateDropDownCMDControl(CMDOption op, RECT& rect, int opNo)
{
	std::vector<std::string> opList = op.extractDropDownList();
	
	int listWidth = 150;
	int listHeight = 210;
	int labelWidth = 100;
	int labelHeight = 20;

	if(opNo == 0)
		rect.top += 20;

	HWND label = CreateWindow(WC_STATIC, _T(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		rect.left, 
		rect.top, 
		labelWidth, labelHeight,
		g_hwndAddCMDWindow, 
		(HMENU)ADD_CMD_WINDOW_OPLIST_LABELS_KEY + opNo,
		GetModuleHandle(0), 
		NULL);

	SetWindowText(label, op.getLabelW().c_str());

	rect.left += (labelWidth + 10);
	//rect.top += (labelHeight + 10);

	HWND list = CreateWindow(WC_COMBOBOX, _T(""),
	CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
	rect.left, 
	rect.top, 
	listWidth, listHeight,
	g_hwndAddCMDWindow,
	(HMENU)(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo),
	GetModuleHandle(0),
	0);

	AddCMDWindowComponents.opKeyList.push_back(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo);

	rect.left = 10;
	rect.top += (20 + 10); //20 for the normal text box height, the height for the combo box is for the list size, not the box size

	for (int i = 0; i < opList.size(); ++i)
	{
		std::string name = opList[i];
		std::wstring w_op;
		int len = MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.length(), NULL, 0);
		if (len > 0)
		{
			w_op.resize(len);
			MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.length(), &w_op[0], len);
		}
		SendMessage(list, CB_ADDSTRING, 0, (LPARAM)w_op.c_str());
	}

	return std::pair<HWND, HWND>(label, list);
}

std::pair<HWND, HWND> CmdAndConquer_MainWindow::CreateTextBoxCMDControl(CMDOption op, RECT& rect, int opNo)
{
	int boxWidth = 150;
	int boxHeight = 25;
	int labelWidth = 100;
	int labelHeight = 20;

	if (opNo == 0)
		rect.top += 20;

	HWND label = CreateWindow(WC_STATIC, _T(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		rect.left,
		rect.top,
		labelWidth, labelHeight,
		g_hwndAddCMDWindow,
		(HMENU)ADD_CMD_WINDOW_OPLIST_LABELS_KEY + opNo,
		GetModuleHandle(0),
		NULL);

	SetWindowText(label, op.getLabelW().c_str());
	rect.left += (labelWidth + 10);

	HWND textBox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, _T(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
		rect.left,
		rect.top,
		boxWidth, boxHeight,
		g_hwndAddCMDWindow,
		(HMENU)(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo),
		GetModuleHandle(0),
		NULL);

	AddCMDWindowComponents.opKeyList.push_back(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo);

	rect.left = 10;
	rect.top += (boxHeight + 10);

	return std::pair<HWND ,HWND>(label, textBox);
}

std::pair<HWND, HWND> CmdAndConquer_MainWindow::CreateCheckBoxCMDControl(CMDOption op, RECT& rect, int opNo)
{
	int boxWidth = 25;
	int boxHeight = 25;
	int labelWidth = 100;
	int labelHeight = 20;

	if (opNo == 0)
		rect.top += 20;

	HWND label = CreateWindow(WC_STATIC, _T(""),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		rect.left,
		rect.top,
		labelWidth, labelHeight,
		g_hwndAddCMDWindow,
		(HMENU)ADD_CMD_WINDOW_OPLIST_LABELS_KEY + opNo,
		GetModuleHandle(0),
		NULL);

	SetWindowText(label, op.getLabelW().c_str());
	rect.left += (labelWidth + 10);

	HWND checkBox = CreateWindow(WC_BUTTON, _T(""),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		rect.left,
		rect.top,
		boxWidth, boxHeight,
		g_hwndAddCMDWindow,
		(HMENU)(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo),
		GetModuleHandle(0),
		NULL);

	AddCMDWindowComponents.opKeyList.push_back(ADD_CMD_WINDOW_OPLIST_CHILD_KEY + opNo);

	rect.left = 10;
	rect.top += (boxHeight + 10);

	return std::pair<HWND, HWND>(label, checkBox);
}

CMDCommand CmdAndConquer_MainWindow::GenerateCommandFromOptions()
{
	std::wstring cmdName = getCurrentlySelectedCommand();
	CMDCommandList* cmdList = CMDCommandList::instance();
	int sel = SendMessage(AddCMDWindowComponents.CommandList, CB_GETCURSEL, 0, 0);
	auto op = cmdList->at(sel);
	auto opList = op.getAvailableOptions();
	std::vector<CMDOption> userOpList;
	int opNo = 0;

	int opListSize = opList.size();

	for (int i = 0; i < opListSize; i++)
	{
		auto hwnd = AddCMDWindowComponents.OptionList[i];
		CMDOption currOp = opList.at(opNo);
		if(currOp.isRequired() || isOpIncluded(hwnd.second, currOp.getType()))
		{
			if(currOp.isRequired() && !isOpIncluded(hwnd.second, currOp.getType()))
			{
				//do some error here, tell the user to include required fields
			}
			CMDOption opToAdd = GetDataFromAddCMDWindowOption(hwnd, opList.at(opNo), opNo);
			userOpList.push_back(opToAdd);
		}
		opNo++;
	}
	//for(auto hwnd : AddCMDWindowComponents.OptionList)
	//{
	//	CMDOption opToAdd = GetDataFromAddCMDWindowOption(hwnd, opList.at(opNo), opNo);
	//	userOpList.push_back(opToAdd);
	//	opNo++;
	//}
	CMDCommand retCMD(convertWStringToString(cmdName));
	for(auto opForUserOpList : userOpList)
	{
		retCMD.addArgument(opForUserOpList);
	}
	return retCMD;
}

bool CmdAndConquer_MainWindow::isOpIncluded(HWND hwnd, int opType)
{
	switch (opType)
	{
		case DropDown:
		{
			TCHAR buf[128];
			GetWindowText(hwnd, buf, 128);
			if (lstrlen(buf) > 0)
				return true;
			return false;
		}
		case TextBox:
		{
			TCHAR buf[128];
			GetWindowText(hwnd, buf, 128);
			if (lstrlen(buf) > 0)
				return true;
			return false;
		}
		case CheckBox:
		{
			return SendMessage(hwnd, BM_GETCHECK, 0, 0);
		}
		default:
			return false;
	}
}

std::string CmdAndConquer_MainWindow::GenerateStringFromCMD(CMDCommand cmd)
{
	std::string ret;

	ret += cmd.getName() + " ";

	auto activeOps = cmd.getActiveArguments();

	for(auto op : activeOps)
	{
		ret += op.getCommand() + " ";
	}

	return ret;
}

void CmdAndConquer_MainWindow::AddCommandToTextView(CMDCommand cmd)
{
	std::string cmdString = GenerateStringFromCMD(cmd);
	std::wstring wCMDString = convertStringToWString(cmdString);
	TextView_AddCMD(g_hwndTextView, wCMDString.c_str());
	//g_fShowAddCMDWindow = false;
}

ULONG TextView::addCMD(TCHAR* cmdText)
{
	if(cmdText != nullptr)
	{
		ULONG len = lstrlen(cmdText);
		EnterText(cmdText, len);
		return 0;
	}
	return 1;
}

CMDOption CmdAndConquer_MainWindow::GetDataFromAddCMDWindowOption(HWNDPAIR p, CMDOption op, int opNo)
{
	auto opType = op.getType();
	switch(opType)
	{
	case DropDown:
		ModifyDropDownCMDOp(op, opNo);
		break;
	case TextBox:
		ModifyTextBoxCMDOp(op, opNo);
		break;
	case CheckBox:
		ModifyCheckBoxCMDOp(op, opNo);
		break;
	}

	return op;
}

std::wstring CmdAndConquer_MainWindow::getCurrentlySelectedCommand()
{
	std::wstring cmdName;

	int sel = SendMessage(AddCMDWindowComponents.CommandList, CB_GETCURSEL, 0, 0);
	TCHAR buf[128];

	int len = SendMessage(AddCMDWindowComponents.CommandList, CB_GETLBTEXT, sel, (LPARAM)buf);
	cmdName = buf;
	return cmdName;
}

std::string CmdAndConquer_MainWindow::convertWStringToString(std::wstring wStr)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(wStr);
}

std::wstring CmdAndConquer_MainWindow::convertStringToWString(std::string str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}


LRESULT WINAPI CmdAndConquer_MainWindow::addCMDWindowStaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(g_hwndMain, GWLP_USERDATA);
	CmdAndConquer_MainWindow * this_window = reinterpret_cast<CmdAndConquer_MainWindow *>(user_data);
	assert(this_window);
	//assert(hwnd == this_window->hWnd_);
	return this_window->addCMDWindowWndProc(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI CmdAndConquer_MainWindow::addCMDWindowWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT winRect;
	GetClientRect(g_hwndAddCMDWindow, &winRect);

	switch (msg)
	{
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (INT_PTR)GetStockObject(NULL);
		}
		case WM_VSCROLL:
		{
			int nPos;
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
			GetScrollInfo(hwnd, SB_VERT, &si);

			switch (LOWORD(wParam)) {
			case SB_TOP:            nPos = si.nMin; break;
			case SB_BOTTOM:         nPos = si.nMax; break;
			case SB_LINEUP:         nPos = si.nPos - 1; break;
			case SB_LINEDOWN:       nPos = si.nPos + 1; break;
			//case SB_PAGEUP:         nPos = si.nPos - CustomLogicalPage(si.nPage); break;
			//case SB_PAGEDOWN:       nPos = si.nPos + CustomLogicalPage(si.nPage); break;
			case SB_THUMBTRACK:     nPos = si.nTrackPos; break;
			default:
			case SB_THUMBPOSITION:  nPos = si.nPos; break;
			}

			int nOldPos;

			nOldPos = GetScrollPos(hwnd, SB_VERT);

			// Setup and refresh the toolbar:
			SetScrollPos(hwnd, SB_VERT, nPos, TRUE);

			// Ensure the new pos is in the allowed range (between nin and nMax):
			nPos = GetScrollPos(hwnd, SB_VERT);

			// Refresh client area:
			UpdateWindow(hwnd);
			//InvalidateRect(hwnd, NULL, true);
			ScrollWindowEx(hwnd, 0, (nOldPos - nPos) * 10,
				NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE);
			InvalidateRect(hwnd, &winRect, true);
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ADD_CMD_WINDOW_COMMANDLIST_KEY:
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							int sel = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
							TCHAR buf[128];

							int len = SendMessage(hwnd, CB_GETLBTEXT, sel, (LPARAM)buf);
							std::wstring cmdName = buf;

							CleanCurrentSelectionWindows();
							CleanUpLastCommandSelection();
							UpdateAddCMDWindowComponentsForSelectedCommand();
							return 0;
						}
						default:
							return DefWindowProc(hwnd, msg, wParam, lParam);
					}
				}
					
				case ADD_CMD_WINDOW_SUBMIT_KEY:
				{
					switch (HIWORD(wParam))
					{
						case BN_CLICKED:
						{
							CMDCommand cmd = GenerateCommandFromOptions();
							AddCommandToTextView(cmd);
							CleanCurrentSelectionWindows();
							CleanUpLastCommandSelection();
							SetWindowText(AddCMDWindowComponents.CommandList, _T(""));
							HideAddCMDWindow();
							return 0;
						}
					}
				}
				case ADD_CMD_WINDOW_EXIT_KEY:
				case ADD_CMD_WINDOW_CANCEL_KEY:
				{
					switch(HIWORD(wParam))
					{
						case BN_CLICKED:
						{
							CleanCurrentSelectionWindows();
							CleanUpLastCommandSelection();
							SetWindowText(AddCMDWindowComponents.CommandList, _T(""));
							HideAddCMDWindow();
							return 0;
						}
					}
				}
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		
		case WM_SIZE:
		{
			SCROLLINFO si;

			// We need to update scrolling page to correspond to the current control height.
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE;
			si.nPage = (HIWORD(lParam) / 10);
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		}
		return 0;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

int CmdAndConquer_MainWindow::isOpCommand(int ref)
{
	for (auto i = AddCMDWindowComponents.opKeyList.begin(); i != AddCMDWindowComponents.opKeyList.end(); ++i)
	{
		if (ref == *i)
			return *i;
	}
	return 0;
}


LRESULT CmdAndConquer_MainWindow::handleOpListCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int opNo = LOWORD(wParam) - ADD_CMD_WINDOW_OPLIST_CHILD_KEY;
	int actNo = HIWORD(wParam);
	int sel = SendMessage(AddCMDWindowComponents.CommandList, CB_GETCURSEL, 0, 0);
	CMDCommandList *list = CMDCommandList::instance();
	CMDCommand currCMD = list->at(sel);
	CMDOption currOp = currCMD.getAvailableOptions().at(opNo - 1);

	HWNDPAIR currPair = AddCMDWindowComponents.OptionList.at(opNo - 1);
	int typeNo = currOp.getType();

	switch(typeNo)
	{
		case DropDown:
		{

			return 0;
		}
		case TextBox:
		{

			return 0;
		}
		case CheckBox:
		{
			switch(HIWORD(actNo))
			{
				case BN_CLICKED:
				{
					/*bool checked = IsDlgButtonChecked(hwnd, 1);
					if (checked)
						CheckDlgButton(hwnd, 1, BST_UNCHECKED);
					else
						CheckDlgButton(hwnd, 1, BST_CHECKED);
					return 0;*/
					CheckDlgButton(hwnd, 1, BST_CHECKED);
				}
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
}

bool CmdAndConquer_MainWindow::InitAddCMDWindow()
{
	WNDCLASSEX wcx = {};

	//Window class for the batch script result window
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_DBLCLKS;
	wcx.lpfnWndProc = addCMDWindowStaticWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = GetModuleHandle(0);
	wcx.hIcon = 0;
	wcx.hCursor = LoadCursor(NULL, IDC_IBEAM);
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);		//NO FLICKERING FOR US!!
	wcx.lpszMenuName = 0;
	wcx.lpszClassName = ADDCMDWINDOW_CLASS;
	wcx.hIconSm = 0;

	return RegisterClassEx(&wcx) ? TRUE : FALSE;
}

HWND CmdAndConquer_MainWindow::CreateAddCMDWindow(HWND parentHwnd)
{
	HWND ret = CreateWindowEx(WS_EX_CLIENTEDGE,
		ADDCMDWINDOW_CLASS, _T(""),
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,
		0, 0, 0, 0,
		parentHwnd,
		0,
		GetModuleHandle(0),
		0);

	AddCMDWindowComponents.CommandList = CreateWindow(WC_COMBOBOX, _T(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
		10, 10, 150, 210,
		ret,
		(HMENU)ADD_CMD_WINDOW_COMMANDLIST_KEY,
		GetModuleHandle(0),
		0);

	AddCMDWindowComponents.ExitButton = CreateWindow(WC_BUTTON, _T("X"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		260, 10, 25, 25,
		ret,
		(HMENU)ADD_CMD_WINDOW_EXIT_KEY,
		GetModuleHandle(0),
		0);

	CMDCommandList* cmdList = CMDCommandList::instance();

	for (int i = 0; i < cmdList->size(); ++i)
	{
		std::string name = cmdList->at(i).getName();
		std::wstring w_filename;
		int len = MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.length(), NULL, 0);
		if (len > 0)
		{
			w_filename.resize(len);
			MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.length(), &w_filename[0], len);
		}
		SendMessage(AddCMDWindowComponents.CommandList, CB_ADDSTRING, 0, (LPARAM)w_filename.c_str());
	}

	return ret;
}

void CmdAndConquer_MainWindow::CleanUpLastCommandSelection()
{
	AddCMDWindowComponents.OptionList.clear();
	AddCMDWindowComponents.opKeyList.clear();
}

void CmdAndConquer_MainWindow::CleanCurrentSelectionWindows()
{
	for (auto p : AddCMDWindowComponents.OptionList)
	{
		DestroyWindow(p.first);
		DestroyWindow(p.second);
	}
	DestroyWindow(AddCMDWindowComponents.SubmitButton);
	DestroyWindow(AddCMDWindowComponents.CancelButton);
}

void CmdAndConquer_MainWindow::ShowAddCMDWindow()
{
	g_fShowAddCMDWindow = TRUE;
}

void CmdAndConquer_MainWindow::HideAddCMDWindow()
{
	g_fShowAddCMDWindow = FALSE;
	ShowWindow(g_hwndAddCMDWindow, SW_HIDE);
	RECT rect;
	GetClientRect(g_hwndMain, &rect);
	PostMessage(g_hwndMain, WM_SIZE, 0, MAKEWPARAM(rect.right, rect.bottom));
}