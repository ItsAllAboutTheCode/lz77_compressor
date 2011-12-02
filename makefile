FRAME:=lz77Frame
APP:=lz77App
COMPRESS:=lz77Type10 lz77Type11 lzBase
HEADERS:=$(addsuffix .h, $(basename $(COMPRESS)))
SRC:=$(FRAME) $(APP) $(COMPRESS) 
LIBS:=`wx-config --libs`
CXXFLAGS:=-std=c++0x `wx-config --cxxflags`
#CXXFLAGS:=-m32 -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_LARGE_FILES -D__WXGTK__ -pthread
OBJFILES:=$(addsuffix .o, $(basename $(SRC)))
EXEFILE:=lz77compressor

all: $(EXEFILE)
$(EXEFILE):$(OBJFILES)
	g++ $(LIBS) -o $(EXEFILE) $(OBJFILES) 
$(FRAME).o: $(FRAME).cpp $(FRAME).h $(HEADERS)
	g++ $(CXXFLAGS) -g -Wall -o $@ -c $<
$(APP).o: $(APP).cpp $(FRAME).h 
	g++ $(CXXFLAGS) -g -Wall -o $@ -c $<
lz77Type10.o: lz77Type10.cpp lz77Type10.h lzBase.h
	g++ $(CXXFLAGS) -g -Wall -o $@ -c $<
lz77Type11.o: lz77Type11.cpp lz77Type11.h lzBase.h
	g++ $(CXXFLAGS) -g -Wall -o $@ -c $<
lzBase.o: lzBase.cpp lzBase.h
	g++ $(CXXFLAGS) -g -Wall -o $@ -c $<
clean:
	rm -f $(OBJFILES)

