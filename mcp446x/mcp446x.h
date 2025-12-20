#pragma once

// #include "esphome/core/component.h"
// #include "esphome/core/log.h"
#include "../mcp4xxx_digipot_i2c/mcp4xxx_digipot_i2c.h"

namespace esphome {
namespace mcp446x {

class MCP446X : public mcp4xxx_digipot_i2c::mcp4xxx_digipot_i2c_component {
  using mcp4xxx_digipot_i2c_component::mcp4xxx_digipot_i2c_component;
};

}  // namespace mcp446x
}  // namespace esphome
