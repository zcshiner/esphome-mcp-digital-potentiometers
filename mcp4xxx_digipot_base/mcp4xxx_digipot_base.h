#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/output/float_output.h"
#include "esphome/core/automation.h"

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

template <typename T, enable_if_t<std::is_unsigned<T>::value, int> = 0>
constexpr T replace_bits(T target, T bitmask, T new_bits) {
  return (target & ~bitmask) | (new_bits & bitmask);
}

// Terminal Connection Registers, not all devices have both TCON registers
enum MCP4XXX_TCON_N : uint8_t {
  MCP4XXX_TCON_0 = MCP4XXXAddresses::MCP4XXX_TCON0,
  MCP4XXX_TCON_1 = MCP4XXXAddresses::MCP4XXX_TCON1
};

class mcp4xxx_digipot_base_component : public Component {
 public:
  mcp4xxx_digipot_base_component(uint16_t digipot_taps) : MCP4XXX_MAX_VALUE(digipot_taps) {}
  void setup() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  /// @brief Get maximum tap position of wiper, either 7 or 8 bit;
  /// @brief This value is the same for all wipers within a digital potentiometer
  /// @return Maximum tap position as integer
  uint16_t get_tap_count() const { return MCP4XXX_MAX_VALUE; }

 protected:
  friend class MCP4XXXWiper;
  friend class mcp4xxx_nonvolatile_memory;
  void dump_config_base_();
  uint16_t build_tcon_payload_(bool connect_a, bool connect_w, bool connect_b, bool hw_config);
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
  uint8_t tcon_initial_value[2] = {0xFF, 0xFF};
  uint16_t wiper_initial_value[4] = {0xFF, 0xFF, 0xFF, 0xFF};
};

class MCP4XXXWiper : public output::FloatOutput, public Parented<mcp4xxx_digipot_base::mcp4xxx_digipot_base_component> {
 public:
  MCP4XXXWiper(mcp4xxx_digipot_base_component *parent, MCP4XXXWiperID wiper) : parent_(parent), wiper_(wiper) {}
  /// @brief Set level of wiper
  /// @param level integer level to set the wiper to, use writestate(float) for float levels
  void set_wiper_level(uint16_t level);
  /// @brief Increase wiper by 1 tap until max value is reached
  uint16_t increase_wiper();
  /// @brief Decrease wiper by 1 tap until min value is reached
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
  /// @brief Get maximum tap position of wiper, either 7 or 8 bit;
  /// @brief This value is the same for all wipers within a digital potentiometer
  uint16_t get_tap_count() { return this->parent_->get_tap_count(); }
  /// @brief Set initial terminal connections for use during component setup
  /// @param con_a Terminal A connection state
  /// @param con_w Terminal W connection state
  /// @param con_b Terminal B connection state
  void set_initial_terminals_(bool con_a, bool con_w, bool con_b);
  /// @brief Set initial wiper position for use during component setup
  /// @param state Float state between 0.0 and 1.0
  void set_initial_state_(float state);

 protected:
  void write_state(float state) override;

  mcp4xxx_digipot_base_component *parent_;
  MCP4XXXWiperID wiper_;

};

template<typename... Ts> class IncreaseAction : public Action<Ts...>, public Parented<MCP4XXXWiper> {
 public:
  explicit IncreaseAction(MCP4XXXWiper *parent) : parent_(parent) {}
  void play(const Ts &...x) override { this->parent_->increase_wiper(); }

 protected:
  MCP4XXXWiper *parent_;
};

template<typename... Ts> class DecreaseAction : public Action<Ts...>, public Parented<MCP4XXXWiper> {
 public:
  explicit DecreaseAction(MCP4XXXWiper *parent) : parent_(parent) {}
  void play(const Ts &...x) override { this->parent_->decrease_wiper(); }

 protected:
  MCP4XXXWiper *parent_;
};

template<typename... Ts> class SetWiperValueAction : public Action<Ts...>, public Parented<MCP4XXXWiper> {
 public:
  explicit SetWiperValueAction(MCP4XXXWiper *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint16_t, level)

  void play(const Ts &...x) override {
    auto level = this->level_.value(x...);
    this->parent_->set_wiper_level(level);
  }

  protected:
  MCP4XXXWiper *parent_;
};

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
