#include <HardwareSerial.h>
#include <SPI.h>
#include <StripLight.h>

class Color {
private:
    static const uint8_t hueToValue(const uint8_t hue) {
        if (hue < 43)
            return 6 * hue;
        if (hue < 128)
            return 255;
        if (hue < 170)
            return (170 - hue) * 6;
        return 0;
    }
public:
    const uint8_t red, green, blue;

    Color(uint8_t red, uint8_t green, uint8_t blue) :
        red(red), green(green), blue(blue) {};

    static Color from_hue(uint8_t hue) {
        return Color(
            Color::hueToValue(hue + 85),
            Color::hueToValue(hue),
            Color::hueToValue(hue - 85));
    }

    // uint8_t renderRed(uint8_t outputIndex) {
    //     return lightnessLUT[red].outValue(outputIndex);
    // }
    // uint8_t renderGreen(uint8_t outputIndex) {
    //     return lightnessLUT[green].outValue(outputIndex);
    // }
    // uint8_t renderBlue(uint8_t outputIndex) {
    //     return lightnessLUT[blue].outValue(outputIndex);
    // }
};

const uint8_t lpd8806_latch = 0x00;
const uint8_t lpd8806_continuation = 0x80;
const uint16_t fadeDelay = 2500;
const Light lightnessLUT[256];
const uint8_t outputCount = 9;
const uint8_t stripBufferSize = outputCount * 4 * 3;
uint8_t stripBuffer[stripBufferSize];


void setOutputHue(uint8_t output, uint8_t index, uint8_t hue) {
    uint8_t *bufferPtr;
    if (index & 2) {
        uint8_t offset = output * 6 + (index - 1) * 3;
        bufferPtr = stripBuffer + stripBufferSize - offset;
    } else {
        bufferPtr = stripBuffer + (output * 6) + (index * 3);
    }
    Color color = Color::from_hue(hue);
    *bufferPtr++ = lightnessLUT[color.green].outValue(0);
    *bufferPtr++ = lightnessLUT[color.red].outValue(0);
    *bufferPtr++ = lightnessLUT[color.blue].outValue(0);
}


void transferBuffer() {
    auto *buf = stripBuffer;
    for (auto i = stripBufferSize; i--;) {
        SPI.transfer(*buf++);
    }
    SPI.transfer(lpd8806_latch);
}

void setup() {
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.transfer(lpd8806_latch);
    memset(stripBuffer, 0x80, stripBufferSize);
}


void loop() {
    const uint8_t outputSelection[] = {0, 1, 3, 2};
    const uint16_t hueChangeInterval = 10;
    const uint16_t frequencyChangeInterval = 1000;
    const uint8_t initialStepDelay = 128;
    uint8_t hue = 0;
    uint8_t hueOffset = 16;
    uint8_t outputIndex = 0;
    auto activeOutput = outputSelection[outputIndex];
    auto stepDelay = initialStepDelay;
    auto nextFrequency = millis() + frequencyChangeInterval;
    auto nextHue = millis() + hueChangeInterval;
    while (true) {
        for (uint8_t output = 0; output < outputCount; ++output) {
            setOutputHue(output, activeOutput, hue + hueOffset * output);
        }
        transferBuffer();
        memset(stripBuffer, 0x80, stripBufferSize);
        outputIndex = (outputIndex + 1) % 4;
        activeOutput = outputSelection[outputIndex];
        if (millis() > nextHue) {
            nextHue = millis() + hueChangeInterval;
            hue += 1;
            if (millis() > nextFrequency) {
                nextFrequency = millis() + frequencyChangeInterval;
                stepDelay = (!stepDelay) ? initialStepDelay : stepDelay >> 1;
            }
        }
        delay(stepDelay);
    }
}