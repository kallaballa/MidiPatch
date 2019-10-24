TARGET  := farts

ifdef JAVASCRIPT
TARGET := farts.html
endif

SRCS  := farts.cpp RtAudio.cpp tonic_lua.cpp RtMidi.cpp PolySynth.cpp
CXXFLAGS += -I../third/kaguya/include -I../third/lua-5.3.5/src -I../third/Tonic/src -D__LINUX_ALSA__
LIBS += -ltonic -llua -ldl -lasound
LDFLAGS += -L../third/lua-5.3.5/src -L../third/Tonic/src

#precompiled headers
HEADERS := tonic_lua.hpp 

GCH     := ${HEADERS:.hpp=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 

.PHONY: all release debug clean distclean 

all: release
release: ${TARGET}
debug: ${TARGET}
info: ${TARGET}
profile: ${TARGET}
hardcore: ${TARGET}
asan: ${TARGET}

${TARGET}: ${OBJS} 
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

${GCH}: %.gch: ${HEADERS} 
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

install:
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp ${TARGET} ${DESTDIR}/${PREFIX}/bin

uninstall:
	rm ${DESTDIR}/${PREFIX}/bin/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${GCH} ${TARGET} 

distclean: clean



