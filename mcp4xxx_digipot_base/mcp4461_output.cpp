#include "mcp4xxx_digipot_base.h"
#include <cmath>

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

static const char *const TAG = "mcp4xxx_digipot_base.output";

void MCP4XXXWiper::set_wiper_level(uint16_t level) {
  this->parent_->set_wiper_value_(this->wiper_, level);
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
    ESP_LOGD(TAG, "Increased wiper %d to tap %d of %d", static_cast<uint8_t>(this->wiper_), wiper_value_, this->parent_->MCP4XXX_MAX_VALUE);
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
    ESP_LOGD(TAG, "Decreased wiper %d to tap %d of %d", static_cast<uint8_t>(this->wiper_), wiper_value_, this->parent_->MCP4XXX_MAX_VALUE);
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

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
