#include "mcp4xxx_digipot_base.h"

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

static const char *const TAG = "mcp4xxx_digipot_base";
// constexpr uint8_t EEPROM_WRITE_TIMEOUT_MS = 10;

void mcp4xxx_digipot_base_component::setup() { 
  ESP_LOGD(TAG, "Setting up MCP4XXX");
  this->communication_init_();
  ESP_LOGV(TAG, "Setup finished");
}

void mcp4xxx_digipot_base_component::dump_config_base() {
  ESP_LOGCONFIG(TAG, "mcp4461:");
}

// inline uint8_t mcp4xxx_digipot_base_component::create_command_byte_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits) {
//   // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
//   // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
//   return (address << 4) | (command & 0x0C) | ((data_bits & 0x100) >> 8);
// }

// inline uint8_t mcp4xxx_digipot_base_component::create_data_byte_(uint16_t data_bits) {
//   // Data byte format: D7 D6 D5 D4 D3 D2 D1 D0
//   return static_cast<uint8_t>(data_bits & 0xFF);
// }

// volatile wiper only
bool mcp4xxx_digipot_base_component::set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value) { //todo max value variable
  if (value > MCP4XXX_MAX_VALUE) {
    ESP_LOGE(TAG, "Invalid wiper value: %d (max: %d)", value, MCP4XXX_MAX_VALUE);
    return false;
  }
  ESP_LOGD(TAG, "Setting wiper %d to tap %d of %d", wiper, value, MCP4XXX_MAX_VALUE);

  this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), MCP4XXXCommands::WRITE, value);

  // ESP_LOGV(TAG, "Wrote wiper value: command=0x%02X, data=0x%02X", data[0], data[1]);
  return true;
}

uint16_t mcp4xxx_digipot_base_component::read_wiper_value_(MCP4XXXWiperID wiper) {

  uint16_t response;
  this->read_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), &response);

  // ESP_LOGV(TAG, "Read wiper value: command=0x%02X, response=0x%02X.%02X",
  //           command_byte, data[0], data[1]);

  return response;
}

bool mcp4xxx_digipot_base_component::increment_wiper_(MCP4XXXWiperID wiper) {

  if (this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), MCP4XXXCommands::INCREMENT)) {
    ESP_LOGD(TAG, "Incremented wiper toward terminal A");
    // ESP_LOGV(TAG, "Incremented wiper: command=0x%02X", command_byte);
    return true;
  }
  return false;
}

bool mcp4xxx_digipot_base_component::decrement_wiper_(MCP4XXXWiperID wiper) {

  if (this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), MCP4XXXCommands::DECREMENT)) {
    ESP_LOGD(TAG, "Decremented wiper toward terminal B");
    // ESP_LOGV(TAG, "Decremented wiper: command=0x%02X", command_byte);
    return true;
  }
  return false;
}

uint16_t mcp4xxx_digipot_base_component::read_tcon_register_(MCP4XXX_TCON_N tcon_id_) {

  uint16_t response;
  this->read_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(tcon_id_), &response);

  // ESP_LOGV(TAG, "Read TCON register: command=0x%02X, response=0x%02X.%02X aka " BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN,
  //         command_byte, data[0], data[1], BYTE_TO_BINARY(data[0]), BYTE_TO_BINARY(data[1]));
  return response;
}

bool mcp4xxx_digipot_base_component::write_tcon_register_(MCP4XXX_TCON_N tcon_id_, uint16_t value) {
  this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(tcon_id_), MCP4XXXCommands::WRITE, value);

  // ESP_LOGV(TAG, "Wrote TCON register: command=0x%02X, data=0x%02X aka " BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN,
  //         data[0], data[1], BYTE_TO_BINARY(data[0]), BYTE_TO_BINARY(data[1]));
  return true;
}

bool mcp4xxx_digipot_base_component::set_terminal_connection_(MCP4XXXWiperID wiper, bool connect_a, bool connect_w, bool connect_b) {
  MCP4XXX_TCON_N tcon_id;
  if (wiper == WIPER_0 || wiper == WIPER_1) {
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_0;

  } else { // WIPER_2 and WIPER_3
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_1;
  }

  uint16_t tcon_value = this->read_tcon_register_(tcon_id);
  uint16_t tcon_update = 0;

  if (connect_b) { tcon_update += 0b00000001; }
  if (connect_w) { tcon_update += 0b00000010; }
  if (connect_a) { tcon_update += 0b00000100; }

  uint16_t bitmask = 0b00000111;  // 3 bits for wiper 0

  if (wiper == WIPER_1 || wiper == WIPER_3) {
    tcon_update = tcon_update << 4;
    bitmask = bitmask << 4;
  }
  tcon_value = (tcon_value & ~bitmask) | (tcon_update & bitmask);
  
  ESP_LOGD(TAG, "Setting terminal connections - A:%s, W:%s, B:%s (TCON=0x%02X)",
           connect_a ? "ON" : "OFF", connect_w ? "ON" : "OFF", connect_b ? "ON" : "OFF", tcon_value);

  return this->write_tcon_register_(tcon_id, tcon_value);
}

bool mcp4xxx_digipot_base_component::set_wiper_enter_shutdown_(MCP4XXXWiperID wiper) {
  MCP4XXX_TCON_N tcon_id;
  if (wiper == WIPER_0 || wiper == WIPER_1) {
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_0;

  } else { // WIPER_2 and WIPER_3
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_1;
  }

  uint16_t tcon_value = this->read_tcon_register_(tcon_id);

  uint16_t tcon_update = 0b00001000;
  uint16_t bitmask = 0b00001000;  // bit 4 for wiper 0
  if (wiper == WIPER_1 || wiper == WIPER_3) {
    tcon_update = tcon_update << 4;
    bitmask = bitmask << 4;
  }
  tcon_value = (tcon_value & ~bitmask) | (~tcon_update & bitmask);

  ESP_LOGD(TAG, "Enabling hardware shutdown of wiper, (TCON=0x%02X)", tcon_value);

  return this->write_tcon_register_(tcon_id, tcon_value);
}

