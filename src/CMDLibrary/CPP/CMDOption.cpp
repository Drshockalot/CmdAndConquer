//
// Created by Chad Simpson on 19/11/2015.
//

#include "../Header/CMDOption.h"
#include <oaidl.h>

CMDOption::CMDOption() : Command(""), Label(""), Required(false), Included(false), Type(-1) { }

CMDOption::CMDOption(const CMDOption& lOp)
{
	(*this).Command = lOp.Command;
	(*this).Label = lOp.Label;
	(*this).Required = lOp.Required;
	(*this).Included = false;
	(*this).Type = lOp.Type;
}

CMDOption::CMDOption(std::string Command)
{
	(*this).Command = Command;
	(*this).Label = "";
	(*this).Required = false;
	(*this).Included = false;
	(*this).Type = -1;
}

CMDOption::CMDOption(std::string Command, std::string Label) 
{
    (*this).Command = Command;
	(*this).Label = Label;
	(*this).Required = false;
	(*this).Included = false;
	(*this).Type = -1;
}

CMDOption::CMDOption(std::string Command, std::string Label, bool Required)
{
	(*this).Command = Command;
	(*this).Label = Label;
	(*this).Required = Required;
	(*this).Included = false;
	(*this).Type = -1;
}

CMDOption::CMDOption(std::string Command, std::string Label, int Type)
{
	(*this).Command = Command;
	(*this).Label = Label;
	(*this).Required = false;
	(*this).Included = false;
	(*this).Type = Type;
}

CMDOption::CMDOption(std::string Command, std::string Label, bool Required, int Type)
{
	(*this).Command = Command;
	(*this).Label = Label;
	(*this).Required = Required;
	(*this).Included = false;
	(*this).Type = Type;
}

CMDOption::CMDOption(std::string Command, std::string Label, bool Required, bool Included, int Type)
{
	(*this).Command = Command;
	(*this).Label = Label;
	(*this).Required = Required;
	(*this).Included = Included;
	(*this).Type = Type;
}

CMDOption::~CMDOption() { }

std::string CMDOption::getCommand() {
    return (*this).Command;
}

void CMDOption::setCommand(std::string Command) {
    (*this).Command = Command;
}

int CMDOption::getType()
{
	return (*this).Type;
}

std::vector<std::string> CMDOption::extractDropDownList()
{
	std::string locCom = (*this).Command;
	std::vector<std::string> opList;
	
	int pos = distanceToNextOption(0);
	int dist = 0;
	int c = std::count(std::begin(locCom), std::end(locCom), '#');
	std::vector<int> posList;
	std::vector<int> disList;
	for (int i = 0; i < c; ++i)
	{
		posList.push_back(pos);
		dist = distanceToNextOption(pos);
		disList.push_back(dist);
		pos = positionOfNextOption(pos);
		
	}
	std::string op;
	for (int i = 0; i < c; ++i)
	{
		op = locCom.substr(posList[i], disList[i]);
		
		/*if(i == c - 1)
		{
			std::string test = locCom.substr(posList[i]);
			op = test;
		}
		else
		{
			std::string test = locCom.substr(posList[i], posList[i] - posList[i + 1] + 1);
			op = test;
		}*/
		opList.push_back(op);
	}

	/*while (hasNextOption(pos))
	{
		opList.push_back(getNextOption());
		pos = distanceToNextOption(pos);
	}*/
	return opList;
}

int CMDOption::distanceToNextOption(int n)
{
	return (*this).Command.find("#", n + 1, 1) - n;
}

int CMDOption::positionOfNextOption(int n)
{
	return (*this).Command.find("#", n + 1, 1);
}


std::wstring CMDOption::getLabelW()
{
	std::wstring w_Label;
	int len = MultiByteToWideChar(CP_ACP, 0, (*this).Label.c_str(), (*this).Label.length(), nullptr, 0);
	if (len > 0)
	{
		w_Label.resize(len);
		MultiByteToWideChar(CP_ACP, 0, (*this).Label.c_str(), (*this).Label.length(), &w_Label[0], len);
	}
	return w_Label;
}

std::string CMDOption::getLabelA()
{
	return (*this).Label;
}

void CMDOption::AddUserDropDownVal(std::string val)
{
	int pos = (*this).Command.find("#");
	std::string baseOp = (*this).Command.substr(0, pos);
	(*this).Command.replace(pos, (*this).Command.size() - pos, val.substr(1));
}

void CMDOption::AddUserTextBoxVal(std::string val)
{
	int pos = (*this).Command.find("~");
	std::string baseOp = (*this).Command.substr(0, pos);
	(*this).Command.insert(pos, val);
	(*this).Command.erase(val.size(), 1);
}

void CMDOption::AddUserCheckBoxVal(bool val)
{
	(*this).Included = val;
}

bool CMDOption::isIncluded()
{
	return (*this).Included;
}

bool CMDOption::isRequired()
{
	return (*this).Required;
}
