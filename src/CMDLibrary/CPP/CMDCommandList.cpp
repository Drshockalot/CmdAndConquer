//
// Created by Chad Simpson on 18/11/2015.
//

#include "../Header/CMDCommandList.h"

CMDCommandList * CMDCommandList::list = 0;

CMDCommandList *CMDCommandList::instance() {
	if(!list)
	{
		list = new CMDCommandList();
	}
    return list;
}

// ~name~ signals that the user needs to provide input for that field
// #item means that those options should be put in a dropdown list
CMDCommandList::CMDCommandList() {
	this->availableCommands = {
		CMDCommand("ADDUSERS.exe",
		{									//most of this hinges on file name, make it required
			CMDOption("~", "Filename", true, TextBox), //filename
			CMDOption("/s:~", "Delimiter", false, TextBox), //delimiter
			CMDOption("~", "Domain", false, TextBox), //Domain
			CMDOption("/c", "Filename", false, CheckBox), //create user account by filename
			CMDOption("/d{:~}", "Filename", false, TextBox), //dump to filename
			CMDOption("/e", "Erase", false, CheckBox), //erase user in filename
			CMDOption("/p:#l#c#e#d", "Password Option", false, DropDown) //password options
		}, false),
        CMDCommand("cmd.exe",
        {
				CMDOption("~", "Command", true, TextBox),
				CMDOption("/c", "Run and Terminate", false, CheckBox),
				CMDOption("/k", "Return to prompt", false, CheckBox),
				CMDOption("/a", "Output ANSI", false, CheckBox),
				CMDOption("/u", "Output Unicode", false, CheckBox),
				CMDOption("/d", "Ignore Registry Autorun", false, CheckBox),
				CMDOption("/e:on", "Enable CMD Extensions", false, CheckBox),
				CMDOption("/x", "Old above", false, CheckBox),
				CMDOption("/e:off", "Disable CMD Extensions", false, CheckBox),
				CMDOption("/y", "Old above", false, CheckBox),
				CMDOption("/f:on", "Enable Autocomplete Path Name", false, CheckBox),
				CMDOption("/f:off", "Disable Autocomplete Path Name", false, CheckBox),
				CMDOption("/q", "Turn ECHO off", false, CheckBox),
				CMDOption("/s", "Strip quotes from Command", false, CheckBox),
				CMDOption("/v:on", "Enable Delayed Vars", false, CheckBox),
				CMDOption("/v:off", "Disable Delayed Vars", false, CheckBox)
        }, false),
        CMDCommand("ADmodcmd.exe",
        {
				CMDOption("-dn \"\""),
				CMDOption("~modification~"),
				CMDOption("-p \"\""),
				CMDOption("-s"),
				CMDOption("-server \"\""),
				CMDOption("-f \"\""),
				CMDOption("-undo \"\"")
        }, false),
		CMDCommand("ARP.exe",
		{
			CMDOption("-a"),
			CMDOption("-g"),
			CMDOption("-N \"\""),
			CMDOption("-d \"\""),
			CMDOption("-s \"\" \"\"")
		}, false),
		CMDCommand("ASSOC",
		{
			CMDOption("\"\"")
		}, false),
		CMDCommand("ASSOCIATE.exe",
		{
			CMDOption("~.ext"),
			CMDOption("~filename"),
			CMDOption("/q"),
			CMDOption("/f"),
			CMDOption("/d")
		}, false),
		CMDCommand("ATTRIB.exe",
		{
			CMDOption("+ \"\""),
			CMDOption("- \"\""),
			CMDOption("/s"),
			CMDOption("/d")
		}, false),
		CMDCommand("BCDBOOT.exe",
		{
			CMDOption("~source~"),
			CMDOption("/l \"\""),
			CMDOption("/s \"\""),
			CMDOption("/v"),
			CMDOption("/m \"[{}]\"")
		}, false),
		CMDCommand("BCDEdit.exe",
		{
			CMDOption("/createstore"),
			CMDOption("/export"),
			CMDOption("/import"),
			CMDOption("/store")
		}, false),
		CMDCommand("BITSADMIN",
		{
			CMDOption("/UTIL /?"),
			CMDOption(""),
		}, true),
		CMDCommand("BCDEdit.exe",
		{
			CMDOption("/addsw"),
			CMDOption("/copy"),
			CMDOption("/dbg1394"),
			CMDOption("/debug"),
			CMDOption("/default"),
			CMDOption("/delete"),
			CMDOption("/ems"),
			CMDOption("/list"),
			CMDOption("/query"),
			CMDOption("/raw"),
			CMDOption("/rebuild"),
			CMDOption("/rmsw"),
			CMDOption("/timeout")
		}, false),
		CMDCommand("BROWSTAT.exe",
		{
			CMDOption("dn"),
			CMDOption("gp"),
			CMDOption("gm"),
			CMDOption("gb"),
			CMDOption("wfw"),
			CMDOption("sts"),
			CMDOption("sta"),
			CMDOption("sta -v"),
			CMDOption("Tic"),
			CMDOption("el"),
			CMDOption("vw"),
		}, false),
		CMDCommand("",
		{
			CMDOption("")
		}, false),
		CMDCommand("",
		{
			CMDOption("")
		}, false),
		CMDCommand("",
		{
			CMDOption("")
		}, false),
    };
}

CMDCommandList::~CMDCommandList()
{
}

CMDCommand CMDCommandList::at(int i) {
    return this->list->availableCommands.at(i);
}

void CMDCommandList::addCMD(CMDCommand cmd) {
    this->list->availableCommands.push_back(cmd);
}

void CMDCommandList::removeCMD(std::string nameToRemove) {
    auto elementToRemove = std::find_if(std::begin(this->list->availableCommands), std::end(this->list->availableCommands), [nameToRemove](CMDCommand cmd){return cmd.getName() == nameToRemove;});
    this->list->availableCommands.erase(elementToRemove, elementToRemove + 1);
}

void CMDCommandList::removeCMD(int i) {
    this->list->availableCommands.erase(std::begin(this->list->availableCommands) + i);
}

void CMDCommandList::removeCMD(CMDCommand command) {
    auto elementToRemove = std::begin(this->list->availableCommands);
    for(auto it = this->list->availableCommands.begin(); it != this->list->availableCommands.end(); ++it)
    {
        if(it->getName() == command.getName())
            elementToRemove = it;
    }
    this->list->availableCommands.erase(elementToRemove, elementToRemove + 1);
}

void CMDCommandList::addCMD(std::string cmdName) {
    CMDCommand cmd(cmdName);
    this->list->availableCommands.push_back(cmd);
}

size_t CMDCommandList::size()
{
	return this->list->availableCommands.size();
}
