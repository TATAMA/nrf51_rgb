/* Lava
 * 
 * WS2812B Tricolor LED (neopixel) controller
 *
 *
 * Example code:
 
	neopixel_strip_t m_strip;
	uint8_t dig_pin_num = 6;
	uint8_t leds_per_strip = 24;
	uint8_t error;
	uint8_t led_to_enable = 10;
	uint8_t red = 255;
	uint8_t green = 0;
	uint8_t blue = 159;

	neopixel_init(&m_strip, dig_pin_num, leds_per_strip);
	neopixel_clear(&m_strip);
	error = neopixel_set_color_and_show(&m_strip, led_to_enable, red, green, blue);
	if (error) {
		//led_to_enable was not within number leds_per_strip
	}
	//clear and remove strip
	neopixel_clear(&m_strip);
	neopixel_destroy(&m_strip);
 
 
 * For use with BLE stack, see information below:
	- Include in main.c
		#include "ble_radio_notification.h"
	- Call (see nrf_soc.h: NRF_RADIO_NOTIFICATION_DISTANCES and NRF_APP_PRIORITIES)
		ble_radio_notification_init(NRF_APP_PRIORITY_xxx,
									NRF_RADIO_NOTIFICATION_DISTANCE_xxx,
									your_radio_callback_handler);
	- Create 
		void your_radio_callback_handler(bool radio_active)
		{
			if (radio_active == false)
			{
				neopixel_show(&strip1);
				neopixel_show(&strip2);
				//...etc
			}
		}
	- Do not use neopixel_set_color_and_show(...) with BLE, instead use uint8_t neopixel_set_color(...);
*/
 
 
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "neopixel.h"


void neopixel_init(neopixel_strip_t *strip, uint8_t pin_num, uint16_t num_leds)
{
	strip->leds = (color_t*) malloc(sizeof(color_t) * num_leds);
	strip->pin_num = pin_num;
	strip->num_leds = num_leds;
	nrf_gpio_cfg_output(pin_num);
	NRF_GPIO->OUTCLR = (1UL << pin_num);
	for (int i = 0; i < num_leds; i++)
	{	
		strip->leds[i].simple.g = 0;
		strip->leds[i].simple.r = 0;
		strip->leds[i].simple.b = 0;
	}
}

void neopixel_clear(neopixel_strip_t *strip)
{
		for (int i = 0; i < strip->num_leds; i++)
		{
			strip->leds[i].simple.g = 0;
			strip->leds[i].simple.r = 0;
			strip->leds[i].simple.b = 0;
		}
			neopixel_show(strip);
}

void neopixel_show(neopixel_strip_t *strip)
{
	const uint8_t PIN =  strip->pin_num;
	NRF_GPIO->OUTCLR = (1UL << PIN);
	nrf_delay_us(50);
			for (int i = 0; i < strip->num_leds; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if ((strip->leds[i].grb[j] & 128) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 64) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 32) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 16) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 8) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 4) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 2) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
					
					if ((strip->leds[i].grb[j] & 1) > 0)	{NEOPIXEL_SEND_ONE}
					else	{NEOPIXEL_SEND_ZERO}
				}
			}
}

uint8_t neopixel_set_color(neopixel_strip_t *strip, uint16_t index, uint8_t red, uint8_t green, uint8_t blue )
{
		if (index < strip->num_leds)
		{
			strip->leds[index].simple.r = red;
			strip->leds[index].simple.g = green;
			strip->leds[index].simple.b = blue;
		}
		else
				return 1;
		return 0;
}

uint8_t neopixel_set_color_and_show(neopixel_strip_t *strip, uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
		if (index < strip->num_leds)
		{
			strip->leds[index].simple.r = red;
			strip->leds[index].simple.g = green;
			strip->leds[index].simple.b = blue;
		  neopixel_show(strip);	
		}
		else
				return 1;
		return 0;
}

void neopixel_destroy(neopixel_strip_t *strip)
{
	free(strip->leds);
	strip->num_leds = 0;
	strip->pin_num = 0;
}


void colorWipe(neopixel_strip_t *strip, color_t* c, uint8_t wait)
{
  for(uint16_t i=0; i<strip->num_leds; i++) {
      neopixel_set_color_and_show(strip, i, c->simple.r, c->simple.g, c->simple.b );
      nrf_delay_ms(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(uint8_t WheelPos, color_t* c)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85)
  {
   c->simple.r = 255 - WheelPos * 3;
   c->simple.g = 0;
   c->simple.b = WheelPos * 3;
  }
  else if(WheelPos < 170)
  {
    WheelPos -= 85;
    c->simple.r = 0;
    c->simple.g = WheelPos * 3;
    c->simple.b = 255 - WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    c->simple.r = WheelPos * 3;
    c->simple.g = 255 - WheelPos * 3;
    c->simple.b = 0;
  }
}

/*
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

*/