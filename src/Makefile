UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 CUSTOM_DEFS = -Wno-register
 AUDIO_BACKEND=-D__MACOSX_CORE__
 MIDI_BACKEND=-D__MACOSX_CORE__
 PACK_CMD=nasm -fmacho64 index.s -o index.o
endif

ifeq ($(UNAME_S), Linux)
 ifeq (${PULSE}, 1)
   AUDIO_BACKEND = -D__LINUX_PULSE__
   MIDI_BACKEND = -D__LINUX_ALSA__
   LIBS += -lpulse -lpulse-simple -lasound
 else ifeq (${JACK}, 1)
   AUDIO_BACKEND = -D__UNIX_JACK__
   MIDI_BACKEND = -D__LINUX_ALSA__
   LIBS += -ljack -lasound
 else
   AUDIO_BACKEND = -D__LINUX_ALSA__
   MIDI_BACKEND = -D__LINUX_ALSA__
   LIBS+= -lasound
 endif
 PACK_CMD=${LD} -r -b binary -o index.o index.min.pack
 LIBS+= -lstdc++fs
endif

TARGET  := midipatch

ifdef JAVASCRIPT
TARGET := midipatch.html
endif

SRCS  := midipatch.cpp Websocket.cpp logger.cpp
CXXFLAGS += -I/usr/lib64/ -I../third/uSockets-0.3.2/src/ -I../third/uWebSockets-0.16.1/src -I../PatchScript/src/ -I../PatchScript/third/SQLiteCPP -I../PatchScript/third/kaguya/include -I../PatchScript/third/lua-5.3.5/src -I../PatchScript/third/Tonic/src -I../third/cxxopts-2.2.0/include -I../third/nlohmann-json-3.7.0/ -I../third/RtMidi -I../third/RtAudio -I../third/ ${CUSTOM_DEFS}
LIBS += -ltonic -llua -ldl -llilv-0 -luSockets -lsqlite3 -lz -lpatchscript 
LDFLAGS += -L../PatchScript/third/lua-5.3.5/src -L../PatchScript/third/Tonic/src -L../third/uSockets-0.3.2/ -L../PatchScript/src/
ASSET_DIRS = $(shell find ../html/ -type d -not -name ".." -not -name "html")
ASSET_FILES = $(shell find ../html/ -type f -not -name index.o -not -name index.min.pack)

#precompiled headers
HEADERS :=

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

${TARGET}: ../html/index.o ../third/RtMidi/RtMidi.o ../third/RtAudio/RtAudio.o ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

../third/RtMidi/RtMidi.o: ../third/RtMidi/RtMidi.cpp
	${CXX} ${CXXFLAGS} ${MIDI_BACKEND} -o $@ -c $<

../third/RtAudio/RtAudio.o: ../third/RtAudio/RtAudio.cpp
	${CXX} ${CXXFLAGS} ${AUDIO_BACKEND} -o $@ -c $<

../html/index.o: ../html/index.min.pack
	cd ../html; ${PACK_CMD}

../html/index.min.pack: $(ASSET_DIRS) $(ASSET_FILES)
	cd ../html; inliner -n -m index.html > index.min.pack

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
	rm -f *~ ${DEPS} ${OBJS} ${GCH} ${TARGET} ../html/index.o ../html/index.min.pack ../third/RtMidi/RtMidi.o ../third/RtAudio/RtAudio.o

distclean: clean



