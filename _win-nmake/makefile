prefix=..
libdir=$(prefix)\lib\windows
bindir=$(prefix)\bin\windows
srcdir=$(prefix)\src
exampledir=$(prefix)\example

CXX=cl
AR=lib
LD=link
FLAGS=/c /Ox /MD -Gm- /nologo /EHsc
LDFLAGS=/nologo# /SUBSYSTEM:WINDOWS
CXXFLAGS= $(FLAGS)

libname=ppkassert

SOURCES=$(srcdir)\pempek_assert.cpp

OBJECTS=$(SOURCES:.cpp=.obj)

all: build-lib build-example

build-lib: $(libdir)\$(libname).lib
$(srcdir)\pempek_assert.obj: $(srcdir)\pempek_assert.cpp
	$(CXX) $(CXXFLAGS) -I$(srcdir) $** /Fo$@
$(libdir):
	mkdir $(libdir)
$(libdir)\$(libname).lib: $(libdir) $(srcdir)\pempek_assert.obj
	$(AR) $(LDFLAGS) $(srcdir)\pempek_assert.obj  /OUT:$@


build-example: $(bindir)\example.exe
$(exampledir)\main.obj: $(exampledir)\main.cpp
	$(CXX) $(CXXFLAGS) -I$(srcdir) $** /Fo$@
$(bindir):
	mkdir $(bindir)
$(bindir)\example.exe: $(bindir) $(exampledir)\main.obj
	$(LD) $(LDFLAGS) /LIBPATH:$(libdir) $(exampledir)\main.obj $(libname).lib User32.lib /OUT:$@


clean:
	del $(srcdir)\*.obj $(exampledir)\*.obj
	rmdir /S /Q $(libdir) $(bindir)
