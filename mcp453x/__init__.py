import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from .. import mcp4xxx_digipot_i2c, mcp4xxx_digipot_base
from esphome.const import CONF_ID, CONF_ADDRESS

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["mcp4xxx_digipot_i2c"]
MULTI_CONF = True

DIGIPOT_TAPS = 128 #7-bit
DEFAULT_ADDRESS = 0x2E

# MCP453X ‘0101 11’b + A0
I2C_ADDRESS_OPTIONS = [
    0x2E, #0b0101110
    0x2F  #0b0101111
]

mcp453x_ns = cg.esphome_ns.namespace("mcp453x")
mcp453X = mcp453x_ns.class_("MCP453X", mcp4xxx_digipot_i2c.mcp4xxx_digipot_i2c_component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(mcp453X),
        }
    )
    .extend(mcp4xxx_digipot_base.BASE_CONFIG_SCHEMA)
    .extend(i2c.i2c_device_schema(None))
    .extend(cv.Schema(
                {
                    cv.Optional(CONF_ADDRESS, default = DEFAULT_ADDRESS): cv.one_of(*I2C_ADDRESS_OPTIONS, int=True)
                }
            )
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], DIGIPOT_TAPS)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
