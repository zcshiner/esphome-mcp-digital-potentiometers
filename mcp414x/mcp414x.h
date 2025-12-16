#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "../mcp4xxx_digipot_base/mcp4xxx_digipot_base.h"

namespace esphome {
namespace mcp414x {

class MCP414X : public mcp4xxx_digipot_base::mcp4xxx_digipot_spi_component {
  using mcp4xxx_digipot_spi_component::mcp4xxx_digipot_spi_component;
};

}  // namespace mcp414x
}  // namespace esphome
