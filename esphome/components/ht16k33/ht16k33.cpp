#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "pgmspace.h"
#include <bitset>
#include "ht16k33.h"
#include "font.h"

#define UNKNOWN_CHAR 95

namespace esphome {
namespace ht16k33 {

static const char *const TAG = "ht16k33";

void HT16K33Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HT16K33...");

  this->write_command(get_system_cmd());
  this->write_command(get_display_cmd());
  this->write_command(get_dimming_cmd());

}

uint8_t HT16K33Component::get_system_cmd() { return CMD_SYSTEM_SETUP | clock_state; }
uint8_t HT16K33Component::get_display_cmd() { return CMD_DISPLAY_SETUP | blink_rate | display_state; }
uint8_t HT16K33Component::get_dimming_cmd() { return CMD_DIMMING_SETUP | brightness; }

void HT16K33Component::dump_config() {
  ESP_LOGCONFIG(TAG, "HT16K33:");
  ESP_LOGCONFIG(TAG, "  Clock State: %s", ONOFF(clock_state));
  ESP_LOGCONFIG(TAG, "  Display State: %s", ONOFF(display_state));
  ESP_LOGCONFIG(TAG, "  Brightness: %u", brightness);
  ESP_LOGCONFIG(TAG, "  Blink Rate: %u", blink_rate);

  LOG_I2C_DEVICE(this);
}
float HT16K33Component::get_setup_priority() const { return setup_priority::PROCESSOR; }
void HT16K33Component::update() {
  this->call_writer();
 }

void HT16K33Component::set_brightness(uint8_t desired_brightness) {
  brightness = std::min(desired_brightness, (uint8_t) 15);
}

bool HT16K33Component::decimal_on(bool update_now) { return set_decimal_state(ON, update_now); }
bool HT16K33Component::decimal_off(bool update_now) { return set_decimal_state(OFF, update_now); }
bool HT16K33Component::set_decimal_state(uint8_t desired_decimal_state, bool update_now) {
  decimal_state = desired_decimal_state;

  if (update_now) {
    return update_display();
  } else {
    return true;
  }
}

bool HT16K33Component::colon_on(bool update_now) { return set_colon_state(ON, update_now); }
bool HT16K33Component::colon_off(bool update_now) { return set_colon_state(OFF, update_now); }
bool HT16K33Component::set_colon_state(uint8_t desired_colon_state, bool update_now) {
  colon_state = desired_colon_state;

  if (update_now) {
    return update_display();
  } else {
    return true;
  }
}

void HT16K33Component::clear_segment_data() {
  for(uint8_t i = 0; i < char_bytes_size; i++){
    segment_data[i] = 0;
  }
}

size_t HT16K33Component::write(uint16_t *encoded_chars) {
  // 01010000 n-h4
  // 00001100 n-h3
  // 00001000 n-h2
  // 00000100 n-h1
  // 00000111 g-a4
  // 00000101 g-a3
  // 00000011 g-a2
  // 00000001 g-a1
  uint8_t split_encoding[char_bytes_size] = {};

  clear_segment_data();

  for (uint8_t i = 0; i < display_size; i++)
  {
    segment_data[i] = 0;
    split_encoding[(char_bytes_size - 1 - i)] = (uint8_t)encoded_chars[i];
    split_encoding[(char_bytes_size - 1 - 4 - i)] = encoded_chars[i] >> 8;
  }

  for(uint8_t i = 0; i < char_bytes_size; i++){
    uint8_t mask = 0b10000000 >> i;

    for(uint8_t b = 0; b < 8; b++){
      uint8_t row = 8-1-b;
      if(i > b){
        segment_data[row] |= ((split_encoding[i] >> (i-b)) & mask);
      } else {
        segment_data[row] |= ((split_encoding[i] << (b-i)) & mask);
      }
    }
  }

  update_display();

  return 0;
}

size_t HT16K33Component::print(const char *str) {
  uint16_t encoded_chars[display_size] = {};
  uint8_t i = 0;
  while (*str != '\0' && i < display_size)
  {
    uint8_t c = (int)(*str++);
    encoded_chars[i] = pgm_read_word(&alphanumeric_segs[c]);
    i++;
  }

  return write(encoded_chars);
}

bool HT16K33Component::update_display() {
  uint8_t mem[mem_size] = {};
  for (uint8_t i = 0; i < mem_size; i++)
  {
    mem[i] = *memory_template[i];
  }
  return write_data(mem);
}

bool HT16K33Component::write_data(uint8_t *mem) {
  if (!this->write_bytes(0, mem, mem_size))
  {
    ESP_LOGE(TAG, "FAILED TO WRITE");
    return false;
  }
  return true;
};

bool HT16K33Component::write_command(uint8_t reg) {
  return this->write_bytes(reg, nullptr, 0);
};

}  // namespace ht16k33
}  // namespace esphome
