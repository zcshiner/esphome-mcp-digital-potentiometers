import esphome.codegen as cg
from esphome.components import spi
import esphome.config_validation as cv
from .. import mcp4xxx_digipot_base
from esphome.const import CONF_ID

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["mcp4xxx_digipot_base"]
MULTI_CONF = True
# DIGIPOT_TAPS = 128 #7-bit

mcp414x_ns = cg.esphome_ns.namespace("mcp414x")
Mcp414xComponent = mcp414x_ns.class_("Mcp414xComponent", mcp4xxx_digipot_base.mcp4xxx_digipot_spi_component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(Mcp414xComponent),
        }
    )
    .extend(mcp4xxx_digipot_base.BASE_CONFIG_SCHEMA)
    .extend(spi.spi_device_schema(cs_pin_required=True))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    # if config[mcp4xxx_digipot_base.CONF_DISABLE_WIPER_0]:
    #     cg.add(var.initialize_wiper_disabled(mcp4xxx_digipot_base.Mcp4461WiperIdx.MCP4461_WIPER_0))
    # if config[mcp4xxx_digipot_base.CONF_DISABLE_WIPER_1]:
    #     cg.add(var.initialize_wiper_disabled(mcp4xxx_digipot_base.Mcp4461WiperIdx.MCP4461_WIPER_1))
    # if config[mcp4xxx_digipot_base.CONF_DISABLE_WIPER_2]:
    #     cg.add(var.initialize_wiper_disabled(mcp4xxx_digipot_base.Mcp4461WiperIdx.MCP4461_WIPER_2))
    # if config[mcp4xxx_digipot_base.CONF_DISABLE_WIPER_3]:
    #     cg.add(var.initialize_wiper_disabled(mcp4xxx_digipot_base.Mcp4461WiperIdx.MCP4461_WIPER_3))

    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
