prefix=..
libdir=$(prefix)\lib\windows
bindir=$(prefix)\bin\windows
srcdir=$(prefix)\src
exampledir=$(prefix)\example

CXX=cl
LD=link
FLAGS=/c /Ox /MD -Gm- /nologo /EHsc
LDFLAGS=/nologo
CXXFLAGS= $(FLAGS)

libname=ppkassert

SOURCES=$(srcdir)\pempek_assert.cpp

OBJECTS=$(SOURCES:.cpp=.obj)

all: build-lib build-example

build-lib: $(libdir)\$(libname).dll
$(srcdir)\pempek_assert.obj: $(srcdir)\pempek_assert.cpp
	$(CXX) $(CXXFLAGS) /DBUILDING_PPKASSERT /DUSING_DLL  /I$(srcdir) $** /Fo$@
$(libdir):
	mkdir $(libdir)
$(libdir)\$(libname).dll: $(libdir) $(srcdir)\pempek_assert.obj
	$(LD) $(LDFLAGS) /DLL $(srcdir)\pempek_assert.obj User32.lib /OUT:$@ /IMPLIB:$(libdir)\$(libname).lib


build-example: $(bindir)\example_shared.exe
$(exampledir)\main.obj: $(exampledir)\main.cpp
	$(CXX) $(CXXFLAGS) /DUSING_DLL /I$(srcdir) $** /Fo$@
$(bindir):
	mkdir $(bindir)
$(bindir)\example_shared.exe: $(bindir) $(exampledir)\main.obj
	$(LD) $(LDFLAGS) /LIBPATH:$(libdir) $(exampledir)\main.obj $(libname).lib /OUT:$@


clean:
	del $(srcdir)\*.obj $(exampledir)\*.obj
	rmdir /S /Q $(libdir) $(bindir)
