//
// Created by Chad Simpson on 18/11/2015.
//

#include "../Header/CMDCommand.h"

CMDCommand::CMDCommand() { }

CMDCommand::CMDCommand(const std::string name) {
    (*this).name = name;
}

CMDCommand::CMDCommand(const std::string name, bool deprecated)
{
	(*this).name = name;
	(*this).deprecated = deprecated;
}

CMDCommand::CMDCommand(const std::string name, std::vector<CMDOption> availableOptions, std::vector<std::string> activeArgs) {
    (*this).name = name;
    (*this).availableOptions = availableOptions;
    (*this).activeArguments = activeArgs;
}

CMDCommand::~CMDCommand() { }

std::vector<std::string> CMDCommand::getArguments() {
    return (*this).activeArguments;
}

std::string CMDCommand::getName() {
    return (*this).name;
}

std::vector<CMDOption> CMDCommand::getAvailableOptions() {
    return (*this).availableOptions;
}

std::string CMDCommand::toString() {
    std::string ret;
    ret = (*this).name;
    ret = ret + (*this).activeArgsToString();
    return ret;
}

std::string CMDCommand::activeArgsToString() {
    std::string ret;
    std::for_each(std::begin((*this).activeArguments), std::end((*this).activeArguments), [&ret] (std::string s) {ret = ret + " " + s;});
    return ret;
}


void CMDCommand::removeArgument(int i) {
    activeArguments.erase(std::remove(std::begin(activeArguments), std::end(activeArguments), i), std::end(activeArguments));
}

void CMDCommand::addArgument(char *arg) {
    std::string str_arg(arg);
    (*this).activeArguments.push_back(str_arg);
}

void CMDCommand::addArgument(std::string arg) {
    (*this).activeArguments.push_back(arg);
}

void CMDCommand::clearArguments() {
    (*this).activeArguments.clear();
}

void CMDCommand::setArugments(std::string *args, size_t size) {
    for(int i = 0; i < size; ++i)
    {
        (*this).activeArguments.push_back(*args);
        ++args;
    }
}

void CMDCommand::setArguments(std::vector<std::string> args) {
    (*this).activeArguments = args;
}

void CMDCommand::addArgumentAt(char *arg, int i) {
    std::string str_arg(arg);
    (*this).activeArguments.insert(std::begin((*this).activeArguments) + i, str_arg);
}

void CMDCommand::addArgumentAt(std::string arg, int i) {
    (*this).activeArguments.insert(std::begin((*this).activeArguments) + i, arg);
}

CMDCommand::CMDCommand(const std::string name, std::vector<CMDOption> availOps, bool deprecated) {
    (*this).name = name;
    (*this).availableOptions = availOps;
	(*this).deprecated = deprecated;
}
