ifdef COMSPEC
CC = cl.exe
LINKER = link.exe
CFLAGS = /c /EHsc /nologo /Wall /I include /Fo
LFLAGS = /LIBPATH:lib /OUT:
CLEARER = del
SRCDIR = "src\\"
BUILDDIR = "build\\"
BINDIR = "bin\\"
TESTBUILDDIR = "test\\build\\"
TESTBINDIR = "test\\bin\\"
else
CC = g++
LINKER = g++
CFLAGS = -c -g -Wall -Iinclude -std=c++11 -o 
CLEARER = rm -rf
LFLAGS = -Llib -lglfw -lglad -lpthread -ldl -lassimp -o 
MDFLAGS = -p
SRCDIR = src/
BUILDDIR = build/
BINDIR = bin/
TESTSRCDIR = test/src/
TESTBUILDDIR = test/build/
TESTBINDIR = test/bin/
.PHONY: clean
	
endif

SRC = $(wildcard $(SRCDIR)*.cpp)
OBJS = $(SRC:$(SRCDIR)%.cpp=$(BUILDDIR)%.o)
TESTSRC = $(wildcard $(TESTSRCDIR)*.cpp)
TESTOBJS = $(TESTSRC:$(TESTSRCDIR)%.cpp=$(TESTBUILDDIR)%.o)
TESTEXECS = $(TESTOBJS:$(TESTBUILDDIR)%.o=$(TESTBINDIR)%)

default: $(TESTEXECS)

$(TESTEXECS): $(TESTBINDIR)%: $(TESTBUILDDIR)%.o $(OBJS) $(TESTBINDIR)
	$(LINKER) $(OBJS) $< $(LFLAGS)$@

$(TESTOBJS): $(TESTBUILDDIR)%.o: $(TESTSRCDIR)%.cpp $(TESTBUILDDIR)
	$(CC) $< $(CFLAGS)$@

$(OBJS): $(BUILDDIR)%.o: $(SRCDIR)%.cpp $(BUILDIR)
	$(CC) $< $(CFLAGS)$@

$(BINDIR):
	mkdir $(MDFLAGS) $(BINDIR)

$(BUILDIR):
	mkdir $(MDFLAGS) $(BUILDDIR)

$(TESTBINDIR):
	mkdir $(MDFLAGS) $(TESTBINDIR)

$(TESTBUILDDIR):
	mkdir $(MDFLAGS) $(TESTBUILDDIR)

clean:
	$(CLEARER) build/*
	$(CLEARER) bin/*
	$(CLEARER) test/build/*
	$(CLEARER) test/bin/*

rebuild: clean default
