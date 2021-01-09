// forward declarations of pattern functions

#ifndef _CL_PATTERNS_FWD_DEC_H_
#define _CL_PATTERNS_FWD_DEC_H_

#include <FastLED.h>

void mirrorFFT();
void centerFFT();
void diamond();
void diagonal(uint8_t corner);
void audioBuffer();
void centerAudioBuffer();
void rightToLeftFade();
void leftToRightFade();
void bottomToTopFade();
void topToBottomFade();
void gradient();
void momentaryAudioRamp();
void audioJump();
void sparkle();
void colorSet(CRGB color);
void resetColorIndex();

#endif // _CL_PATTERNS_FWD_DEC_H_
