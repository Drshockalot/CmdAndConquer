//
// Created by Chad Simpson on 23/11/2015.
//

#include "CMDCommandList.h"

#ifndef CMDLIBRARY_CMDLIBRARY_H
#define CMDLIBRARY_CMDLIBRARY_H

class CMDLibrary
{
public:
    static CMDCommandList& getInstance()
    {
        static CMDCommandList *CMDList = new CMDCommandList();
        return *CMDList;
    }
private:
    CMDLibrary(){}
};

#endif //CMDLIBRARY_CMDLIBRARY_H
