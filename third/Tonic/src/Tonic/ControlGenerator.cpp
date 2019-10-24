//
//  ControlGenerator.cpp
//  TonicDemo
//
//  Created by Morgan Packard on 2/12/13.

//

#ifndef TonicDemo_ControlGenerator_cpp
#define TonicDemo_ControlGenerator_cpp

#include "ControlGenerator.h"
#include "RampedValue.h"
#include "ControlArithmetic.h"
#include "ControlChangeNotifier.h"
#include "ControlDbToLinear.h"
#include "ControlDelay.h"
#include "ControlFloor.h"
#include "ControlMetroDivider.h"
#include "ControlMidiToFreq.h"
#include "ControlPrinter.h"
#include "ControlRecorder.h"
#include "ControlSnapToScale.h"
#include <cmath>

namespace Tonic{
  namespace Tonic_{
    
    ControlGenerator_::ControlGenerator_() :
      lastFrameIndex_(0)
    {
    }

    ControlGenerator_::~ControlGenerator_()
    {
    }
    
    ControlGeneratorOutput ControlGenerator_::initialOutput(){
      // return output from passing in dummy context
      return tick(Tonic::DummyContext);
    }
    
  }
  RampedValue ControlGenerator::smoothed(float length){
    return RampedValue( obj->initialOutput().value, length ).target(*this);
  }
  
  ControlAdder ControlGenerator::operator+(ControlGenerator b){
     ControlAdder adder;
     adder.input(*this);
     adder.input(b);
     return adder;
   }

   ControlAdder ControlGenerator::operator+(float b){
     ControlAdder adder;
     adder.input(*this);
     adder.input(ControlValue(b));
     return adder;
   }

   ControlSubtractor ControlGenerator::operator-(ControlGenerator b){
     ControlSubtractor subtractor;
     subtractor.left(*this);
     subtractor.right(b);
     return subtractor;
   }

   ControlSubtractor ControlGenerator::operator-(float b){
     ControlSubtractor subtractor;
     subtractor.left(*this);
     subtractor.right(ControlValue(b));
     return subtractor;
   }

   ControlMultiplier ControlGenerator::operator*(ControlGenerator b){
     ControlMultiplier mult;
     mult.input(*this);
     mult.input(b);
     return mult;
   }

   ControlMultiplier ControlGenerator::operator*(float b){
     ControlMultiplier mult;
     mult.input(*this);
     mult.input(ControlValue(b));
     return mult;
   }


   ControlDivider ControlGenerator::operator/(ControlGenerator b){
     ControlDivider divider;
     divider.left(*this);
     divider.right(b);
     return divider;
   }

   ControlDivider ControlGenerator::operator/(float b){
     ControlDivider divider;
     divider.left(*this);
     divider.right(ControlValue(b));
     return divider;
   }

//		ControlChangeNotifier ControlGenerator::operator >>(ControlChangeNotifier rhs) {
//			return rhs.input(*this);
//		}
//		ControlDbToLinear ControlGenerator::operator >>(ControlDbToLinear rhs) {
//			return rhs.input(*this);
//		}
//		ControlDelay ControlGenerator::operator >>(ControlDelay rhs) {
//			return rhs.input(*this);
//		}
//		ControlFloor ControlGenerator::operator >>(ControlFloor rhs) {
//			return rhs.input(*this);
//		}
//		ControlMetroDivider ControlGenerator::operator >>(ControlMetroDivider rhs) {
//			return rhs.input(*this);
//		}
//		ControlMidiToFreq ControlGenerator::operator >>(ControlMidiToFreq rhs) {
//			return rhs.input(*this);
//		}
//		ControlPrinter ControlGenerator::operator >>(ControlPrinter rhs) {
//			return rhs.input(*this);
//		}
//		ControlRecorder ControlGenerator::operator >>(ControlRecorder rhs) {
//			return rhs.input(*this);
//		}
//		ControlSnapToScale ControlGenerator::operator >>(ControlSnapToScale rhs) {
//			return rhs.input(*this);
//		}
//		ControlChangeNotifier ControlGenerator::operator >>(ControlChangeNotifier rhs) {
//			return rhs.input(*this);
//		}

}

#endif
