#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"


#include "ngx_http_graphite_module.h"
#include "ngx_http_lua_graphite.h"
#include "ngx_http_lua_util.h"


static int ngx_http_lua_graphite(lua_State *L);
static int ngx_http_lua_graphite_get(lua_State *L);
static int ngx_http_lua_graphite_set(lua_State *L);


void
ngx_http_lua_inject_graphite_api(lua_State *L)
{
    lua_createtable(L, 0, 2);
    lua_newtable(L);
    lua_pushcfunction(L, ngx_http_lua_graphite);
    lua_setfield(L, -2, "__call");
    lua_setmetatable(L, -2);

    lua_pushcfunction(L, ngx_http_lua_graphite_get);
    lua_setfield(L, -2, "get");

    lua_pushcfunction(L, ngx_http_lua_graphite_set);
    lua_setfield(L, -2, "set");

    lua_setfield(L, -2, "graphite");
}


static int
ngx_http_lua_graphite(lua_State *L) {

    size_t n = lua_gettop(L) - 1;
    if (n != 2 && n != 3) {
        return luaL_error(L, "ngx.graphite expecting 2 or 3 arguments got %d", n);
    }

    ngx_http_request_t *r;
    r = ngx_http_lua_get_req(L);

    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    ngx_str_t name;
    name.data = (u_char*)lua_tolstring(L, 2, &name.len);
    if (name.data == NULL)
        return 0;

    double value = lua_tonumber(L, 3);

    ngx_http_graphite(r, &name, value);

    return 0;
}


static int
ngx_http_lua_graphite_get(lua_State *L) {

    size_t n = lua_gettop(L);
    if (n != 1) {
        return luaL_error(L, "ngx.graphite.get expecting 1 argument got %d", n);
    }

    ngx_http_request_t *r;
    r = ngx_http_lua_get_req(L);

    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    ngx_str_t name;
    name.data = (u_char*)lua_tolstring(L, 1, &name.len);
    if (name.data == NULL)
        return 0;

    double value = ngx_http_graphite_get(r, &name);

    lua_pushnumber(L, value);

    return 1;
}


static int
ngx_http_lua_graphite_set(lua_State *L) {

    size_t n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "ngx.graphite.get expecting 2 arguments got %d", n);
    }

    ngx_http_request_t *r;
    r = ngx_http_lua_get_req(L);

    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    ngx_str_t name;
    name.data = (u_char*)lua_tolstring(L, 1, &name.len);
    if (name.data == NULL)
        return 0;

    double value = lua_tonumber(L, 2);

    ngx_http_graphite_set(r, &name, value);

    return 0;
}