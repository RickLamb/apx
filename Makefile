### Build flags for all targets
#
CF_ALL          = -g -Wall
LF_ALL          =
LL_ALL          =

### Build tools
#
CC              = cl
COMP            = $(CC) $(CF_ALL) $(CF_TGT) -o $@ -c $<
LINK            = $(CC) $(LF_ALL) $(LF_TGT) -o $@ $^ $(LL_TGT) $(LL_ALL)
COMPLINK        = $(CC) $(CF_ALL) $(CF_TGT) $(LF_ALL) $(LF_TGT) -o $@ $< $(LL_TGT) $(LL_ALL)

### Base Env Vars
#
BASE_OBJ        = "./obj"
BASE_BIN        = "./bin"

### Standard parts
#
include ./build/Rules.mk
