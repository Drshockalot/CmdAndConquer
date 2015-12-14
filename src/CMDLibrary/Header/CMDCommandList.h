//
// Created by Chad Simpson on 18/11/2015.
//
#include "CMDCommand.h"
#ifndef CMDLIBRARY_CMDCOMMANDLIST_H
#define CMDLIBRARY_CMDCOMMANDLIST_H

class CMDCommandList
{
public:
    static CMDCommandList& instance();
    CMDCommand at(int);
    void addCMD(CMDCommand);
    void removeCMD(std::string);
    void removeCMD(int);
    void removeCMD(CMDCommand);
    void addCMD(std::string);

private:
    CMDCommandList();
    ~CMDCommandList();
    static CMDCommandList* list;
    std::vector<CMDCommand> availableCommands;
};

std::vector<CMDCommand> initCMDList();
#endif //CMDLIBRARY_CMDCOMMANDLIST_H
