#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
// #include "esphome/components/spi/spi.h"

#include "../mcp4xxx_digipot_base/mcp4xxx_digipot_base.h"

namespace esphome {
namespace mcp414x {

class Mcp414xComponent : public mcp4xxx_digipot_base::mcp4xxx_digipot_spi_component { // break base into volatile memory and nv

};

}  // namespace mcp414x
}  // namespace esphome
