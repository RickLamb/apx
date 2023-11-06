
# Standard stuff

.SUFFIXES:
.SUFFIXES:	.cpp .cpp2 .o

all:		targets

# The variables TGT_*, CLEAN and CMD_INST* may be added to by the Makefile
# fragments in the various subdirectories.
TGT_BIN          =
TGT_LIB          =


# Subdirectories, in random order

dir	:= thirdparty/cppfront
include		$(dir)/Rules.mk
dir	:= src
include		$(dir)/Rules.mk

.PHONY:		targets
targets:	$(TGT_BIN) $(TGT_LIB)

