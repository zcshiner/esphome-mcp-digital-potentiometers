#include "mcp4xxx_digipot_spi.h"

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mcp4xxx_digipot_spi {

static const char *const TAG = "mcp4xxx_digipot_spi";

bool mcp4xxx_digipot_spi_component::check_spi_CMDERR_(uint8_t *data) {
  // // check response for error, it's always in the first byte
  uint8_t error_code = (data[0] & 0b00000010) >> 1;
  ESP_LOGV(TAG, "SPI CMDERR check: response=0b" BYTE_TO_BINARY_PATTERN ", CMDERR=%d",
           BYTE_TO_BINARY(data[0]), error_code);
  if (error_code == 0) {
    ESP_LOGE(TAG, "Write command returned CMDERR");
    return true; // return non-zero on failure
  }
  return false; // return 0 on success
}

bool mcp4xxx_digipot_spi_component::write_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address,
                                          mcp4xxx_digipot_base::MCP4XXXCommands command, uint16_t data_bits) {
  uint8_t buffer[2] = {0x00, 0xFF};

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  // With SPI methods, we need to set D9 to 1 for multiplexed SDI/SDO pin use
  buffer[0] = (address << 4) | (command & 0x0C) | 0x02 | ((data_bits & 0x100) >> 8);
  
  this->enable();
  if (   command == mcp4xxx_digipot_base::MCP4XXXCommands::INCREMENT
      || command == mcp4xxx_digipot_base::MCP4XXXCommands::DECREMENT) {
    this->transfer_array(buffer, 1);
    this->disable();
    ESP_LOGV(TAG, "Wrote increment/decrement command: command=0x%02X 0b" BYTE_TO_BINARY_PATTERN,
             buffer[0], BYTE_TO_BINARY(buffer[0]));

  } else {
    // Data byte format: D7 D6 D5 D4 D3 D2 D1 D0
    buffer[1] = static_cast<uint8_t>(data_bits & 0xFF);
    this->transfer_array(buffer, 2);
    this->disable();
    ESP_LOGV(TAG, "Wrote two bytes: data=0x%x 0b" BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
             data_bits, BYTE_TO_BINARY(buffer[0]), BYTE_TO_BINARY(buffer[1]));
  }
  return check_spi_CMDERR_(buffer);
}

bool mcp4xxx_digipot_spi_component::read_mcp4xxx_register_(mcp4xxx_digipot_base::MCP4XXXAddresses address, uint16_t *data) {
  uint8_t buffer[2] = {0x00, 0xFF};

  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0, D9 unused
  // With SPI methods, we need to set D9 through D0 to 1 for multiplexed SDI/SDO pin use
  buffer[0] = (address << 4) | (mcp4xxx_digipot_base::MCP4XXXCommands::READ & 0x0C) | 0x03;

  ESP_LOGV(TAG, "Buffer before read: 0x%02X.%02X 0b"
            BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
            buffer[0], buffer[1], BYTE_TO_BINARY(buffer[0]), BYTE_TO_BINARY(buffer[1]));

  this->enable();
  this->transfer_array(buffer, 2); // reads always return 2 bytes
  this->disable();
  ESP_LOGV(TAG, "Read register: transferred 0x%02X.%02X 0b"
            BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
            buffer[0], buffer[1], BYTE_TO_BINARY(buffer[0]), BYTE_TO_BINARY(buffer[1]));

  if (check_spi_CMDERR_(buffer)) {
    return true;
  }

  *data = encode_uint16(buffer[0] & 0x01, buffer[1]);
  return false; // return 0 on success
}

void mcp4xxx_digipot_spi_component::dump_config() {
  this->dump_config_base_();
  LOG_PIN("  CS Pin: ", this->cs_);
}

inline void mcp4xxx_digipot_spi_component::communication_init_() {
  this->spi_setup();
}

}  // namespace mcp4xxx_digipot_spi
}  // namespace esphome
