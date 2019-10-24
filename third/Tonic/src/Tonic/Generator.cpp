//
//  Generator.cpp
//  Tonic
//
//  Created by Morgan Packard on 1/23/13.
//
//
// See LICENSE.txt for license and usage information.
//


#include "Generator.h"
#include "Arithmetic.h"


namespace Tonic{ namespace Tonic_{
  
  Generator_::Generator_() : lastFrameIndex_(0), isStereoOutput_(false){
    outputFrames_.resize(kSynthesisBlockSize, 1, 0);
  }
  
  Generator_::~Generator_() {}
  
  void Generator_::setIsStereoOutput(bool stereo){
    if (stereo != isStereoOutput_){
      outputFrames_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
    }
    isStereoOutput_ = stereo;
  }

}

Adder Generator::operator + (Generator b){
     Adder add;
     add.input(*this);
     add.input(b);
     return add;
   }

   Adder Generator::operator + (float b){
     Adder add;
     add.input(*this);
     add.input(FixedValue(b));
     return add;
   }

   Adder Generator::operator + (ControlGenerator b){
     return (*this) + FixedValue().setValue(b);
   }

   Subtractor Generator::operator - (Generator b){
     Subtractor sub;
     sub.left(*this);
     sub.right(b);
     return sub;
   }

   Subtractor Generator::operator - (float b){
     Subtractor sub;
     sub.left(*this);
     sub.right(FixedValue(b));
     return sub;
   }

   Subtractor Generator::operator - (ControlGenerator b){
     return (*this) - FixedValue().setValue(b);
   }

   Multiplier Generator::operator*(Generator b){
     Multiplier mult;
     mult.input((*this));
     mult.input(b);
     return mult;
   }

   Multiplier Generator::operator*(float b){
     return (*this) * FixedValue(b);
   }

   Multiplier Generator::operator * (ControlGenerator b){
     return (*this) * FixedValue().setValue(b);
   }

   Divider Generator::operator / (Generator b){
     Divider div;
     div.left(*this);
     div.right(b);
     return div;
   }

   Divider Generator::operator / (float b){
     Divider div;
     div.left(*this);
     div.right(FixedValue(b));
     return div;
   }

   // Subtract a Generatator and a ControlGenerator

   Divider Generator::operator / (ControlGenerator b){
     return (*this) / FixedValue().setValue(b);
   }

}
