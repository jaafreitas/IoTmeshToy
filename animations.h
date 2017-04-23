#ifndef   _ANIMATIONS_H_
#define   _ANIMATIONS_H_

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#define PIXEL_COUNT       16  // make sure to set this to the number of pixels in your strip
#define PIXEL_PIN         2  // make sure to set this to the correct pin, ignored for Esp8266
#define ANIMATION_COUNT   3

// Actually, it can be any number.
#define MAX_BLIPS         4

#define ledPinR D1 // pwm pin with red led
#define ledPinG D2 // pwm pin with green led
#define ledPinB D3 // pwm pin with blue led

struct AnimationController {
  float dimmer;  // controls overall brightness 0-1f
  float width; // width of the blip
  float hue[MAX_BLIPS]; // color of the blip
  float offset;  // relative offset of this blip
  bool  direction;
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

void placeBlip(float& blipPos, AnimationController* controller, uint8_t hueIdx);
void allDark( void );

#endif //   _ANIMATIONS_H_

