#pragma once
#include "esphome/components/i2c/i2c.h"
#include "../mcp4xxx_digipot_base/mcp4xxx_digipot_base.h"

namespace esphome {
namespace mcp4xxx_digipot_i2c {

class mcp4xxx_digipot_i2c_component : public mcp4xxx_digipot_base::mcp4xxx_digipot_base_component, public i2c::I2CDevice {
 using mcp4xxx_digipot_base::mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
 public:
  void dump_config() override;

 protected:
  bool write_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address,
                               mcp4xxx_digipot_base::MCP4XXXCommands command, uint16_t data_bits = 0) override;
  bool read_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address, uint16_t *data) override;
  void communication_init_() override;
};

}  // namespace mcp4xxx_digipot_i2c
}  // namespace esphome
