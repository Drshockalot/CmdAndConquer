//
// Created by Chad Simpson on 19/11/2015.
//
#include <string>
#ifndef CMDLIBRARY_CMDOPTION_H
#define CMDLIBRARY_CMDOPTION_H
#include <vector>

class CMDOption
{
public:
    CMDOption();
	CMDOption(const CMDOption&);
	CMDOption(std::string);
	CMDOption(std::string, std::string);
	CMDOption(std::string, std::string, bool);
	CMDOption(std::string, std::string, int);
	CMDOption(std::string, std::string, bool, int);
	CMDOption(std::string, std::string, bool, bool, int);
    ~CMDOption();
    std::string getCommand();
    void setCommand(std::string);
	int getType();
	int positionOfNextOption(int);
	int distanceToNextOption(int);
	std::vector<std::string> extractDropDownList();
	std::wstring getLabelW();
	std::string getLabelA();
	void AddUserDropDownVal(std::string val);
	void AddUserTextBoxVal(std::string val);
	void AddUserCheckBoxVal(bool val);
	bool isIncluded();
	bool isRequired();
private:
	std::string Command;
	std::string Label;
	bool Required;
	bool Included;
	int Type;
};

enum opTypes
{
	CheckBox,
	DropDown,
	TextBox,
	LabelOnly
};
#endif //CMDLIBRARY_CMDOPTION_H
