#include <Windows.h>
#include "../../TextView/Header/TextView.h"
#include "../../TextView/Header/TextViewInternal.h"
#include "../Headers/BatchSyntax.h"

ULONG TextView::addForStatement(int id)
{
	auto text = ForLoops[id];
	EnterText(const_cast<TCHAR*>(text.c_str()), text.length());
	return 0;
}

ULONG TextView::addIfStatement(int id)
{
	auto text = IfStatements[id];
	EnterText(const_cast<TCHAR*>(text.c_str()), text.length());
	return 0;
}
