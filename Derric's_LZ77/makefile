DIALOG=lz77Dlg
APP=lz77App
LIBS=`wx-config --libs`
CXXFLAGS=`wx-config --cxxflags` 
OBJFILES=${DIALOG}.o ${APP}.o
EXEFILE=lz77compressor
all: $(EXEFILE)
$(EXEFILE):$(OBJFILES)
	g++ -o $(EXEFILE) ${OBJFILES} ${LIBS}
${DIALOG}.o: ${DIALOG}.cpp ${DIALOG}.h
	g++ -c ${DIALOG}.cpp ${CXXFLAGS}
${APP}.o: ${APP}.cpp ${APP}.h Compression.h
	g++ -c ${APP}.cpp ${CXXFLAGS}
clean:
	rm -f ${OBJFILES}

