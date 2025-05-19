#pragma once
#ifndef MSGMACROS_H
#define MSGMACROS_H

#include <string>
#include <vector>

extern void s_splitString(std::vector<std::string>& out, const std::string& str, char delimiter);
extern bool getIfCondition(const std::string &line, bool needElseIf, std::string &cond, std::vector<std::string> &values);

extern void msgMacroProcess(const std::string in, std::string &out, int macro_player, int macro_state);

#endif // MSGMACROS_H
