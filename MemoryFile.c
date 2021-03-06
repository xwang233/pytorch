#include "general.h"

static const void* torch_MemoryFile_id;
static const void* torch_CharStorage_id;

static int torch_MemoryFile_new(lua_State *L)
{
  const char *mode;
  THCharStorage *storage = luaT_toudata(L, 1, torch_CharStorage_id);
  THFile *self;

  if(storage)
  {
    mode = luaL_optstring(L, 2, "rw");
    self = THMemoryFile_newWithStorage(storage, mode);
  }
  else
  {
    mode = luaL_optstring(L, 1, "rw");    
    self = THMemoryFile_new(mode);
  }

  luaT_pushudata(L, self, torch_MemoryFile_id);
  return 1;
}

static int torch_MemoryFile_storage(lua_State *L)
{
  THFile *self = luaT_checkudata(L, 1, torch_MemoryFile_id);
  THCharStorage_retain(THMemoryFile_storage(self));
  luaT_pushudata(L, THMemoryFile_storage(self), torch_CharStorage_id);
  return 1;
}

static int torch_MemoryFile_free(lua_State *L)
{
  THFile *self = luaT_checkudata(L, 1, torch_MemoryFile_id);
  THFile_free(self);
  return 0;
}

static int torch_MemoryFile___tostring__(lua_State *L)
{
  THFile *self = luaT_checkudata(L, 1, torch_MemoryFile_id);
  lua_pushfstring(L, "torch.MemoryFile [status: %s -- mode: %c%c]",
                  (THFile_isOpened(self) ? "open" : "closed"),
                  (THFile_isReadable(self) ? 'r' : ' '),
                  (THFile_isWritable(self) ? 'w' : ' '));
  return 1;
}

static const struct luaL_Reg torch_MemoryFile__ [] = {
  {"storage", torch_MemoryFile_storage},
  {"__tostring__", torch_MemoryFile___tostring__},
  {NULL, NULL}
};

void torch_MemoryFile_init(lua_State *L)
{
  torch_CharStorage_id = luaT_checktypename2id(L, "torch.CharStorage");

  torch_MemoryFile_id = luaT_newmetatable(L, "torch.MemoryFile", "torch.File",
                                          torch_MemoryFile_new, torch_MemoryFile_free, NULL);

  luaL_register(L, NULL, torch_MemoryFile__);
  lua_pop(L, 1);
}
