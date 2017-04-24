#ifndef   _ANIMATIONS_H_
#define   _ANIMATIONS_H_

#include <NeoPixelAnimator.h>

#define ANIMATION_COUNT   3

// Actually, it can be any number.
#define MAX_BLIPS         4

#define ledPinR D1 // pwm pin with red led
#define ledPinG D2 // pwm pin with green led
#define ledPinB D3 // pwm pin with blue led

struct AnimationController {
  float dimmer;  // controls overall brightness 0-1f
  float hue[MAX_BLIPS]; // color of the blip
  uint16_t nextAnimation;
};

enum animationNames {
  turnOnIdx = 0,
  searchingIdx = 1,
  smoothIdx = 2
};

void animationsInit();

void turnOn(const AnimationParam& param);
void searchingBlip(const AnimationParam& param);
void smoothBlip(const AnimationParam& param);

#endif //   _ANIMATIONS_H_

