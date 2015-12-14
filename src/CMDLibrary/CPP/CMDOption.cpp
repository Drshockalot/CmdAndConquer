//
// Created by Chad Simpson on 19/11/2015.
//

#include "../Header/CMDOption.h"

CMDOption::CMDOption() { }

CMDOption::CMDOption(std::string name) {
    (*this).name = name;
}

CMDOption::~CMDOption() { }

std::string CMDOption::getName() {
    return (*this).name;
}

void CMDOption::setName(std::string name) {
    (*this).name = name;
}