bool mcp4xxx_digipot_base_component::set_wiper_exit_shutdown_(MCP4XXXWiperID wiper) {
  MCP4XXX_TCON_N tcon_id;
  if (wiper == WIPER_0 || wiper == WIPER_1) {
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_0;

  } else { // WIPER_2 and WIPER_3
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_1;
  }

  uint16_t tcon_value = this->read_tcon_register_(tcon_id);

  uint16_t tcon_update = 0b00000000;
  uint16_t bitmask = 0b00001000;  // bit 4 for wiper 0
  if (wiper == WIPER_1 || wiper == WIPER_3) {
    tcon_update = tcon_update << 4;
    bitmask = bitmask << 4;
  }
  tcon_value = (tcon_value & ~bitmask) | (~tcon_update & bitmask);

  ESP_LOGD(TAG, "Disable hardware shutdown of wiper, (TCON=0x%02X)", tcon_value);

  return this->write_tcon_register_(tcon_id, tcon_value);
}

bool mcp4xxx_digipot_i2c_component::write_mcp4xxx_register_(MCP4XXXAddresses address, MCP4XXXCommands command, uint16_t data_bits) {
  uint8_t data[2];

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  data[0] = (address << 4) | (command & 0x0C) | ((data_bits & 0x100) >> 8);

  // commands with no data send 1 byte
  if (data_bits == 0) {
    return this->write(data, 1) != i2c::ERROR_OK;

  } else {
    // Data byte format: D7 D6 D5 D4 D3 D2 D1 D0
    data[1] = static_cast<uint8_t>(data_bits & 0xFF);
    return this->write(data, 2) != i2c::ERROR_OK;
  }
}

// TODO: set warnings or return error codes? or both?
bool mcp4xxx_digipot_i2c_component::read_mcp4xxx_register_(MCP4XXXAddresses address, uint16_t *data) {
  uint8_t buffer[2]; // reads always return 2 bytes

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  uint8_t command_byte = (address << 4) | (MCP4XXXCommands::READ & 0x0C);
  this->write(&command_byte, 1);

  if (this->read(buffer, 2) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Read failed");
    this->status_set_warning();
    return 1;
  }

  this->status_clear_warning();
  *data = encode_uint16(buffer[0], buffer[1]);
  return 0;
}

// bool mcp4xxx_digipot_i2c_component::mcp4xxx_write_(const uint8_t *data, size_t len) {
//   return this->write(data, len) != i2c::ERROR_OK;
// }

// bool mcp4xxx_digipot_i2c_component::mcp4xxx_read_(uint8_t *data, size_t len) {
//   if (this->read(data, len) != i2c::ERROR_OK) {
//     ESP_LOGW(TAG, "Read failed");
//     this->status_set_warning();
//     return 1;
//   }
//   this->status_clear_warning();
//   return 0;
// }

void mcp4xxx_digipot_i2c_component::dump_config() {
  this->dump_config_base();
  LOG_I2C_DEVICE(this);
}

inline void mcp4xxx_digipot_i2c_component::communication_init_() {
  if (this->write(nullptr, 0)) {
    // this->error_code_ = MCP4461_STATUS_I2C_ERROR;
    ESP_LOGE(TAG, "Communication with device failed during setup");
    this->mark_failed();
    // return;
  }
}

// bool mcp4xxx_digipot_spi_component::mcp4xxx_write_(const uint8_t *data, size_t len) {
//   enable();
//   write_array(data, len);
//   disable();
//   return false; // return 0 on success to match the i2c version
// }

// bool mcp4xxx_digipot_spi_component::mcp4xxx_read_(uint8_t *data, size_t len) {
//   this->enable();
//   this->read_array(data, len);
//   // this->transfer_array(data, len);
//   this->disable();
//   return false; // return 0 on success to match the i2c version
// }

// void mcp4xxx_digipot_spi_component::dump_config() {
//   this->dump_config_base();
// }

// inline void mcp4xxx_digipot_spi_component::communication_init_() {
//   this->spi_setup();
// }

// // note to self. this makes me feel gross... consider going back to enum class
// bool mcp4xxx_nonvolatile_memory::set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value, bool nonvolatile) {
//   if (nonvolatile) {
//     switch (wiper) {
//       case MCP4XXXWiperID::WIPER_0:
//         wiper = (MCP4XXXWiperID)MCP4XXXAddresses::MCP4XXX_NVW0;
//         break;
//       case MCP4XXXWiperID::WIPER_1:
//         wiper = (MCP4XXXWiperID)MCP4XXXAddresses::MCP4XXX_NVW1;
//         break;
//       case MCP4XXXWiperID::WIPER_2:
//         wiper = (MCP4XXXWiperID)MCP4XXXAddresses::MCP4XXX_NVW2;
//         break;
//       case MCP4XXXWiperID::WIPER_3:
//         wiper = (MCP4XXXWiperID)MCP4XXXAddresses::MCP4XXX_NVW3;
//         break;
//       default:
//         ESP_LOGE(TAG, "Invalid volatile wiper specified for nonvolatile write");
//         return false;
//     }
//   }
//   return set_wiper_value_(wiper, value);
// }

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
