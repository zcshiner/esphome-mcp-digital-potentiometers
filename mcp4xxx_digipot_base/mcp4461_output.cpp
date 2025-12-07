#include "mcp4xxx_digipot_base.h"
#include <cmath>

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

static const char *const TAG = "mcp4xxx_digipot_base.output";

void MCP4XXXWiper::set_level(uint16_t level) {
  this->parent_->set_wiper_value_(this->wiper_, level);
}

void MCP4XXXWiper::write_state(float state) {
  this->set_level(static_cast<uint16_t>(state * this->parent_->MCP4XXX_MAX_VALUE));
}

void MCP4XXXWiper::increase_wiper() {
  if (this->parent_->increment_wiper_(this->wiper_)) {
    // this->state_ = this->update_state();
    ESP_LOGD(TAG, "Increased wiper %u to %u", static_cast<uint8_t>(this->wiper_),
             static_cast<uint16_t>(std::roundf(this->state_ * this->parent_->MCP4XXX_MAX_VALUE)));
  }
}

void MCP4XXXWiper::decrease_wiper() {
  if (this->parent_->decrement_wiper_(this->wiper_)) {
    // this->state_ = this->update_state();
    ESP_LOGD(TAG, "Decreased wiper %u to %u", static_cast<uint8_t>(this->wiper_),
             static_cast<uint16_t>(std::roundf(this->state_ * this->parent_->MCP4XXX_MAX_VALUE)));
  }
}

void MCP4XXXWiper::set_terminals(bool con_a, bool con_w, bool con_b) {
  this->parent_->set_terminal_connection_(this->wiper_, con_a, con_w, con_b);
}

void MCP4XXXWiper::enter_shutdown() {
  this->parent_->enter_shutdown_(this->wiper_);
}

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
