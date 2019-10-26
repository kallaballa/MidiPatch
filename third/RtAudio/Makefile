ifndef STATIC
ifeq ($(UNAME_S), Darwin)
TARGET := librtaudio.dylib
else
TARGET  := librtaudio.so
endif
else
TARGET := librtrtaudio.a
endif

SRCS    := RtAudio.cpp


#precompiled headers
HEADERS := 
GCH    := ${HEADERS:.hpp=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 

CXXFLAGS += -fpic -I../ -D__LINUX_PULSE__
LDFLAGS += -shared 
.PHONY: all release debug clean distclean 

ifdef SYSROOT
CXXFLAGS += --sysroot=${SYSROOT}
endif

all: release
release: ${TARGET}
debug: ${TARGET}
info: ${TARGET}
profile: ${TARGET}
hardcore: ${TARGET}

${TARGET}: ${OBJS}
ifndef STATIC
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}
else
	ar -rcs $@ $^
endif


${OBJS}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

${GCH}: %.gch: ${HEADERS} 
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

install:
	mkdir -p ${DESTDIR}/${PREFIX}
	cp ${TARGET} ${DESTDIR}/${PREFIX}

uninstall:
	rm ${DESTDIR}/${PREFIX}/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${CUO} ${GCH} ${TARGET} 

distclean: clean

