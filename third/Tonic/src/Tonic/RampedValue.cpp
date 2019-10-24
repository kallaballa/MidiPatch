//
//  RampedValue.cpp
//  Tonic
//
//  Created by Nick Donaldson on 2/7/13.

//

#include "RampedValue.h"
#include "Arithmetic.h"

namespace Tonic { namespace Tonic_{
  
  RampedValue_::RampedValue_() :
    finished_(true),
    count_(0),
    len_(0),
    target_(0),
    last_(0),
    inc_(0)
  {
  }
  
  RampedValue_::~RampedValue_(){
    
  }
  
  
  bool RampedValue_::isFinished(){
    return finished_;
  }


} // Namespace Tonic_
  
Adder RampedValue::operator+(Generator b){
   Adder adder;
   adder.input(*this);
   adder.input(b);
   return adder;
 }

 Adder RampedValue::operator+(float b){
   Adder adder;
   adder.input(*this);
   adder.input(FixedValue(b));
   return adder;
 }
 Adder RampedValue::operator + (ControlGenerator b){
   return (*this) + FixedValue().setValue(b);
 }

 Subtractor RampedValue::operator-(Generator b){
	 Subtractor subtractor;
   subtractor.left(*this);
   subtractor.right(b);
   return subtractor;
 }

 Subtractor RampedValue::operator-(float b){
   Subtractor subtractor;
   subtractor.left(*this);
   subtractor.right(FixedValue(b));
   return subtractor;
 }

 Subtractor RampedValue::operator - (ControlGenerator b){
   return (*this) - FixedValue().setValue(b);
 }

 Multiplier RampedValue::operator*(Generator b){
   Multiplier mult;
   mult.input(*this);
   mult.input(b);
   return mult;
 }

 Multiplier RampedValue::operator*(float b){
   Multiplier mult;
   mult.input(*this);
   mult.input(FixedValue(b));
   return mult;
 }

 Multiplier RampedValue::operator* (ControlGenerator b){
   return (*this) * FixedValue().setValue(b);
 }

 Divider RampedValue::operator/(Generator b){
   Divider divider;
   divider.left(*this);
   divider.right(b);
   return divider;
 }

 Divider RampedValue::operator/(float b){
   Divider divider;
   divider.left(*this);
   divider.right(FixedValue(b));
   return divider;
 }

 Divider RampedValue::operator / (ControlGenerator b){
   return (*this) / FixedValue().setValue(b);
 }

  bool RampedValue::isFinished(){
    return gen()->isFinished();
  }
  
} // Namespace Tonic
