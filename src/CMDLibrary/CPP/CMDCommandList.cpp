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
		CMDCommand("ADDUSERS",
		{									//most of this hinges on file name, make it required
			CMDOption("~", "Filename", true, TextBox), //filename
			CMDOption("/s:~", "Delimiter", false, TextBox), //delimiter
			CMDOption("~", "Domain", false, TextBox), //Domain
			CMDOption("/c", "Filename", false, CheckBox), //create user account by filename
			CMDOption("/d{:~}", "Filename", false, TextBox), //dump to filename
			CMDOption("/e", "Erase", false, CheckBox), //erase user in filename
			CMDOption("/p:#l#c#e#d", "Password Option", false, DropDown) //password options
		}, false),
        CMDCommand("CMD",
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
        CMDCommand("COMPRESS",
        {
				CMDOption("-R", "Rename files", false, CheckBox),
				CMDOption("-D", "Update files", false, CheckBox),
				CMDOption("-S", "Suppress copyright info", false, CheckBox),
				CMDOption("-ZX", "Default compression", false, CheckBox),
				CMDOption("Source", "Source files", false, TextBox),
				CMDOption("Destination", "Destination location", false, TextBox)
        }, false),
		CMDCommand("@ECHO",
		{
			CMDOption("ON", "Enable Echo", false, CheckBox),
			CMDOption("OFF", "Disable Echo", false, CheckBox),
			CMDOption("\"~\"", "Message", false, TextBox)
		}, false),
		CMDCommand("FIND",
		{
			CMDOption("\"~\"", "Key string", false, TextBox),
			CMDOption("\"~\"", "Pathname", false, TextBox),
			CMDOption("/v", "Display irrelevant lines", false, CheckBox),
			CMDOption("/c", "Display line count", false, CheckBox),
			CMDOption("/n", "Display line numbers", false, CheckBox),
			CMDOption("/i", "Case insensitive", false, CheckBox)
		}, false),
		CMDCommand("EXIT",
		{
			CMDOption("/b", "Exit script only", CheckBox),
			CMDOption("\"~\"", "Exit code", false, TextBox),
		}, false),
		CMDCommand("GOTO",
		{
			CMDOption(":~", "Label", false, TextBox),
			CMDOption(":eof", "End of file", false, CheckBox)
		}, false),
		CMDCommand("IPCONFIG",
		{
			CMDOption("/all", "Full information", false, CheckBox),
			CMDOption("/release ~", "Release Adaptor IP", false, TextBox),
			CMDOption("/renew ~", "Renew Adaptor IP", false, TextBox),
			CMDOption("/flushdns", "Purge DNS cache", false, CheckBox),
			CMDOption("/registerdns", "Refresh DNS names", false, CheckBox),
			CMDOption("/displaydns", "Display DNS names", false, CheckBox),
			CMDOption("/showclassid ~", "Display Adaptor classes", false, TextBox),
		}, false),
		CMDCommand("MEM",
		{
			CMDOption("/p", "List programs in memory", false, CheckBox),
			CMDOption("/d", "List Devices", false, CheckBox),
			CMDOption("/c", "List all memory", false, CheckBox),
		}, false),
		CMDCommand("REGEDIT",
		{
			CMDOption("\"~\"", "Import", false, TextBox),
			CMDOption("/e", "Export registry", false, CheckBox),
			CMDOption("/s", "Silent import", false, CheckBox)
		}, true),
		CMDCommand("REGSVR32",
		{
			CMDOption("\"~\"", "Path to DLL", false, TextBox),
			CMDOption("/u", "Unregister server", false, CheckBox),
			CMDOption("/s", "Silent", false, CheckBox),
			CMDOption("/i", "Call DllInstall", false, CheckBox),
			CMDOption("/n", "No DllRegisterServer call", false, CheckBox),
			CMDOption("\"~\"", "DllInstall CMDLine", false, TextBox),
			CMDOption("/c", "Console output", false, CheckBox)

		}, false),
		CMDCommand("TELNET",
		{
			CMDOption("-a", "Automatic login", false, CheckBox),
			CMDOption("-e ~", "Escape character", false, TextBox),
			CMDOption("-f ~", "Log file", false, TextBox),
			CMDOption("-l", "Username", false, TextBox),
			CMDOption("-t", "Terminal type", false, TextBox),
			CMDOption("~", "Host", false, TextBox),
			CMDOption("~", "Port", false, TextBox)
		}, false)
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
