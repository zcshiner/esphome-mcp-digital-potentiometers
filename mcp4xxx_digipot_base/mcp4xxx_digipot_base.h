#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/output/float_output.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

// Commands Bits C1 C0
enum MCP4XXXCommands : uint8_t {
  WRITE = 0x00,
  INCREMENT = 0x04,
  DECREMENT = 0x08,
  READ = 0x0C
};

// Memory Addresses A3 A2 A1 A0, not all devices have all registers
enum MCP4XXXAddresses : uint8_t {
  MCP4XXX_VW0 = 0x00,
  MCP4XXX_VW1 = 0x01,
  MCP4XXX_NVW0 = 0x02,
  MCP4XXX_NVW1 = 0x03,
  MCP4XXX_TCON0 = 0x04,
  MCP4XXX_STATUS = 0x05,
  MCP4XXX_VW2 = 0x06,
  MCP4XXX_VW3 = 0x07,
  MCP4XXX_NVW2 = 0x08,
  MCP4XXX_NVW3 = 0x09,
  MCP4XXX_TCON1 = 0x0A
};

// Wiper Identifiers, not all devices have all 4 wipers
enum MCP4XXXWiperID : uint8_t {
  WIPER_0 = MCP4XXXAddresses::MCP4XXX_VW0,
  WIPER_1 = MCP4XXXAddresses::MCP4XXX_VW1,
  WIPER_2 = MCP4XXXAddresses::MCP4XXX_VW2,
  WIPER_3 = MCP4XXXAddresses::MCP4XXX_VW3
};

// Terminal Connection Registers, not all devices have both TCON registers
enum MCP4XXX_TCON_N : uint8_t {
  MCP4XXX_TCON_0 = MCP4XXXAddresses::MCP4XXX_TCON0,
  MCP4XXX_TCON_1 = MCP4XXXAddresses::MCP4XXX_TCON1
};

class mcp4xxx_digipot_base_component : public Component {
 public:
  mcp4xxx_digipot_base_component(uint16_t digipot_taps) : MCP4XXX_MAX_VALUE(digipot_taps) {}
  void setup() override;
  void dump_config_base();
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  /// @brief Get maximum tap position of wiper, either 7 or 8 bit
  /// @return Maximum tap position as integer
  uint16_t get_tap_count() const { return MCP4XXX_MAX_VALUE; }

 protected:
  friend class MCP4XXXWiper;
  friend class mcp4xxx_nonvolatile_memory;
  bool write_tcon_register_(MCP4XXX_TCON_N tcon_id_, uint16_t value);
  uint16_t read_tcon_register_(MCP4XXX_TCON_N tcon_id_);
  uint16_t read_status_register_();
  bool set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value);
  uint16_t read_wiper_value_(MCP4XXXWiperID wiper);
  bool increment_wiper_(MCP4XXXWiperID wiper);
  bool decrement_wiper_(MCP4XXXWiperID wiper);
  bool set_terminal_connection_(MCP4XXXWiperID wiper, bool connect_a, bool connect_w, bool connect_b);
  bool set_wiper_enter_shutdown_(MCP4XXXWiperID wiper);
  bool set_wiper_exit_shutdown_(MCP4XXXWiperID wiper);
  bool EEPROM_write_active_();
  virtual bool write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits = 0) = 0;
  virtual bool read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) = 0;
  virtual void communication_init_() = 0;
  uint16_t MCP4XXX_MAX_VALUE;
};

class mcp4xxx_digipot_i2c_component : public mcp4xxx_digipot_base_component, public i2c::I2CDevice {
 using mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
 public:
  void dump_config() override;

 protected:
  bool write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits = 0) override;
  bool read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) override;
  void communication_init_() override;
};

class mcp4xxx_digipot_spi_component : public mcp4xxx_digipot_base_component,
              public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                     spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_200KHZ> {
 using mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
 public:
  void dump_config() override;

protected:
  bool check_spi_CMDERR_(uint8_t *data);
  bool write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits = 0) override;
  bool read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) override;
  void communication_init_() override;
};

class MCP4XXXWiper : public output::FloatOutput, public Parented<mcp4xxx_digipot_base_component> {
 public:
  MCP4XXXWiper(mcp4xxx_digipot_base_component *parent, MCP4XXXWiperID wiper) : parent_(parent), wiper_(wiper) {}
  /// @brief Set level of wiper
  /// @param[in] state float value between 0.0 and 1.0
  void set_wiper_level(uint16_t level);
  /// @brief Increase wiper by 1 tap, until max value is reached
  uint16_t increase_wiper();
  /// @brief Decrease wiper by 1 tap, until min value is reached
  uint16_t decrease_wiper();
    /// @brief Increase wiper by 1 tap blindly
  void increase_wiper_fast();
  /// @brief Decrease wiper by 1 tap blindly
  void decrease_wiper_fast();
  /// @brief Set terminal connections
  /// @param con_a Terminal A connection state
  /// @param con_w Terminal W connection state
  /// @param con_b Terminal B connection state
  void set_terminals(bool con_a, bool con_w, bool con_b);
  /// @brief Put wiper into shutdown mode
  void enter_shutdown();
  /// @brief Wake wiper from shutdown mode
  void exit_shutdown();
  /// @brief Get maximum tap position of wiper, either 7 or 8 bit
  uint16_t get_tap_count() { return this->parent_->get_tap_count(); }

 protected:
  void write_state(float state) override;

  mcp4xxx_digipot_base_component *parent_;
  MCP4XXXWiperID wiper_;

};

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
