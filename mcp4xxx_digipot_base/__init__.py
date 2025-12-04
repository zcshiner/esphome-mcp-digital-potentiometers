import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import coroutine, ID

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True
CONF_DISABLE_WIPER_0 = "disable_wiper_0"
CONF_DISABLE_WIPER_1 = "disable_wiper_1"
CONF_DISABLE_WIPER_2 = "disable_wiper_2"
CONF_DISABLE_WIPER_3 = "disable_wiper_3"

mcp4xxx_digipot_base_ns = cg.esphome_ns.namespace("mcp4xxx_digipot_base")
mcp4xxx_digipot_base_component = mcp4xxx_digipot_base_ns.class_("mcp4xxx_digipot_base_component", cg.Component, i2c.I2CDevice)

Mcp4461WiperIdx = mcp4xxx_digipot_base_ns.enum("Mcp4461WiperIdx", is_class=True)

CONF_MCP4461_ID = "mcp4461_id"

BASE_CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Optional(CONF_DISABLE_WIPER_0, default=False): cv.boolean,
            cv.Optional(CONF_DISABLE_WIPER_1, default=False): cv.boolean,
            cv.Optional(CONF_DISABLE_WIPER_2, default=False): cv.boolean,
            cv.Optional(CONF_DISABLE_WIPER_3, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x2C))
)

@coroutine
async def register_mcp4xxx(config):
    id: ID = config[CONF_ID]
    var = cg.new_Pvariable(
        config[CONF_ID],
    )
    if config[CONF_DISABLE_WIPER_0]:
        cg.add(var.initialize_wiper_disabled(Mcp4461WiperIdx.MCP4461_WIPER_0))
    if config[CONF_DISABLE_WIPER_1]:
        cg.add(var.initialize_wiper_disabled(Mcp4461WiperIdx.MCP4461_WIPER_1))
    if config[CONF_DISABLE_WIPER_2]:
        cg.add(var.initialize_wiper_disabled(Mcp4461WiperIdx.MCP4461_WIPER_2))
    if config[CONF_DISABLE_WIPER_3]:
        cg.add(var.initialize_wiper_disabled(Mcp4461WiperIdx.MCP4461_WIPER_3))

    await cg.register_component(var, config)
    return var
