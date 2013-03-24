#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "cog_main.h"

static int cogl_init(lua_State* L) {
    cog_init();
    return 0;
}

static int cogl_mainloop(lua_State* L) {
    cog_mainloop();
    return 0;
}

static int cogl_quit(lua_State* L) {
    cog_quit();
    return 0;
}

int luaopen_libcogl(lua_State *L) {
    lua_register(L, "cogl_init", cogl_init);
    lua_register(L, "cogl_mainloop", cogl_mainloop);
    lua_register(L, "cogl_quit", cogl_quit);
    return 0;
}
