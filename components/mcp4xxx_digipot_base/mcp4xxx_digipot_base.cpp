#include "mcp4xxx_digipot_base.h"

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace mcp4xxx_digipot_base {

static const char *const TAG = "mcp4xxx_digipot_base";

void mcp4xxx_digipot_base_component::setup() { 
  ESP_LOGD(TAG, "Setting up");
  this->communication_init_();

  uint8_t tcon_lookup[2] = {MCP4XXX_TCON_N::MCP4XXX_TCON_0,
                            MCP4XXX_TCON_N::MCP4XXX_TCON_1};
  uint8_t wiper_lookup[4] = {MCP4XXXWiperID::WIPER_0, MCP4XXXWiperID::WIPER_1,
                             MCP4XXXWiperID::WIPER_2, MCP4XXXWiperID::WIPER_3};

  for (int i = 0; i < sizeof(tcon_lookup); i++) {
    if (this->tcon_initial_value[i] != 0xFF) {
      this->write_tcon_register_(static_cast<MCP4XXX_TCON_N>(tcon_lookup[i]), this->tcon_initial_value[i]);
      ESP_LOGD(TAG, "Initialized TCON_%d to 0x%X", i, this->tcon_initial_value[i]);
      ESP_LOGVV(TAG, "TCON_%d bits: 0b" BYTE_TO_BINARY_PATTERN,
                i, BYTE_TO_BINARY(this->tcon_initial_value[i]));
    }
  }
  for (int j = 0; j < sizeof(wiper_lookup); j++) {
    if (this->wiper_initial_value[j] != 0xFF) {
      this->set_wiper_value_(static_cast<MCP4XXXWiperID>(wiper_lookup[j]), this->wiper_initial_value[j]);
      ESP_LOGD(TAG, "Initialized WIPER_%d to tap %d", j, this->wiper_initial_value[j]);
    }
  }
  ESP_LOGVV(TAG, "Setup finished");
}

void mcp4xxx_digipot_base_component::dump_config_base_() {
  ESP_LOGCONFIG(TAG,
                "MCP4XXX Digipot Base:\n"
                "  Tap Count: %d\n"
                "  Has NV Memory: %s",
                this->MCP4XXX_MAX_VALUE, this->MCP4XXX_HAS_NV_MEMORY ? "true" : "false");
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
}

bool mcp4xxx_digipot_base_component::set_wiper_value_(MCP4XXXWiperID wiper, uint16_t value, bool nonvolatile) {
  if (value > MCP4XXX_MAX_VALUE) {
    ESP_LOGE(TAG, "Invalid wiper value: %d (max: %d)", value, MCP4XXX_MAX_VALUE);
    return true;
  }

  ESP_LOGVV(TAG, "HAS_NV_MEMORY=%s, NONVOLATILE=%s", this->MCP4XXX_HAS_NV_MEMORY ? "true" : "false", nonvolatile ? "true" : "false");
  
  MCP4XXXAddresses wiper_address = static_cast<MCP4XXXAddresses>(wiper);
  if (this->MCP4XXX_HAS_NV_MEMORY && nonvolatile) {
    switch (wiper) {
      case MCP4XXXWiperID::WIPER_0:
        wiper_address = MCP4XXX_NVW0;
        break;
      case MCP4XXXWiperID::WIPER_1:
        wiper_address = MCP4XXX_NVW1;
        break;
      case MCP4XXXWiperID::WIPER_2:
        wiper_address = MCP4XXX_NVW2;
        break;
      case MCP4XXXWiperID::WIPER_3:
        wiper_address = MCP4XXX_NVW3;
        break;
      default:
        break;
    }
    ESP_LOGV(TAG, "Previous non-volatile wiper %d was tap %d of %d",
            wiper, this->read_wiper_value_(wiper_address), MCP4XXX_MAX_VALUE);
    ESP_LOGV(TAG, "Setting non-volatile wiper %d to tap %d of %d", wiper, value, MCP4XXX_MAX_VALUE);
  } else {
    ESP_LOGV(TAG, "Setting volatile wiper %d to tap %d of %d", wiper, value, MCP4XXX_MAX_VALUE);
  }

  return this->write_mcp4xxx_register_(wiper_address, MCP4XXXCommands::WRITE, value);
}

uint16_t mcp4xxx_digipot_base_component::read_wiper_value_(MCP4XXXWiperID wiper) {

  uint16_t response;
  this->read_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), &response);

  ESP_LOGV(TAG, "Read wiper %d got 0x%X", wiper, response);
  return response;
}

