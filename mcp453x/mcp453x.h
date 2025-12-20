#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "../mcp4xxx_digipot_i2c/mcp4xxx_digipot_i2c.h"

namespace esphome {
namespace mcp453x {

class MCP453X : public mcp4xxx_digipot_i2c::mcp4xxx_digipot_i2c_component {
  using mcp4xxx_digipot_i2c::mcp4xxx_digipot_i2c_component::mcp4xxx_digipot_i2c_component;
};

}  // namespace mcp453x
}  // namespace esphome
