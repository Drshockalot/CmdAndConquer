//
// Created by Chad Simpson on 18/11/2015.
//

#include "../Header/CMDCommandList.h"

CMDCommandList &CMDCommandList::instance() {
    list = new CMDCommandList();
    return *list;
}

// ~name~ signals that the user needs to provide input for that field
// #item means that those options should be put in a dropdown list
CMDCommandList::CMDCommandList() {
	this->availableCommands = {
		CMDCommand("ADDUSERS.exe",
		{
			CMDOption("~filename", true),
			CMDOption("/s:~delimiter~", false),
			CMDOption("~Domain", false),
			CMDOption("/c", false),
			CMDOption("/d{:~filename#}", false),
			CMDOption("/e", false),
			CMDOption("/p:#l#c#e#d", false)
		}, false),
        CMDCommand("cmd.exe",
        {
				CMDOption("/c"),
				CMDOption("/k"),
				CMDOption("/a"),
				CMDOption("/a"),
				CMDOption("/u"),
				CMDOption("/d"),
				CMDOption("/e:on"),
				CMDOption("/x"),
				CMDOption("/e:off"),
				CMDOption("/y"),
				CMDOption("/f:on"),
				CMDOption("/f:off"),
				CMDOption("/q"),
				CMDOption("/s"),
				CMDOption("/v:on"),
				CMDOption("/v:off")
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

CMDCommandList::~CMDCommandList() {

}

CMDCommand CMDCommandList::at(int i) {
    return this->list->at(i);
}

void CMDCommandList::addCMD(CMDCommand cmd) {
    this->list->availableCommands.push_back(cmd);
}

void CMDCommandList::removeCMD(std::string nameToRemove) {
    std::sort(std::begin(this->list->availableCommands), std::end(this->list->availableCommands));
    auto elementToRemove = std::find_if(std::begin(this->list->availableCommands), std::end(this->list->availableCommands), [nameToRemove](CMDCommand cmd){return cmd.getName() == nameToRemove;});
    this->list->availableCommands.erase(elementToRemove, elementToRemove + 1);
}

void CMDCommandList::removeCMD(int i) {
    std::sort(std::begin(this->list->availableCommands), std::end(this->list->availableCommands));
    auto elementToRemove = std::begin(this->list->availableCommands) + i;
    this->list->availableCommands.erase(elementToRemove, elementToRemove + 1);
}

void CMDCommandList::removeCMD(CMDCommand command) {
    std::sort(std::begin(this->list->availableCommands), std::end(this->list->availableCommands));
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

