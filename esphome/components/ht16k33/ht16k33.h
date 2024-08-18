#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ht16k33 {

typedef enum {
  CMD_SYSTEM_SETUP = 0x20,
  CMD_DISPLAY_SETUP = 0x80,
  CMD_DIMMING_SETUP = 0xE0,
} command_t;

typedef enum {  // CMD_SYSTEM_SETUP
  ON = 0b1,
  OFF = 0b0,
} state_t;

typedef enum {  // CMD_DISPLAY_SETUP
  BLINK_RATE_NOBLINK = 0x0,
  BLINK_RATE_2HZ = 0x2,
  BLINK_RATE_1HZ = 0x4,
  BLINK_RATE_0_5HZ = 0x6,
} blink_rate_t;

class HT16K33Component : public PollingComponent, public i2c::I2CDevice {
  private:


 protected:
  uint8_t zero = 0;
  static constexpr uint8_t display_size = 4;
  static constexpr uint8_t char_bytes_size = 8;
  static constexpr uint8_t mem_size = 16;

  // config
  state_t clock_state = ON;
  blink_rate_t blink_rate = BLINK_RATE_NOBLINK;
  state_t display_state = ON;
  uint8_t brightness = 8;

  // state
  uint8_t decimal_state = OFF;
  uint8_t colon_state = OFF;
  uint8_t segment_data[char_bytes_size] = {};
  uint8_t *memory_template[mem_size] = {
      &segment_data[0],
      &colon_state,
      &segment_data[1],
      &decimal_state,
      &segment_data[2],
      &zero,
      &segment_data[3],
      &zero,
      &segment_data[4],
      &zero,
      &segment_data[5],
      &zero,
      &segment_data[6],
      &zero,
      &segment_data[7],
      &zero
  };

  uint8_t get_system_cmd();
  uint8_t get_display_cmd();
  uint8_t get_dimming_cmd();

  bool set_colon_state(uint8_t desired_colon_state, bool update_now);
  bool set_decimal_state(uint8_t desired_decimal_state, bool update_now);

  bool write_data(uint8_t *mem);
  bool write_command(uint8_t reg);

 public:

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  void set_brightness(uint8_t brightness);

  bool colon_on(bool update_now);
  bool colon_off(bool update_now);

  bool decimal_on(bool update_now);
  bool decimal_off(bool update_now);

  size_t write(uint16_t *encoded_chars);
  size_t write(const char *str);

  bool update_display();
};

}  // namespace ht16k33
}  // namespace esphome
