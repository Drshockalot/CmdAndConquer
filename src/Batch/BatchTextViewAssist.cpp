#include <Windows.h>
#include <tchar.h>
#include "../CommandPipeRedirection/Header/Redir.h"
#include "../TextView/Header/TextView.h"
#include "../TextView/Header/TextViewInternal.h"
#include "../TextView/Header/TextDocument.h"
#include "../Unicode/Unicode.h"

ULONG TextView::SetTextBase(CHAR *aText)
{
	//ClearFile();

	if (m_pTextDoc->initBatchResultWindow(aText))
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

bool TextDocument::initBatchResultWindow(CHAR *aText)
{
	if ((m_nDocLength_bytes = strlen(aText)) == 0)
		return false;

	char *buffer = new char[m_nDocLength_bytes];

	//	couple of strange control characters are appearing at the end, find out why
	snprintf(buffer, m_nDocLength_bytes - 2, "%s", aText);

	m_seq.init((BYTE *)buffer, m_nDocLength_bytes);

	m_nFileFormat = detect_file_format(&m_nHeaderSize);

	if (!init_linebuffer())
		clear();
	
	return true;
}