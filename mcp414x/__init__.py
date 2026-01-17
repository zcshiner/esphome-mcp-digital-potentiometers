import esphome.codegen as cg
from esphome.components import spi
import esphome.config_validation as cv
from .. import mcp4xxx_digipot_base, mcp4xxx_digipot_spi
from esphome.const import CONF_ID

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["mcp4xxx_digipot_spi"]
MULTI_CONF = True

DIGIPOT_TAPS = 128 #7-bit
HAS_NV_MEMORY = True

mcp414x_ns = cg.esphome_ns.namespace("mcp414x")
mcp414x = mcp414x_ns.class_("MCP414X", mcp4xxx_digipot_spi.mcp4xxx_digipot_spi_component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(mcp414x),
        }
    )
    .extend(mcp4xxx_digipot_base.BASE_CONFIG_SCHEMA)
    .extend(spi.spi_device_schema(cs_pin_required=True))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], DIGIPOT_TAPS, HAS_NV_MEMORY)
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
