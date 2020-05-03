#include <HardwareSerial.h>
#include <SPI.h>
#include <StripLight.h>

const uint8_t lpd8806_latch = 0x00;
const uint8_t lpd8806_continuation = 0x80;
const uint16_t fadeDelay = 2500;
const uint8_t output_count = 9;
const Light lightnessLUT[256];

void output_grey(byte level) {
  // LED order: green, red, blue
  SPI.transfer(level);
  SPI.transfer(lpd8806_continuation);
  SPI.transfer(level);
}


void light_divided(Light light) {
  for (uint8_t i = output_count; i--;) {
    output_grey(light.outValue(0));
    output_grey(light.outValue(3));
    output_grey(light.outValue(1));
    output_grey(light.outValue(2));
  }
}


void light_equal(Light light) {
  for (uint8_t i = output_count * 4; i--;) {
    output_grey(light.outValue(0));
  }
}


void setup() {
  Serial.begin(57600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.transfer(lpd8806_latch);

  byte i = 0;
  do {
    Serial.print("LUT value ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(lightnessLUT[i].base);
    Serial.print(", ");
    Serial.println(lightnessLUT[i].partial);
  } while (i++ < 255);
}

void loop() {
  uint8_t max = 255;
  uint8_t step = 0;

  // Pulse once using 7-bit "direct drive"
  while (step++ < max) {
    light_equal(lightnessLUT[step]);
    SPI.transfer(lpd8806_latch);
    delayMicroseconds(fadeDelay);
  }
  while (--step) {
    light_equal(lightnessLUT[step]);
    SPI.transfer(lpd8806_latch);
    delayMicroseconds(fadeDelay);
  }

  // Pulse once using 9-bit "staggered drive"
  while (step++ < max) {
    light_divided(lightnessLUT[step]);
    SPI.transfer(lpd8806_latch);
    delayMicroseconds(fadeDelay);
  }
  while (--step) {
    light_divided(lightnessLUT[step]);
    SPI.transfer(lpd8806_latch);
    delayMicroseconds(fadeDelay);
  }
}
