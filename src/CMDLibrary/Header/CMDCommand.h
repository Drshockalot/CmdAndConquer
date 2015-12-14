//
// Created by Chad Simpson on 18/11/2015.
//
#include <string>
#include <vector>
#include <algorithm>
#include "CMDOption.h"
#ifndef CMDLIBRARY_CMDCOMMAND_H
#define CMDLIBRARY_CMDCOMMAND_H
class CMDCommand
{
public:
    CMDCommand();
    CMDCommand(const std::string);
	CMDCommand(const std::string, bool);
    CMDCommand(const std::string, std::vector<CMDOption>, bool);
    CMDCommand(const std::string, std::vector<CMDOption>, std::vector<std::string>);
    ~CMDCommand();

    void setArguments(std::vector<std::string>);
    void setArugments(std::string[], size_t size);
    void clearArguments();
    void addArgument(std::string);
    void addArgument(char[]);
    void addArgumentAt(std::string, int);
    void addArgumentAt(char[], int);
    void removeArgument(int);
    std::string toString();
    std::vector<std::string> getArguments();
    std::string getName();
    std::vector<CMDOption> getAvailableOptions();

private:
    std::string name;
    std::vector<std::string> activeArguments;
    std::vector<CMDOption> availableOptions;
	bool deprecated;

    std::string activeArgsToString();
};

#endif //CMDLIBRARY_CMDCOMMAND_H
