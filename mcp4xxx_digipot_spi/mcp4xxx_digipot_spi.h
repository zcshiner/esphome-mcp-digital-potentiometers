#pragma once
#include "esphome/components/spi/spi.h"
#include "../mcp4xxx_digipot_base/mcp4xxx_digipot_base.h"

namespace esphome {
namespace mcp4xxx_digipot_spi {

class mcp4xxx_digipot_spi_component : public mcp4xxx_digipot_base::mcp4xxx_digipot_base_component,
              public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                     spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_200KHZ> {
 using mcp4xxx_digipot_base::mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
 public:
  void dump_config() override;

protected:
  bool check_spi_CMDERR_(uint8_t *data);
  bool write_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address, mcp4xxx_digipot_base::MCP4XXXCommands command, uint16_t data_bits = 0) override;
  bool read_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address, uint16_t *data) override;
  void communication_init_() override;
};

}  // namespace mcp4xxx_digipot_spi
}  // namespace esphome
