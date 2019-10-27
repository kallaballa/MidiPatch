#include "lvplugin.hpp"


namespace Tonic {

  // -----------------------------------------
  //                 ADDER
  // -----------------------------------------

  namespace Tonic_{
  using namespace Tonic_;

  void LV2Effect_::setInput( Generator input ) {
    input_ = input;
    setIsStereoInput(input_.isStereoOutput());
  }

  void LV2Effect_::setIsStereoInput( bool stereo ) {
    if (stereo != isStereoInput_){
      dryFrames_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
      outputFrames_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
    }
    isStereoInput_ = stereo;
    isStereoOutput_ = stereo;
  }
}}
