SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ARDUINO_SKETCHBOOK = $(abspath $(SELF_DIR))

include $(SELF_DIR)../../Arduino-Makefile/Arduino.mk
