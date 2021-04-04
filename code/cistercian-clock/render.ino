/*
 * This file is part of Cistercian Clock.
 *
 * Cistercian Clock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Cistercian Clock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Cistercian Clock. If not, see <https://www.gnu.org/licenses/>.
 */
 
#include <FastLED.h>
#include <Time.h>
#include <TimeLib.h>

#include "config_struct.h"

#define NEOPIXEL_PIN D4

static const uint8_t matrix_width = 5;
static const uint8_t matrix_height = 7;

static const uint8_t onesMask[10][7] = {{0b00100000,  /* 0 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00111000,  /* 1 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 2 */
                                     0b00100000,
                                     0b00111000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 3 */
                                     0b00110000,
                                     0b00101000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00101000,  /* 4 */
                                     0b00110000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00111000,  /* 5 */
                                     0b00110000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00101000,  /* 6 */
                                     0b00101000,
                                     0b00101000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00111000,  /* 7 */
                                     0b00101000,
                                     0b00101000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00101000,  /* 8 */
                                     0b00101000,
                                     0b00111000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00111000,  /* 9 */
                                     0b00101000,
                                     0b00111000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000}};

static const uint8_t tensMask[10][7] = {{0b00100000,  /* 00 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b11100000,  /* 10 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 20 */
                                     0b00100000,
                                     0b11100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 30 */
                                     0b01100000,
                                     0b10100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b10100000,  /* 40 */
                                     0b01100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b11100000,  /* 50 */
                                     0b01100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b10100000,  /* 60 */
                                     0b10100000,
                                     0b10100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b11100000,  /* 70 */
                                     0b10100000,
                                     0b10100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b10100000,  /* 80 */
                                     0b10100000,
                                     0b11100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b11100000,  /* 90 */
                                     0b10100000,
                                     0b11100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000}};

static const uint8_t hundredsMask[10][7] = {{0b00100000,  /* 000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 100 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00111000},
                                    {0b00100000,  /* 200 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00111000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 300 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00101000,
                                     0b00110000,
                                     0b00100000},
                                    {0b00100000,  /* 400 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00110000,
                                     0b00101000},
                                    {0b00100000,  /* 500 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00110000,
                                     0b00111000},
                                    {0b00100000,  /* 600 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00101000,
                                     0b00101000,
                                     0b00101000},
                                    {0b00100000,  /* 700 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00101000,
                                     0b00101000,
                                     0b00111000},
                                    {0b00100000,  /* 800 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00111000,
                                     0b00101000,
                                     0b00101000},
                                    {0b00100000,  /* 900 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00111000,
                                     0b00101000,
                                     0b00111000}};

static const uint8_t thousandsMask[10][7] = {{0b00100000,  /* 0000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 1000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b11100000},
                                    {0b00100000,  /* 2000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b11100000,
                                     0b00100000,
                                     0b00100000},
                                    {0b00100000,  /* 3000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b10100000,
                                     0b01100000,
                                     0b00100000},
                                    {0b00100000,  /* 4000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b01100000,
                                     0b10100000},
                                    {0b00100000,  /* 5000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b01100000,
                                     0b11100000},
                                    {0b00100000,  /* 6000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b10100000,
                                     0b10100000,
                                     0b10100000},
                                    {0b00100000,  /* 7000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b10100000,
                                     0b10100000,
                                     0b11100000},
                                    {0b00100000,  /* 8000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b11100000,
                                     0b10100000,
                                     0b10100000},
                                    {0b0100000,  /* 9000 */
                                     0b00100000,
                                     0b00100000,
                                     0b00100000,
                                     0b11100000,
                                     0b10100000,
                                     0b11100000}};

static const uint8_t colMask[] = { 0b10000000,
                                    0b01000000,
                                    0b00100000,
                                    0b00010000,
                                    0b00001000 };

CRGB leds[matrix_width * matrix_height];

extern clockConfig_t clockConfig;

void render_setup()
{
  FastLED.addLeds<WS2811, NEOPIXEL_PIN, GRB>(leds, matrix_width * matrix_height).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(40);
  fill_solid(leds, matrix_width * matrix_height, 0);
  FastLED.show();
}

void render_fillRed()
{
  FastLED.setBrightness(40);
  fill_solid(leds, matrix_width * matrix_height, CRGB(255, 0, 0)); // Red
  FastLED.show();
}

void render_fillGreen()
{
  FastLED.setBrightness(40);
  fill_solid(leds, matrix_width * matrix_height, CRGB(0, 255, 0)); // Green
  FastLED.show();
}

void render_fillBlue()
{
  FastLED.setBrightness(40);
  fill_solid(leds, matrix_width * matrix_height, CRGB(0, 0, 255)); // Blue
  FastLED.show();
}

void render_clear()
{
  fill_solid(leds, matrix_width * matrix_height, 0);
  FastLED.show();
}

static uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( y & 0x01) {
    // Odd rows run backwards
    uint8_t reverseX = (matrix_width - 1) - x;
    i = (y * matrix_width) + reverseX;
  } else {
    // Even rows run forwards
    i = (y * matrix_width) + x;
  }
  
  return i;
}

void render_time(tmElements_t now, uint8_t bright) {
  CRGB displayColour;
  uint32_t tmpHue;
  uint8_t hue, sat, val;
  uint8_t displayMask[7];
  uint8_t i;
  uint8_t tenHours, oneHours, tenMinutes, oneMinutes;
  uint8_t x, y;
  
  sat = 255;
  val = 255;
  tmpHue = ((now.Hour * 60) + now.Minute) % 256;
  hue = (uint8_t) tmpHue;
  hsv2rgb_rainbow(CHSV(hue, sat, val), displayColour);
  
  yield();
  
  fill_solid(leds, matrix_width * matrix_height, 0);

  tenHours = now.Hour / 10;
  oneHours = now.Hour % 10;
  tenMinutes = now.Minute / 10;
  oneMinutes = now.Minute % 10;
  
  for (i = 0; i < matrix_height; i++)
  {
    displayMask[i] = onesMask[oneMinutes][i] | tensMask[tenMinutes][i] | hundredsMask[oneHours][i] | thousandsMask[tenHours][i];
  }

  for (x = 0; x < matrix_width; x++)
  {
    for (y = 0; y < matrix_height; y++)
    {
      if (displayMask[y] & colMask[x])
      {
        leds[XY(x, y)] = displayColour;
      }
    }
  }

  FastLED.setBrightness(bright);
  yield();
  FastLED.show();
}