bool mcp4xxx_digipot_base_component::increment_wiper_(MCP4XXXWiperID wiper) {

  if (this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), MCP4XXXCommands::INCREMENT)) {
    ESP_LOGV(TAG, "Incremented wiper toward terminal A");
    return true;
  }
  return false;
}

bool mcp4xxx_digipot_base_component::decrement_wiper_(MCP4XXXWiperID wiper) {

  if (this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(wiper), MCP4XXXCommands::DECREMENT)) {
    ESP_LOGV(TAG, "Decremented wiper toward terminal B");
    return true;
  }
  return false;
}

uint16_t mcp4xxx_digipot_base_component::read_tcon_register_(MCP4XXX_TCON_N tcon_id_) {

  uint16_t response;
  this->read_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(tcon_id_), &response);

  ESP_LOGV(TAG, "Read 0x%X from TCON_%s", response, tcon_id_ == MCP4XXX_TCON_N::MCP4XXX_TCON_0 ? "0" : "1");
  return response;
}

bool mcp4xxx_digipot_base_component::write_tcon_register_(MCP4XXX_TCON_N tcon_id_, uint16_t value) {
  this->write_mcp4xxx_register_(static_cast<MCP4XXXAddresses>(tcon_id_), MCP4XXXCommands::WRITE, value);
  
  ESP_LOGV(TAG, "Wrote 0x%X to TCON_%s", value, tcon_id_ == MCP4XXX_TCON_N::MCP4XXX_TCON_0 ? "0" : "1");
  return true;
}

uint16_t mcp4xxx_digipot_base_component::read_status_register_() {

  uint16_t response;
  if(this->read_mcp4xxx_register_(MCP4XXXAddresses::MCP4XXX_STATUS, &response)) {
    ESP_LOGW(TAG, "Failed to read STATUS");
  }

  ESP_LOGV(TAG, "Read 0x%X from STATUS", response);
  return response;
}

uint16_t mcp4xxx_digipot_base_component::build_tcon_payload_(bool connect_a, bool connect_w,
                                                             bool connect_b, bool hw_config) {
  uint16_t tcon_update = 0;

  if (connect_b) { tcon_update += 0b00000001; }
  if (connect_w) { tcon_update += 0b00000010; }
  if (connect_a) { tcon_update += 0b00000100; }
  if (hw_config) { tcon_update += 0b00001000; }

  return tcon_update;
}

bool mcp4xxx_digipot_base_component::set_terminal_connection_(MCP4XXXWiperID wiper,
                                                              bool connect_a, bool connect_w, bool connect_b) {
  MCP4XXX_TCON_N tcon_id;
  if (wiper == WIPER_0 || wiper == WIPER_1) {
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_0;

  } else { // WIPER_2 and WIPER_3
    tcon_id = MCP4XXX_TCON_N::MCP4XXX_TCON_1;
  }

  uint16_t tcon_value = this->read_tcon_register_(tcon_id);
  uint16_t tcon_update = this->build_tcon_payload_(connect_a, connect_w, connect_b, true);
  uint16_t bitmask = 0b00000111;  // 3 bits for wiper 0

  if (wiper == WIPER_1 || wiper == WIPER_3) {
    tcon_update = tcon_update << 4;
    bitmask = bitmask << 4;
  }
  tcon_value = mcp4xxx_digipot_base::replace_bits(tcon_value, bitmask, tcon_update);
  
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
  tcon_value = mcp4xxx_digipot_base::replace_bits(tcon_value, bitmask, static_cast<uint16_t>(~tcon_update));

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
  tcon_value = mcp4xxx_digipot_base::replace_bits(tcon_value, bitmask, static_cast<uint16_t>(~tcon_update));

  ESP_LOGD(TAG, "Disable hardware shutdown of wiper, (TCON=0x%02X)", tcon_value);

  return this->write_tcon_register_(tcon_id, tcon_value);
}

bool mcp4xxx_digipot_base_component::EEPROM_write_active_() {
  uint16_t status = this->read_status_register_();
  uint16_t bitmask = 0b00010000;
  bool write_active = (status & bitmask) == bitmask;
  ESP_LOGD(TAG, "EEPROM write active: %s", write_active ? "YES" : "NO");

  return write_active;
}

}  // namespace mcp4xxx_digipot_base
}  // namespace esphome
