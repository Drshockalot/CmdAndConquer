//
// Created by Chad Simpson on 19/11/2015.
//
#include <string>
#ifndef CMDLIBRARY_CMDOPTION_H
#define CMDLIBRARY_CMDOPTION_H
class CMDOption
{
public:
    CMDOption();
	CMDOption(std::string);
	CMDOption(std::string, bool);
    ~CMDOption();
    std::string getName();
    void setName(std::string);
private:
    std::string name;
	bool required;
};
#endif //CMDLIBRARY_CMDOPTION_H
