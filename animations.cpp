#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <easyMesh.h>
#include <easyWebSocket.h>

#include "animations.h"

NeoPixelAnimator animations(ANIMATION_COUNT); // NeoPixel animation management object
AnimationController controllers[ANIMATION_COUNT];

extern easyMesh mesh;

void RGBSetPixelColor(RgbColor color) {
  // Common anode RGB LED.
  analogWrite(ledPinR, map(color.R, 0, 255, 255, 0));
  analogWrite(ledPinG, map(color.G, 0, 255, 255, 0));
  analogWrite(ledPinB, map(color.B, 0, 255, 255, 0));
}

void animationsInit() {
  animations.StartAnimation(turnOnIdx, 2000, turnOn);
  controllers[turnOnIdx].dimmer = 0.5f;  // controls overall brightness 0-0.5f
  controllers[turnOnIdx].nextAnimation = searchingIdx;  // run once and then run search

  animations.StartAnimation(searchingIdx, 100, searchingBlip);
  animations.StopAnimation(searchingIdx);
  controllers[searchingIdx].dimmer = 0.1f;  // controls overall brightness 0-0.5f
  controllers[searchingIdx].hue[0] = 0.0f; // color of the blip
  controllers[searchingIdx].nextAnimation = searchingIdx;  // run itself over and over until this changes.

  animations.StartAnimation(smoothIdx, 2000, smoothBlip);
  animations.StopAnimation(smoothIdx);
  controllers[smoothIdx].dimmer = 0.5f;  // controls overall brightness 0-0.5f

  // Initial color for up to 4 nodes.
  controllers[smoothIdx].hue[0] = 0.00f;
  controllers[smoothIdx].hue[1] = 0.65f;
  controllers[smoothIdx].hue[2] = 0.35f;
  controllers[smoothIdx].hue[3] = 0.15f;

  controllers[smoothIdx].nextAnimation = smoothIdx;  // run itself over and over until this changes.
}

void turnOn(const AnimationParam& param) {
  if (param.state == AnimationState_Completed) { // animation finished, restart
    animations.RestartAnimation(controllers[param.index].nextAnimation);
  }

  // 2 animation rounds every one second.
  float hue = 2 * param.progress * animations.AnimationDuration(param.index) / 1000;

  while (hue > 1) //kluge to get around lack of functioning fmod
    hue -= 1;

  RgbColor color = HslColor(hue, 1.0f, controllers[param.index].dimmer);
  RGBSetPixelColor(color);
}

void searchingBlip(const AnimationParam& param) {
  if (param.state == AnimationState_Completed) { // animation finished, restart
    animations.RestartAnimation(controllers[param.index].nextAnimation);
  }

  controllers[param.index].hue[0] += 0.0001f;
  if (controllers[param.index].hue[0] > 1) {
    controllers[param.index].hue[0] -= 1;
  }

  RgbColor color = HslColor(controllers[param.index].hue[0], 1.0f, controllers[param.index].dimmer);
  RGBSetPixelColor(color);
}

void smoothBlip(const AnimationParam& param) {
  if (param.state == AnimationState_Completed) { // animation finished, restart
    animations.RestartAnimation(controllers[param.index].nextAnimation);
  }

  uint16_t blips = mesh.connectionCount() + 1;
  if (blips > MAX_BLIPS )
    blips = MAX_BLIPS;

  uint8_t hueIdx = (int)(param.progress * blips);

  // Lightness has to be calculated before any hueIdx adjustments.
  // Lightness will be stronger in the middle of each period of each color.
  float lightness = ((param.progress * blips) - hueIdx) * 2;
  if (lightness > 1)
    lightness = 2 - lightness;
  lightness *= controllers[param.index].dimmer;

  if (hueIdx >= blips)
    hueIdx = 0;

  RgbColor color = HslColor(controllers[param.index].hue[hueIdx], 1.0f, lightness);
  RGBSetPixelColor(color);
}

