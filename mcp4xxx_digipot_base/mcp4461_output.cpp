#include "mcp4xxx_digipot_base.h"
#include <cmath>

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

static const char *const TAG = "mcp4xxx_digipot_base.output";

bool MCP4XXXWiper::set_wiper_level(uint16_t level) {
  return this->parent_->set_wiper_value_(this->wiper_, level, false);
}

bool MCP4XXXWiper::set_nonvolatile_wiper_level(uint16_t level) {
  if (!this->parent_->MCP4XXX_HAS_NV_MEMORY) {
    ESP_LOGW(TAG, "NV wiper not supported");
  }

  if (this->parent_->EEPROM_write_active_()) {
    ESP_LOGE(TAG, "Unable to write to non-volatile wiper");
  } else {
    return this->parent_->set_wiper_value_(this->wiper_, level, true);
  }
  return true;
}

// Override from FloatOutput - Called from Home Assistant
void MCP4XXXWiper::write_state(float state) {
  this->set_wiper_level(static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE));
}

void MCP4XXXWiper::increase_wiper_fast() {
  if (this->is_inverted()) {
  this->parent_->decrement_wiper_(this->wiper_);
  ESP_LOGV(TAG, "Using inverted logic");
  } else {
    this->parent_->increment_wiper_(this->wiper_);
  }
}

void MCP4XXXWiper::decrease_wiper_fast() {
  if (this->is_inverted()) {
  this->parent_->increment_wiper_(this->wiper_);
  ESP_LOGV(TAG, "Using inverted logic");
  } else {
    this->parent_->decrement_wiper_(this->wiper_);
  }
}

uint16_t MCP4XXXWiper::increase_wiper() {
  uint16_t wiper_value_ = this->parent_->read_wiper_value_(this->wiper_);
  if (wiper_value_ < this->parent_->MCP4XXX_MAX_VALUE) {
    this->increase_wiper_fast();
    wiper_value_++;
    ESP_LOGD(TAG, "Increased wiper %d to tap %d of %d", static_cast<uint8_t>(this->wiper_),
             wiper_value_, this->parent_->MCP4XXX_MAX_VALUE);
  } else {
    ESP_LOGW(TAG, "Unable to increment wiper %u. Already at max level.", static_cast<uint8_t>(this->wiper_));
  }
  return wiper_value_;
}

uint16_t MCP4XXXWiper::decrease_wiper() {
    uint16_t wiper_value_ = this->parent_->read_wiper_value_(this->wiper_);
  if (wiper_value_ > 0) {
    this->decrease_wiper_fast();
    wiper_value_--;
    ESP_LOGD(TAG, "Decreased wiper %d to tap %d of %d", static_cast<uint8_t>(this->wiper_),
             wiper_value_, this->parent_->MCP4XXX_MAX_VALUE);
  } else {
    ESP_LOGW(TAG, "Unable to decrement wiper %u.  Already at min level.", static_cast<uint8_t>(this->wiper_));
  }
  return wiper_value_;
}

void MCP4XXXWiper::set_terminals(bool con_a, bool con_w, bool con_b) {
  this->parent_->set_terminal_connection_(this->wiper_, con_a, con_w, con_b);
}

void MCP4XXXWiper::enter_shutdown() {
  this->parent_->set_wiper_enter_shutdown_(this->wiper_);
}

void MCP4XXXWiper::exit_shutdown() {
  this->parent_->set_wiper_exit_shutdown_(this->wiper_);
}

void MCP4XXXWiper::set_initial_terminals_(bool con_a, bool con_w, bool con_b) {
  uint8_t tcon_index;
  if (this->wiper_ == WIPER_0 || this->wiper_ == WIPER_1) {
    tcon_index = 0;
    
  } else { // WIPER_2 and WIPER_3
    tcon_index = 1;
  }

  uint16_t tcon_value = this->parent_->tcon_initial_value[tcon_index];
  uint16_t tcon_update = this->parent_->build_tcon_payload_(con_a, con_w, con_b, true);
  uint16_t bitmask = 0b00000111;  // 3 bits for wiper 0

  if (this->wiper_ == WIPER_1 || this->wiper_ == WIPER_3) {
    tcon_update = tcon_update << 4;
    bitmask = bitmask << 4;
  }
  this->parent_->tcon_initial_value[tcon_index] = mcp4xxx_digipot_base::replace_bits(tcon_value, bitmask, tcon_update);
  ESP_LOGVV(TAG, "Initial terminal connections set - A:%s, W:%s, B:%s (TCON=0x%02X)",
           con_a ? "ON" : "OFF", con_w ? "ON" : "OFF", con_b ? "ON" : "OFF",
           this->parent_->tcon_initial_value[tcon_index]);
}

void MCP4XXXWiper::set_initial_state_(float state) {
  switch (this->wiper_) {
  case WIPER_0:
    this->parent_->wiper_initial_value[0] = static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE);
    break;
  case WIPER_1:
    this->parent_->wiper_initial_value[1] = static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE);
    break;
  case WIPER_2:
    this->parent_->wiper_initial_value[2] = static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE);
    break;
  case WIPER_3:
    this->parent_->wiper_initial_value[3] = static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE);
    break;

  default:
    break;
  }
  ESP_LOGVV(TAG, "Initial wiper %d level set to tap %d of %d",
           static_cast<uint8_t>(this->wiper_),
           static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE),
           this->parent_->MCP4XXX_MAX_VALUE);
}

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
