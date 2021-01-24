// forward declarations of pattern functions

#ifndef _CL_PATTERNS_FWD_DEC_H_
#define _CL_PATTERNS_FWD_DEC_H_

#include <FastLED.h>

void mirrorFFT(uint32_t now, void* arg);
void centerFFT(uint32_t now, void* arg);
void diamond(uint32_t now, void* arg);
void diagonal(uint32_t now, void* arg);
void audioBuffer(uint32_t now, void* arg);
void centerAudioBuffer(uint32_t now, void* arg);
void linearFade(uint32_t now, void* arg);
void gradient(uint32_t now, void* arg);
void momentaryAudioRamp(uint32_t now, void* arg);
void audioJump(uint32_t now, void* arg);
void sparkle(uint32_t now, void* arg);
void solidColor(uint32_t now, void* arg);

void colorSet(CRGB color);
void resetColorIndex();

#endif // _CL_PATTERNS_FWD_DEC_H_
