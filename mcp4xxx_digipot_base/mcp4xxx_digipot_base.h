#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/output/float_output.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

// struct WiperState {
//   bool enabled = true;
//   uint16_t state = 0;
//   optional<float> initial_value;
//   bool terminal_a = true;
//   bool terminal_b = true;
//   bool terminal_w = true;
//   bool terminal_hw = true;
//   bool wiper_lock_active = false;
//   bool level_is_stale = false;
//   bool terminal_is_stale = false;
// };

struct NonvolatileWiperState {
  uint16_t nv_wiper_value = 0;
  bool wiper_lock_active = false;
  bool EEPROM_write_protected = false;
  bool level_is_stale = true;
};

enum MCP4XXXCommands : uint8_t {
  WRITE = 0x00,
  INCREMENT = 0x04,
  DECREMENT = 0x08,
  READ = 0x0C
};

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

enum MCP4XXXWiperID : uint8_t {
  WIPER_0 = MCP4XXXAddresses::MCP4XXX_VW0,
  WIPER_1 = MCP4XXXAddresses::MCP4XXX_VW1,
  WIPER_2 = MCP4XXXAddresses::MCP4XXX_VW2,
  WIPER_3 = MCP4XXXAddresses::MCP4XXX_VW3
};

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

 protected:
  friend class MCP4XXXWiper;
  friend class mcp4xxx_nonvolatile_memory;
  uint8_t create_command_byte_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits);
  uint8_t create_data_byte_(uint16_t data_bits);
  bool write_tcon_register_(MCP4XXX_TCON_N tcon_id_, uint16_t value);
  uint16_t read_tcon_register_(MCP4XXX_TCON_N tcon_id_);
  bool set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value);
  uint16_t read_wiper_value_(MCP4XXXWiperID wiper);
  bool increment_wiper_(MCP4XXXWiperID wiper);
  bool decrement_wiper_(MCP4XXXWiperID wiper);
  bool set_terminal_connection_(MCP4XXXWiperID wiper, bool connect_a, bool connect_w, bool connect_b);
  bool set_wiper_enter_shutdown_(MCP4XXXWiperID wiper);
  bool set_wiper_exit_shutdown_(MCP4XXXWiperID wiper);
  virtual bool write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits = 0) = 0;
  virtual bool read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) = 0;
  // virtual bool mcp4xxx_write_(const uint8_t *data, size_t len) = 0;
  // virtual bool mcp4xxx_read_(uint8_t *data, size_t len) = 0;
  virtual void communication_init_() = 0;
  uint16_t MCP4XXX_MAX_VALUE;
  // uint8_t current_wiper_value_{0};
};

// class mcp4xxx_nonvolatile_memory {
//   bool set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value, bool nonvolatile);
// };

class mcp4xxx_digipot_i2c_component : public mcp4xxx_digipot_base_component, public i2c::I2CDevice {
 using mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
 public:
  // bool mcp4xxx_write_(const uint8_t *data, size_t len) override;
  // bool mcp4xxx_read_(uint8_t *data, size_t len) override;
  void dump_config() override;

 protected:
  bool write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits = 0) override;
  bool read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) override;
  void communication_init_() override;
};

// class mcp4xxx_digipot_spi_component : public mcp4xxx_digipot_base_component,
//               public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
//                      spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_200KHZ> {
//  using mcp4xxx_digipot_base_component::mcp4xxx_digipot_base_component;
//  public:
//   bool mcp4xxx_write_(const uint8_t *data, size_t len) override;
//   bool mcp4xxx_read_(uint8_t *data, size_t len) override;
//   void dump_config() override;

// protected:
//   write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits) override;
//   void communication_init_() override;
// };

class MCP4XXXWiper : public output::FloatOutput, public Parented<mcp4xxx_digipot_base_component> {
 public:
  MCP4XXXWiper(mcp4xxx_digipot_base_component *parent, MCP4XXXWiperID wiper) : parent_(parent), wiper_(wiper) {}
  /// @brief Set level of wiper
  /// @param[in] state - lkjh
  void set_wiper_level(uint16_t level);
  /// @brief Increase wiper by 1 tap, until max value is reached
  uint16_t increase_wiper();
  /// @brief Decrease wiper by 1 tap, until min value is reached
  uint16_t decrease_wiper();
    /// @brief Increase wiper by 1 tap blindly
  void increase_wiper_fast();
  /// @brief Decrease wiper by 1 tap blindly
  void decrease_wiper_fast();
  void set_terminals(bool con_a, bool con_w, bool con_b);
  void enter_shutdown();
  void exit_shutdown();
  // void set_initial_conditions(float level, bool terminal_a, bool terminal_w, bool terminal_b);

 protected:
  void write_state(float state) override;

  mcp4xxx_digipot_base_component *parent_;
  MCP4XXXWiperID wiper_;
  // VolatileWiperState volatile_wiper_state_;

};


}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
