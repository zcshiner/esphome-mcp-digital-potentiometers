#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
// #include "esphome/components/i2c/i2c.h"

#include "../mcp4xxx_digipot_base/mcp4xxx_digipot_base.h"

namespace esphome {
namespace mcp446x {

class MCP446X : public mcp4xxx_digipot_base::mcp4xxx_digipot_i2c_component { //, mcp4xxx_digipot_base::mcp4xxx_nonvolatile_memory {
  using mcp4xxx_digipot_i2c_component::mcp4xxx_digipot_i2c_component;
};

}  // namespace mcp446x
}  // namespace esphome
