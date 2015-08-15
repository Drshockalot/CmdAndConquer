#include "../Header/CmdAndConquer_Globals.h"

int PointsToLogical(int nPointSize)
{
	HDC hdc = GetDC(0);
	int nLogSize = -MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(0, hdc);

	return nLogSize;
}

HFONT EasyCreateFont(int nPointSize, BOOL fBold, DWORD dwQuality, TCHAR *szFace)
{
	return CreateFont(PointsToLogical(nPointSize),
		0, 0, 0,
		fBold ? FW_BOLD : 0,
		0, 0, 0, 0, 0, 0,
		dwQuality,
		0,
		szFace);
}