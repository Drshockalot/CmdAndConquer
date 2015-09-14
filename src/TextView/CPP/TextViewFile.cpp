#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include "../Header/TextView.h"
#include "../Header/TextViewInternal.h"
#include "../../Unicode/Unicode.h"

//
//
//
LONG TextView::OpenFile(TCHAR *szFileName)
{
	ClearFile();

	if (m_pTextDoc->init(szFileName))
	{
		m_nLineCount = m_pTextDoc->linecount();
		m_nLongestLine = m_pTextDoc->longestline(m_nTabWidthChars);

		m_nVScrollPos = 0;
		m_nHScrollPos = 0;

		m_nSelectionStart = 0;
		m_nSelectionEnd = 0;
		m_nCursorOffset = 0;

		UpdateMarginWidth();
		UpdateMetrics();
		ResetLineCache();
		return TRUE;
	}

	return FALSE;
}

LONG TextView::SaveFile(TCHAR *szFileName)
{
	ULONG docBeginning = 0;
	ULONG docLength = m_pTextDoc->m_nDocLength_bytes;

	HANDLE hMem;
	TCHAR *ptrBuf = NULL;

	if ((hMem = GlobalAlloc(GPTR, (docLength + 1) * sizeof(TCHAR))) != 0)
	{
		if ((ptrBuf = (TCHAR *)GlobalLock(hMem)) != 0)
		{
			GetText(ptrBuf, docBeginning, docLength + 1);

			GlobalUnlock(hMem);
		}
	}

	BYTE* buffer = new BYTE[m_pTextDoc->m_nDocLength_bytes];
	HANDLE hFile;
	DWORD bytesToWrite = lstrlenW(ptrBuf);
	DWORD bytesWritten = 0;
	CCHAR* asciichars = NULL;
	size_t* aslen = (size_t *)&m_pTextDoc->m_nDocLength_bytes;
	
	size_t len = utf16_to_ascii((UTF16 *)ptrBuf, (bytesToWrite + 1) * sizeof(TCHAR), buffer, aslen);

	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL errorflag = WriteFile(hFile, buffer, bytesToWrite, &bytesWritten, NULL);
		
	CloseHandle(hFile);
	delete[] buffer;
	return TRUE;
}

//
//
//
LONG TextView::ClearFile()
{
	if (m_pTextDoc)
	{
		m_pTextDoc->clear();
		m_pTextDoc->EmptyDoc();
	}

	ResetLineCache();

	m_nLineCount = m_pTextDoc->linecount();
	m_nLongestLine = m_pTextDoc->longestline(m_nTabWidthChars);

	m_nVScrollPos = 0;
	m_nHScrollPos = 0;

	m_nSelectionStart = 0;
	m_nSelectionEnd = 0;
	m_nCursorOffset = 0;

	m_nCurrentLine = 0;
	m_nCaretPosX = 0;

	UpdateMetrics();



	return TRUE;
}

ULONG TextView::changeFileExt(TCHAR *fileName, TCHAR *extension)
{
	size_t fileLen = wcslen(fileName);
	size_t extLen = wcslen(extension);
	size_t oldExtPos;
	size_t oldExtLen;

	TCHAR newPath[MAX_PATH + 1];

	for (int i = 0; i < fileLen; ++i)
	{
		if (fileName[i] == _T('.'))
		{
			oldExtPos = i;
			oldExtLen = fileLen - i;
		}
	}

	for (int i = 0; i < extLen; ++i)
	{
		fileName[oldExtPos + 1 + i] = extension[i];
	}

	fileName[oldExtPos + extLen + 1] = '\0';
	
	return 0;
}