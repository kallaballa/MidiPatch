UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 CUSTOM_DEFS = -D__MACOSX_CORE__
endif

ifeq ($(UNAME_S), Linux)
 CUSTOM_DEFS = -D__LINUX_ALSA__
 LIBS+= -lasound
endif

TARGET  := farts

ifdef JAVASCRIPT
TARGET := farts.html
endif

SRCS  := farts.cpp tonic_lua0.cpp tonic_lua1.cpp tonic_lua2.cpp PolySynth.cpp ../third/RtMidi/RtMidi.cpp ../third/RtAudio/RtAudio.cpp lvplugin.cpp LCD.cpp Websocket.cpp UDP.cpp
CXXFLAGS += -I/usr/lib64/ -I../third/uSockets-0.3.2/src/ -I../third/uWebSockets-0.16.1/src -I../third/kaguya/include -I../third/lua-5.3.5/src -I../third/Tonic/src -I../third/cxxopts-2.2.0/include -I../third/nlohmann-json-3.7.0/ -I../third/RtMidi -I../third/RtAudio ${CUSTOM_DEFS}
LIBS += -ltonic -llua -ldl -llilv-0 -luSockets -lz
LDFLAGS += -L../third/lua-5.3.5/src -L../third/Tonic/src -L../third/uSockets-0.3.2/
ASSET_DIRS = $(shell find ../html/ -type d)
ASSET_FILES = $(shell find ../html/ -type f -name '*')

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

${TARGET}: ../html/index.o ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

../html/index.o: ../html/index.min.pack
	cd ../html; ld -r -b binary -o index.o index.min.pack

../html/index.min.pack: $(ASSET_DIRS) $(ASSET_FILES)
	cd ../html; inliner -m index.html > index.min.pack

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
	rm -f *~ ${DEPS} ${OBJS} ${GCH} ${TARGET} ../html/index.o ../html/index.min.pack

distclean: clean



