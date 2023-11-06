
OBJDIR := $(BASE_OBJ)/cppfront/
BINDIR := $(BASE_BIN)/
CPPFLAGS  = /Fo$(OBJDIR) -std:c++20 -EHsc
LFLAGS = /Fe$(BINDIR)

TGT_BIN +=  cppfront

$(OBJDIR):
	@IF NOT EXIST $(subst /,\,$(OBJDIR)) mkdir $(subst /,\,$(OBJDIR)) >NUL  2>NUL
	@IF NOT EXIST $(subst /,\,$(BINDIR)) mkdir $(subst /,\,$(BINDIR)) >NUL  2>NUL

./thirdparty/cppfront/source/reflect.h: ./thirdparty/cppfront/source/reflect.h2
	./bin/cppfront ./thirdparty/cppfront/source/reflect.h2

cppfront: ./thirdparty/cppfront/source/cppfront.cpp ./thirdparty/cppfront/source/reflect.h | $(OBJDIR)
	cl ./thirdparty/cppfront/source/cppfront.cpp $(CPPFLAGS) $(LFLAGS)

  

