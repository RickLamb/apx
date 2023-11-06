
OBJDIR := $(BASE_OBJ)/main
BINDIR := $(BASE_BIN)/
CPPFLAGS  = /I ./thirdparty/cppfront/include/ /Fo$(OBJDIR)/ /Z7 -std:c++20 -EHsc
LFLAGS = /Fe$(BINDIR)

TGT_BIN +=  main ./src/pure2-hello.cpp ./src/mycomponent.cpp

$(OBJDIR):
	@IF NOT EXIST $(subst /,\,$(OBJDIR)) mkdir $(subst /,\,$(OBJDIR)) >NUL  2>NUL
	@IF NOT EXIST $(subst /,\,$(BINDIR)) mkdir $(subst /,\,$(BINDIR)) >NUL  2>NUL

./src/pure2-hello.cpp: ./src/pure2-hello.cpp2 cppfront
	./bin/cppfront ./src/pure2-hello.cpp2

./src/mycomponent.h: ./src/mycomponent.h2 cppfront
	./bin/cppfront ./src/mycomponent.h2

./src/mycomponent.cpp: ./src/mycomponent.cpp2 cppfront
	./bin/cppfront ./src/mycomponent.cpp2

main: ./src/main.cpp ./src/pure2-hello.cpp ./src/mycomponent.cpp | $(OBJDIR)
	cl ./src/main.cpp ./src/pure2-hello.cpp ./src/mycomponent.cpp $(CPPFLAGS) $(LFLAGS)


