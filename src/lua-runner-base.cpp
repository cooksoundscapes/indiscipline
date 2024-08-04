#include "lua-runner-base.h"

int LuaRunnerBase::getTableIntValue(lua_State* l, int t_index, const char* k) {
    lua_pushstring(l, k);
    lua_gettable(l, t_index);
    int v = luaL_checknumber(l, -1);
    lua_pop(l, 1);
    return v;
}

bool LuaRunnerBase::getTableBoolValue(lua_State* l, int t_index, const char* k) {
    lua_pushstring(l, k);
    lua_gettable(l, t_index);
    bool v = lua_toboolean(l, -1);
    lua_pop(l, 1);
    return v;
}

void LuaRunnerBase::callFunctionOnStack(lua_State* l, int index, int argc, int n_result) {
    if (!lua_isfunction(l, index)) {
        lua_pushstring(l, "Expected a function");
        lua_error(l);
    }
    if (lua_pcall(l, argc, n_result, 0) != LUA_OK) {
        const char *error_message = lua_tostring(l, -1);
        lua_pushstring(l, error_message);
        lua_error(l);
    }
}

void LuaRunnerBase::callTableFunction(
    lua_State* l,
    int table_index,
    const char* fn_name,
    int argc,
    int n_result
) {
    // Preserve the current stack top
    int stack_top = lua_gettop(l);

    luaL_checktype(l, table_index, LUA_TTABLE);

    lua_pushstring(l, fn_name);
    lua_gettable(l, table_index);

    // Push the table itself as the first argument (self)
    lua_pushvalue(l, table_index);
    argc++;

    callFunctionOnStack(l, -(argc+1), argc, n_result);

    lua_settop(l, stack_top + n_result);
}

void LuaRunnerBase::printTypeAtIndex(lua_State* l, int i) {
    std::cout << "[Lua]: '" << luaL_typename(l, i) << "' found at index " << i << ";\n";
}

void LuaRunnerBase::printStackSize(lua_State* l) {
    std::cout << "Stack size is " << lua_gettop(l) << '\n';
}