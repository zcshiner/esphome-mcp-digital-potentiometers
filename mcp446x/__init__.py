import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from .. import mcp4xxx_digipot_base
from esphome.const import CONF_ID, CONF_ADDRESS

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["mcp4xxx_digipot_base"]
MULTI_CONF = True

# NUM_DIGIPOTS = 4
DIGIPOT_TAPS = 257 #8-bit
DEFAULT_ADDRESS = 0x2C

# MCP446X ‘0101 1’b + A1:A0
I2C_ADDRESS_OPTIONS = [
    0x2C, #0b0101100
    0x2D, #0b0101101
    0x2E, #0b0101110
    0x2F  #0b0101111
]

mcp446x_ns = cg.esphome_ns.namespace("mcp446x")
mcp446X = mcp446x_ns.class_("MCP446X", mcp4xxx_digipot_base.mcp4xxx_digipot_i2c_component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(mcp446X),
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
    config[mcp4xxx_digipot_base.CONF_NUM_DIGIPOTS] = 4
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
