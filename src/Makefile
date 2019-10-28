UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 CUSTOM_DEFS = -D__MACOSX_CORE__
endif

ifeq ($(UNAME_S), Linux)
 CUSTOM_DEFS = -D__LINUX_ALSA__
endif

TARGET  := farts

ifdef JAVASCRIPT
TARGET := farts.html
endif

SRCS  := farts.cpp tonic_lua0.cpp tonic_lua1.cpp tonic_lua2.cpp PolySynth.cpp ../third/RtMidi/RtMidi.cpp ../third/RtAudio/RtAudio.cpp lvplugin.cpp
CXXFLAGS += -I../third/kaguya/include -I../third/lua-5.3.5/src -I../third/Tonic/src -I../third/RtMidi -I../third/RtAudio ${CUSTOM_DEFS}
LIBS += -ltonic -llua -ldl -lboost_program_options -llilv-0
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



