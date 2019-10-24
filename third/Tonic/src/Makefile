ifndef STATIC
ifeq ($(UNAME_S), Darwin)
TARGET := libtonic.dylib
else
TARGET  := libtonic.so
endif
else
TARGET := libtonic.a
endif

SRCS    := Tonic/ADSR.cpp Tonic/Arithmetic.cpp Tonic/AudioFileUtils.cpp Tonic/BasicDelay.cpp Tonic/BitCrusher.cpp Tonic/BLEPOscillator.cpp Tonic/BufferFiller.cpp Tonic/BufferPlayer.cpp Tonic/CombFilter.cpp Tonic/CompressorLimiter.cpp Tonic/ControlArithmetic.cpp Tonic/ControlCallback.cpp Tonic/ControlChangeNotifier.cpp Tonic/ControlComparison.cpp Tonic/ControlConditioner.cpp Tonic/ControlCounter.cpp Tonic/ControlDbToLinear.cpp Tonic/ControlDelay.cpp Tonic/ControlFloor.cpp Tonic/ControlGenerator.cpp Tonic/ControlMetro.cpp Tonic/ControlMetroDivider.cpp Tonic/ControlMidiToFreq.cpp Tonic/ControlParameter.cpp Tonic/ControlPrinter.cpp Tonic/ControlPulse.cpp Tonic/ControlRandom.cpp Tonic/ControlRecorder.cpp Tonic/ControlSnapToScale.cpp Tonic/ControlStepper.cpp Tonic/ControlSwitcher.cpp Tonic/ControlTrigger.cpp Tonic/ControlTriggerFilter.cpp Tonic/ControlValue.cpp Tonic/ControlXYSpeed.cpp Tonic/DelayUtils.cpp Tonic/DSPUtils.cpp Tonic/Effect.cpp Tonic/Filters.cpp Tonic/FilterUtils.cpp Tonic/FixedValue.cpp Tonic/Generator.cpp Tonic/LFNoise.cpp Tonic/Mixer.cpp Tonic/MonoToStereoPanner.cpp Tonic/Noise.cpp Tonic/RampedValue.cpp Tonic/RectWave.cpp Tonic/Reverb.cpp Tonic/RingBuffer.cpp Tonic/SampleTable.cpp Tonic/SawtoothWave.cpp Tonic/SineWave.cpp Tonic/StereoDelay.cpp Tonic/Synth.cpp Tonic/TableLookupOsc.cpp Tonic/TonicFrames.cpp


#precompiled headers
HEADERS := 
GCH    := ${HEADERS:.hpp=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 

CXXFLAGS += -fpic -I../ 
LDFLAGS += -shared 
.PHONY: all release debug clean distclean 

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

