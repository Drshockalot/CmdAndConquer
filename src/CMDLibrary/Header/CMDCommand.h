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
    CMDCommand(const std::string, std::vector<CMDOption>, std::vector<CMDOption>);
    ~CMDCommand();

    void setArguments(std::vector<CMDOption>);
    void setArugments(std::string[], size_t size);
    void clearArguments();
    void addArgument(std::string);
    void addArgument(char[]);
	void addArgument(CMDOption);
    void addArgumentAt(std::string, int);
    void addArgumentAt(char[], int);
    void removeArgument(int);
    std::string toString();
    std::vector<CMDOption> getActiveArguments();
    std::string getName();
    std::vector<CMDOption> getAvailableOptions();

private:
    std::string name;
    std::vector<CMDOption> activeArguments;
    std::vector<CMDOption> availableOptions;
	bool deprecated;

    std::string activeArgsToString();
};

#endif //CMDLIBRARY_CMDCOMMAND_H
