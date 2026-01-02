#include "mcp4xxx_digipot_i2c.h"

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mcp4xxx_digipot_i2c {

static const char *const TAG = "mcp4xxx_digipot_i2c";

bool mcp4xxx_digipot_i2c_component::write_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address,
                                          mcp4xxx_digipot_base::MCP4XXXCommands command, uint16_t data_bits) {
  uint8_t data[2];

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  data[0] = (address << 4) | (command & 0x0C) | ((data_bits & 0x100) >> 8);

  if (   command == mcp4xxx_digipot_base::MCP4XXXCommands::INCREMENT
      || command == mcp4xxx_digipot_base::MCP4XXXCommands::DECREMENT) {
    ESP_LOGV(TAG, "Wrote increment/decrement command: command=0x%02X 0b" BYTE_TO_BINARY_PATTERN,
         data[0], BYTE_TO_BINARY(data[0]));
    return this->write(data, 1) != i2c::ERROR_OK;

  } else {
    // Data byte format: D7 D6 D5 D4 D3 D2 D1 D0
    data[1] = static_cast<uint8_t>(data_bits & 0xFF);
    ESP_LOGV(TAG, "Wrote two bytes: data=0x%x 0b" BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
         data_bits, BYTE_TO_BINARY(data[0]), BYTE_TO_BINARY(data[1]));
    return this->write(data, 2) != i2c::ERROR_OK;
  }
}

bool mcp4xxx_digipot_i2c_component::read_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address,
                                                           uint16_t *data) {
  uint8_t buffer[2]; // reads always return 2 bytes

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  uint8_t command_byte = (address << 4) | (mcp4xxx_digipot_base::MCP4XXXCommands::READ & 0x0C);
  this->write(&command_byte, 1);

  if (this->read(buffer, 2) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Read failed");
    return 1;
  }
  ESP_LOGV(TAG, "Read register: command=0x%02X, response=0x%02X.%02X 0b"
            BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
            command_byte, buffer[0], buffer[1], BYTE_TO_BINARY(buffer[0]), BYTE_TO_BINARY(buffer[1]));

  *data = encode_uint16(buffer[0], buffer[1]);
  return 0;
}

void mcp4xxx_digipot_i2c_component::dump_config() {
  this->dump_config_base_();
  LOG_I2C_DEVICE(this);
}

inline void mcp4xxx_digipot_i2c_component::communication_init_() {
  if (this->write(nullptr, 0)) {
    ESP_LOGE(TAG, "Comm failed during setup");
    this->mark_failed();
  }
}

}  // namespace mcp4xxx_digipot_i2c
}  // namespace esphome
