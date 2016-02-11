#include "tchar.h"

enum 
{
	FORFiles,
	FORFilesRootedAtPath,
	FORFolders,
	FORListOfNumbers,
	FORFileSetContents,
	FORFileTextStringContents,
	FORCommandResults
};

enum
{
	IFExist,
	IFExistElse,
	IFEquals,
	IFComparison,
	IFComparisonElse,
	IFNotDefined,
	IFErrorLevel
};

const std::wstring ForLoops[7] = {
	_T("FOR [%%parameter] IN ( [set] ) DO [command]"),
	_T("FOR /R [[drive:]path] [%%parameter] IN ( [set] ) DO [command]"),
	_T("FOR /D [%%parameter] IN ( [folder_set] ) DO [command] "),
	_T("FOR /L [%%parameter] IN ( [start,step,end] ) DO [command]"),
	_T("FOR /F [options] [%%parameter] IN ( [filenameset] ) DO [command]"),
	_T("FOR /F [options] [%%parameter] IN ( [Text string to process] ) DO [command]"),
	_T("FOR /F [options] [%%parameter] IN ( [command to process] ) DO [command]")
};

const std::wstring IfStatements[7] = {
	_T("IF [NOT] EXIST [filename] [command]"),
	_T("IF [NOT] EXIST [filename] ( [command] ) ELSE ( [command] )"),
	_T("IF [/I] [NOT] [item1==item2] [command]"),
	_T("IF [/I] [item1] [compare-op] [item2] [command]"),
	_T("IF [/I] [item1] [compare-op] [item2] ( [command] ) ELSE ( [command] )"),
	_T("IF [NOT] DEFINED [variable] [command]"),
	_T("IF [NOT] ERRORLEVEL [number] [command]")
};