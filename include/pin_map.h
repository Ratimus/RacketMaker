#pragma once

#include <stdint.h>

// SPI pins
constexpr uint8_t CIPO = 12;
constexpr uint8_t COPI = 13;
constexpr uint8_t SCLK = 14;

// DAC chip select pins
constexpr uint8_t DAC0_CS = 33;
constexpr uint8_t DAC1_CS = 32;

// ADC chip select pin
constexpr uint8_t ADC_CS = 15;

// TODO: use SPI OLED. Move encoder off SPI bus and onto other pins

// GATE output pins
constexpr uint8_t GATEout_0 = 25;
constexpr uint8_t GATEout_1 = 26;
constexpr uint8_t GATEout_2 = 4;
constexpr uint8_t GATEout_3 = 27;
constexpr uint8_t NUM_GATES_OUT = 4;
constexpr uint8_t GATE_OUT_PINS[NUM_GATES_OUT]{GATEout_0, GATEout_1, GATEout_2, GATEout_3};

// GATE input pins
constexpr uint8_t GATEin_0 = 36;
constexpr uint8_t GATEin_1 = 39;
constexpr uint8_t GATEin_2 = 34;
constexpr uint8_t GATEin_3 = 35;
constexpr uint8_t NUM_GATES_IN = 4;
constexpr uint8_t GATE_IN_PINS[NUM_GATES_IN]{GATEin_0, GATEin_1, GATEin_2, GATEin_3};

// encoder pins
constexpr uint8_t ENC_A = 18;
constexpr uint8_t ENC_B = 5;
constexpr uint8_t ENC_SW = 19;

constexpr uint8_t BN_1 = 4;
constexpr uint8_t BN_2 = 25;
constexpr uint8_t BN_3 = 26;
constexpr uint8_t BN_4 = 27;
constexpr uint8_t NUM_PUSHBUTTONS = 4;
constexpr uint8_t PUSHBUTTON_PINS[NUM_PUSHBUTTONS]{BN_1, BN_2, BN_3, BN_4};
