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

typedef enum {
  ON = 0b1,
  OFF = 0b0,
} state_t;

typedef enum {
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

  // h4	h3	h2	h1	a4	a3	a2	a1
  // i4	i3	i2	i1	b4	b3	b2	b1
  // j4	j3	j2	j1	c4	c3	c2	c1
  // k4	k3	k2	k1	d4	d3	d2	d1
  // l4	l3	l2	l1	e4	e3	e2	e1
  // m4	m3	m2	m1	f4	f3	f2	f1
  // n4	n3	n2	n1	g4	g3	g2	g1
  // -- --  --	--	--	--	--	--
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

  void clear_segment_data();
  size_t write(uint16_t *encoded_chars);

  bool write_data(uint8_t *mem);
  bool write_command(uint8_t reg);

  // lambda
  void call_writer() { this->writer_(*this); }

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

  size_t print(const char *str);

  bool update_display();

  // lambda
  void set_writer(std::function<void(HT16K33Component &)> &&writer) { this->writer_ = std::move(writer); }
  std::function<void(HT16K33Component &)> writer_;

};

}  // namespace ht16k33
}  // namespace esphome
