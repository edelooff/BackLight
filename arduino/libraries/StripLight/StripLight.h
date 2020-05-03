#include <Arduino.h>

class Light {
private:
  static constexpr float linear_divisor = 841 / 108 * 116 * 2.55;
  static const uint16_t lightness_range = 508;
  static uint8_t auto_init_count;

public:
  uint8_t base, partial;
  Light();
  Light(uint8_t value);
  uint8_t outValue(uint8_t index);
};

