// forward declarations of pattern functions

#ifndef _CL_PATTERNS_FWD_DEC_H_
#define _CL_PATTERNS_FWD_DEC_H_

#include <FastLED.h>

typedef void(*ani_frame_fn)(uint32_t now, void* arg);
typedef struct _animation_t {
  volatile ani_frame_fn get_frame;
  volatile void* arg;
} animation_t;

void mirrorFFT(uint32_t now, void* arg);
void centerFFT(uint32_t now, void* arg);
void diamond(uint32_t now, void* arg);
void diagonal(uint32_t now, void* arg);
void audioBuffer(uint32_t now, void* arg);
void centerAudioBuffer(uint32_t now, void* arg);
void rightToLeftFade(uint32_t now, void* arg);
void leftToRightFade(uint32_t now, void* arg);
void bottomToTopFade(uint32_t now, void* arg);
void topToBottomFade(uint32_t now, void* arg);
void gradient(uint32_t now, void* arg);
void momentaryAudioRamp(uint32_t now, void* arg);
void audioJump(uint32_t now, void* arg);
void sparkle(uint32_t now, void* arg);
void solidColor(uint32_t now, void* arg);

void colorSet(CRGB color);
void resetColorIndex();

#endif // _CL_PATTERNS_FWD_DEC_H_
