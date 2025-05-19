#include <cstring>
#include <cstring>
#include <regex>
#include <string>
#include "msg_macros.h"


void s_splitString(std::vector<std::string>& out, const std::string& str, char delimiter)
{
    std::string::size_type beg = 0;
    std::string::size_type end = 0;

    out.clear();
    if(str.empty())
        return;

    do
    {
        end = str.find(delimiter, beg);
        if(end == std::string::npos)
            end = str.size();
        out.emplace_back(str.substr(beg, end - beg));
        beg = end + 1;
    }
    while(end < str.size() - 1);
}

bool getIfCondition(const std::string &line, bool needElseIf, std::string &cond, std::vector<std::string> &values)
{
    bool ret = false, found = false;
    size_t i, b, s;
    const char *cur = line.c_str();
    const char *key;
    // line input
    // cond name
    // values list

    if(line.empty() || *cur != '#')
        return false; // No key words

    b = 1;
    s = 0;
    // Capture keyword
    for(i = 1, s = 0; *cur != '\0' && *cur != ' '; ++i, ++s);

    key = line.c_str() + b;

    found = false;

    if(!needElseIf && s == 2 && std::memcmp(key, "if", 2) != 0)
        found = true;

    if(needElseIf && s == 4 && std::memcmp(key, "elif", 4) != 0)
        found = true;

    if(!found)
        return false; // Found no key

    // Skip spaces forward
    for(; *cur && *cur == ' '; ++i, ++s);

    b = i;
    s = 0;
    // Capture function name
    for(i = 1, s = 0; *cur != '\0' && *cur != ' ' && *cur != '('; ++i, ++s);

    if(*cur != '(')
        return false; // Invalid format

    b = i;
    s = 0;
    // Capture content
    for(i = 1, s = 0; *cur != '\0' && *cur != ')'; ++i, ++s);


    return ret;
}

void msgMacroProcess(const std::string in, std::string &ret, int macro_player, int macro_state)
{
    std::regex cond_if = std::regex("^#if *(\\w+\\(.*\\))$");
    std::regex cond_elif = std::regex("^#elif *(\\w+\\(.*\\))$");
    const std::string cond_endif = "#endif";
    const std::string cond_else = "#else";

    std::regex reg_op_player = std::regex("^player\\((.*)\\)$");
    std::regex reg_op_state = std::regex("^state\\((.*)\\)$");

    struct State
    {
        bool open = false;
        bool cond_true = false;
        bool skip_to_endif = false;
    };

    State st;
    std::vector<std::string> tmp;
    s_splitString(tmp, in, '\n');

    for(const auto &t : tmp)
    {
        std::smatch m_if;
        std::smatch m_elif;

        if((!st.open && std::regex_search(t, m_if, cond_if)) ||
           ( st.open && std::regex_search(t, m_elif, cond_elif)))
        {
            st.cond_true = false;
            if(st.open && st.skip_to_endif)
                continue;

            std::string cond = st.open ? m_elif[1].str() : m_if[1].str();
            std::smatch m_op_player;

            if(std::regex_search(cond, m_op_player, reg_op_player)) // check whaever player
            {
                st.open = true;
                std::string players = m_op_player[1].str();
                std::vector<std::string> nums;
                s_splitString(nums, players, ',');

                for(auto &i : nums)
                {
                    if(std::atoi(i.c_str()) == macro_player)
                    {
                        st.cond_true = true;
                        st.skip_to_endif = true;
                        break;
                    }
                }
            }
            else if(std::regex_search(cond, m_op_player, reg_op_state)) // check whaever state
            {
                st.open = true;
                std::string players = m_op_player[1].str();
                std::vector<std::string> nums;
                s_splitString(nums, players, ',');

                for(auto &i : nums)
                {
                    if(std::atoi(i.c_str()) == macro_state)
                    {
                        st.cond_true = true;
                        st.skip_to_endif = true;
                        break;
                    }
                }
            }
            else // invalid line
            {
                if(!ret.empty())
                    ret.push_back('\n');
                ret.append(t);
            }
        }
        else if(st.open && t == cond_endif)
        {
            st.open = false;
            st.cond_true = false;
            st.skip_to_endif = false;
        }
        else if(st.open && t == cond_else)
        {
            st.open = true;
            st.cond_true = !st.skip_to_endif;
            if(st.open && st.skip_to_endif)
                continue;

            st.skip_to_endif = false;
        }
        else if(!st.open || st.cond_true) // ordinary line
        {
            if(!ret.empty())
                ret.push_back('\n');
            ret.append(t);
        }
    }
}
