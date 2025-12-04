import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from .. import mcp4xxx_digipot_base
from esphome.const import CONF_ID

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["mcp4xxx_digipot_base"]
MULTI_CONF = True

mcp446x_ns = cg.esphome_ns.namespace("mcp446x")
Mcp446xComponent = mcp446x_ns.class_("Mcp446xComponent", mcp4xxx_digipot_base.mcp4xxx_digipot_base_component, i2c.I2CDevice)
CONF_MCP4461_ID = "mcp4461_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(Mcp446xComponent),
        }
    )
    .extend(mcp4xxx_digipot_base.BASE_CONFIG_SCHEMA)
)

async def to_code(config):
    var = await mcp4xxx_digipot_base.register_mcp4xxx(config)
    await i2c.register_i2c_device(var, config)
