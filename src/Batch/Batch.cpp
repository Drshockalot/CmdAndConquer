#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include "Shlwapi.h"
#include "../CommandPipeRedirection/Header/Redir.h"
#include "../TextView/Header/TextView.h"
#include "../TextView/Header/TextViewInternal.h"
#include "../Unicode/Unicode.h"

#pragma comment(lib, "Shlwapi.lib")

#define BUFFERFORTHISSIZE 4096
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

PROCESS_INFORMATION CreateChildProcess(TCHAR *cmdLine);
CHAR* ReadFromPipe(PROCESS_INFORMATION);

CHAR* doStuff(TCHAR *cmdLine)
{
	SECURITY_ATTRIBUTES sa;
	// Set the bInheritHandle flag so pipe handles are inherited. 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	// Create a pipe for the child process's STDERR. 
	if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0)) {
		exit(1);
	}
	// Ensure the read handle to the pipe for STDERR is not inherited.
	if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
		exit(1);
	}
	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) {
		exit(1);
	}
	// Ensure the read handle to the pipe for STDOUT is not inherited
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		exit(1);
	}
	// Create the child process. 
	PROCESS_INFORMATION piProcInfo = CreateChildProcess(cmdLine);

	// The remaining open handles are cleaned up when this process terminates. 
	// To avoid resource leaks in a larger application, 
	//   close handles explicitly.
	return ReadFromPipe(piProcInfo);
}

PROCESS_INFORMATION CreateChildProcess(TCHAR *cmdLine) 
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	bool bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDERR and STDOUT handles for redirection.
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_ERR_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bSuccess = CreateProcess(NULL,
		cmdLine,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION
	CloseHandle(g_hChildStd_ERR_Wr);
	CloseHandle(g_hChildStd_OUT_Wr);
	// If an error occurs, exit the application. 
	if (!bSuccess) {
		exit(1);
	}
	return piProcInfo;
}

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
CHAR* ReadFromPipe(PROCESS_INFORMATION piProcInfo) {
	DWORD dwRead;
	CHAR chBufSuccess[BUFFERFORTHISSIZE];
	CHAR chBufFailure[BUFFERFORTHISSIZE];
	bool batchResult = FALSE;
	bool bSuccess = FALSE;

	for (;;) {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBufSuccess, BUFFERFORTHISSIZE, &dwRead, NULL);
		if (dwRead > 0)
			batchResult = TRUE;
		if (!bSuccess || dwRead == 0) break;
	}
	dwRead = 0;
	for (;;) {
		bSuccess = ReadFile(g_hChildStd_ERR_Rd, chBufFailure, BUFFERFORTHISSIZE, &dwRead, NULL);
		if (dwRead > 0)
			batchResult = FALSE;
		if (!bSuccess || dwRead == 0) break;
	}

	return (batchResult ? chBufSuccess : chBufFailure);
}

ULONG TextView::runFileAsBatch()
{
	TCHAR tempPath[MAX_PATH];
	TCHAR tempName[MAX_PATH];
	TCHAR comPath[MAX_PATH];
	TCHAR *prefix = _T("cac");

	GetTempPath(MAX_PATH, tempPath);
	GetTempFileName(tempPath, prefix, 0, tempName);
	PathCombine(comPath, tempPath, tempName);
	changeFileExt(comPath, _T("bat"));
	SaveFile(comPath);

	TCHAR *cmd = _T("cmd.exe /C \0");
	TCHAR cmdLine[MAX_PATH];
	cmdLine[0] = '\0';
	_tcscat_s(cmdLine, cmd);
	_tcscat_s(cmdLine, comPath);
	TCHAR *env = GetEnvironmentStrings();
	if (!env)
		abort();

	const TCHAR *var = env;
	size_t totallen = 0;
	size_t len;
	while ((len = wcslen(var)) > 0)
	{
		totallen += len + 1;
		var += len + 1;
	}
	std::wstring result(env, totallen);
	FreeEnvironmentStrings(env);
	
	CHAR *batchResult = doStuff(cmdLine);

	/*CRedirector redir;
	redir.Open(cmdLine);
*/
	//STARTUPINFO si = { sizeof(STARTUPINFO) };
	//PROCESS_INFORMATION pi;
	//HANDLE readPipe = NULL;
	//HANDLE writePipe = NULL;

	//SECURITY_ATTRIBUTES saAttr;
	//saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	//saAttr.bInheritHandle = TRUE;
	//saAttr.lpSecurityDescriptor = NULL;

	//CreatePipe(&readPipe, &readPipe, &saAttr, 0);
	//SetHandleInformation(&readPipe, HANDLE_FLAG_INHERIT, 0);

	//si.cb = sizeof(STARTUPINFO);
	//si.hStdOutput = readPipe;
	//si.dwFlags |= STARTF_USESTDHANDLES;


	//CreateProcess(NULL, cmdLine, NULL, NULL, false, CREATE_UNICODE_ENVIRONMENT, (LPVOID)result.c_str(), NULL, &si, &pi);
	//
	//DWORD dwRead, dwWritten;
	//TCHAR chBuf[4096];
	//BOOL bSuccess = FALSE;

	//for (;;)
	//{
	//	bSuccess = ReadFile(readPipe, chBuf, 4096, &dwRead, NULL);
	//	if (!bSuccess || dwRead == 0) break;
	//}

	return TRUE;
}