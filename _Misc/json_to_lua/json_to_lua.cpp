#include <stdio.h>
#include <stack>
#include <iostream>

#include "json.hpp"

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

using json = nlohmann::json;

struct LuaSACKS : public nlohmann::json_sax<json>
{
    luabind::object data;
    lua_State *L = nullptr;

    LuaSACKS() : nlohmann::json_sax<json>()
    {}

    ~LuaSACKS()
    {
        cleanStack();
    }

    void cleanStack()
    {
        while(!stack.empty())
            stack.pop();
    }

    struct State
    {
        std::string     key;
        size_t          couter = 1;
        size_t          totalElements = 0;
        bool            isArray = false;
        luabind::object lua;
    };

    std::stack<State> stack;
    State             cs;
    bool              isRoot = true;

    template<class T>
    void addValue(const T &value)
    {
        assert(cs.lua.is_valid());
        if(cs.isArray)
            cs.lua[cs.couter++] = value;
        else
            cs.lua[cs.key] = value;
    }

    // called when null is parsed
    bool null()
    {
        addValue(nullptr);
        return true;
    }

    // called when a boolean is parsed; value is passed
    bool boolean(bool val)
    {
        addValue(val);
        return true;
    }

    // called when a signed or unsigned integer number is parsed; value is passed
    bool number_integer(number_integer_t val)
    {
        addValue(val);
        return true;
    }
    bool number_unsigned(number_unsigned_t val)
    {
        addValue(val);
        return true;
    }

    // called when a floating-point number is parsed; value and original string is passed
    bool number_float(number_float_t val, const string_t &)
    {
        addValue(val);
        return true;
    }

    // called when a string is parsed; value is passed and can be safely moved away
    bool string(string_t& val)
    {
        addValue(val);
        return true;
    }

    bool startObj(std::size_t elements, bool isArray)
    {
        if(isRoot)
        {
            isRoot = false;
            cs = State();
            data = luabind::newtable(L);
            cs.lua = data;
        }
        else
        {
            State ncs = State();
            if(cs.isArray)
            {
                size_t count = cs.couter++;
                cs.lua[count] = luabind::newtable(L);
                ncs.lua = cs.lua[count];
            }
            else
            {
                cs.lua[cs.key] = luabind::newtable(L);
                ncs.lua = cs.lua[cs.key];
            }
            stack.push(cs);
            cs = ncs;
        }
        cs.couter = 1;
        cs.totalElements = elements;
        cs.isArray = isArray;
        cs.key.clear();
        return true;
    }

    bool endObj()
    {
        if(stack.empty())
        {
            isRoot = true;
        }
        else
        {
            cs = stack.top();
            stack.pop();
        }
        return true;
    }

    // called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
    bool start_object(std::size_t elements)
    {
        return startObj(elements, false);
    }
    bool end_object()
    {
        return endObj();
    }

    bool start_array(std::size_t elements)
    {
        return startObj(elements, true);
    }

    bool end_array()
    {
        return endObj();
    }

    // called when an object key is parsed; value is passed and can be safely moved away
    bool key(string_t &val)
    {
        cs.key = val;
        return true;
    }

    // called when a parse error occurs; byte position, the last token, and an exception is passed
    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::detail::exception &ex)
    {
        if(!data.is_valid())
            data = luabind::newtable(L);
        data["error"] = luabind::newtable(L);
        data["error"]["position"] = position;
        data["error"]["last_token"] = last_token;
        data["error"]["exception"] = ex.what();
        cleanStack();
        return true;
    }
};

static luabind::object getSampleJson(lua_State *L)
{
    FILE *j = fopen(PATH_TO_SAMPLE_JSON, "r");
    std::string buffer;
    if(j)
    {
        fseek(j, 0, SEEK_END);
        size_t fSize = ftell(j);
        fseek(j, 0, SEEK_SET);
        buffer.resize(fSize);
        fread(&buffer[0], 1, fSize, j);
        fclose(j);

        if(!buffer.empty())
        {
            LuaSACKS sax;
            sax.L = L;
            json::sax_parse(buffer, &sax);
            return sax.data;
        }
        else
            return luabind::newtable(L);
    }
    else
    {
        fprintf(stderr, "Failed to open JSON!\n");
        return luabind::newtable(L);
    }
}

int main()
{
    lua_State* L = luaL_newstate();
    if(!L)
    {
        fprintf(stderr, "Failed to initialize Lua Engine!\n");
        return 1;
    }

    lua_pushcfunction(L, luaopen_base);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_package);
    lua_call(L, 0, 0);

    lua_pushcfunction(L, luaopen_io);
    lua_call(L, 0, 0);

    lua_getfield(L, LUA_GLOBALSINDEX, "package");
    lua_getfield(L, -1, "preload");

    //Remove unsafe apis
    {
        luabind::object _G =    luabind::globals(L);
        luabind::object osTable = _G["os"];
        osTable["execute"] =    luabind::object();
        osTable["exit"] =       luabind::object();
        //osTable["getenv"] =   object();
        osTable["remove"] =     luabind::object();
        osTable["rename"] =     luabind::object();
        osTable["setlocal"] =   luabind::object();
        osTable["tmpname"] =    luabind::object();
    }

    //Activate Luabind for out state
    try
    {
        luabind::open(L);
    }
    catch(const std::runtime_error &e)
    {
        fprintf(stderr, "Can't start LuaBind engine: %s\n", e.what());
        return 1;
    }

    luabind::module(L)[
        luabind::def("getSampleJson", &getSampleJson)
    ];

    luabind::object _G = luabind::globals(L);
    luabind::object package = _G["package"];
    package["path"] = ";" PATH_TO_LUA_DIR "/?.lua";

    int errorCode = luaL_loadfile(L, PATH_TO_TESTLUA);
    if(errorCode)
    {
        fprintf(stderr, "Got lua error, reporting... [load] %s\n\n%s\n", "test.lua", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    errorCode = lua_pcall(L, 0, 0, 0);
    if(errorCode)
    {
        fprintf(stderr, "Got lua error, reporting... [pcall] %s\n\n%s\n", "test.lua", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    luabind::call_function<void>(L, "test");

    lua_close(L);

    return 0;
}

