UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 BOOST_MT=-mt
endif

CXX      := g++
CXXFLAGS := -pthread -fno-strict-aliasing -std=c++17 -pedantic -Wall
LDFLAGS  := -L/opt/local/lib
LIBS     := -lpthread -lm 
.PHONY: all release debian-release info debug clean debian-clean distclean asan
DESTDIR := /
PREFIX := /usr/local
MACHINE := $(shell uname -m)

ifeq ($(UNAME_S), Darwin)
 CXX=clang++
 LD=clang++
 CFLAGS += -mmacosx-version-min=1
 LDFLAGS += -L/opt/local/lib # MacPorts Boost doesn't come with pkgconfig
 CXXFLAGS += -stdlib=libc++  -mmacosx-version-min=10.15 -lstdc++fs
 LDFLAGS += -stdlib=libc++ -lstdc++fs -framework CoreAudio -framework AudioToolbox -framework CoreMIDI -framework CoreData -framework CoreFoundation -framework Accelerate -undefined dynamic_lookup
endif

ifeq ($(MACHINE), x86_64)
  LIBDIR = lib64
endif
ifeq ($(MACHINE), i686)
  LIBDIR = lib
endif

ifdef SYSROOT
CXXFLAGS += --sysroot=${SYSROOT}
endif
ifdef JAVASCRIPT
CXX			 := em++
CXXFLAGS += -I/usr/local/include
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -m32 
endif

ifdef STATIC
LDFLAGS += -static-libgcc -Wl,-Bstatic
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -static-libgcc -m32 -Wl,-Bstatic
endif 

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/
endif

all: release

ifneq ($(UNAME_S), Darwin)
release: LDFLAGS += -s
endif
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O0
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic -D__MIDIPATCH_DEBUG
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

asan: CXXFLAGS += -g3 -O0 -rdynamic -fno-omit-frame-pointer -fsanitize=address -D__MIDIPATCH_DEBUG
asan: LDFLAGS += -Wl,--export-dynamic -fsanitize=address
asan: LIBS+= -lbfd -ldw
asan: dirs

profile: CXXFLAGS += -g3 -O1
profile: LDFLAGS += -Wl,--export-dynamic -rdynamic
profile: dirs

hardcore: CXXFLAGS += -g0 -Ofast -DNDEBUG

ifeq ($(UNAME_S), Darwin)
hardcore: LDFLAGS += -s
endif
hardcore: dirs

clean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS
export DESTDIR
export PREFIX

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}

debian-release:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" release

debian-clean:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" clean

debian-install: ${TARGET}
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" install

install: ${TARGET}
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}"  install

distclean:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" distclean

