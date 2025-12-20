import esphome.codegen as cg
from esphome.components import output
import esphome.config_validation as cv
from esphome.const import CONF_CHANNEL, CONF_ID
from .. import mcp4xxx_digipot_base

DEPENDENCIES = ["mcp4xxx_digipot_i2c"]
CONF_MCP4XXX_ID = "mcp446x_id"

mcp4xxx_wiper = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_(
    "MCP4XXXWiper", output.FloatOutput, cg.Parented.template(mcp4xxx_digipot_base.mcp4xxx_digipot_base_component)
)

CHANNEL_OPTIONS = {
    '0': mcp4xxx_digipot_base.MCP4XXXWiperID.WIPER_0,
    '1': mcp4xxx_digipot_base.MCP4XXXWiperID.WIPER_1,
    '2': mcp4xxx_digipot_base.MCP4XXXWiperID.WIPER_2,
    '3': mcp4xxx_digipot_base.MCP4XXXWiperID.WIPER_3,
}

CONFIG_SCHEMA = (
    output.FLOAT_OUTPUT_SCHEMA
    .extend(
        {
            cv.Required(CONF_ID): cv.declare_id(mcp4xxx_wiper),
            cv.GenerateID(CONF_MCP4XXX_ID): cv.use_id(mcp4xxx_digipot_base.mcp4xxx_digipot_base_component),            
            cv.Required(CONF_CHANNEL): cv.enum(CHANNEL_OPTIONS, upper=True)
        }
    )
    # .extend(mcp4xxx_digipot_base.OUTPUT_OPTIONAL_CONFIG_SCHEMA)
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_MCP4XXX_ID])
    var = cg.new_Pvariable(
        config[CONF_ID],
        parent,
        config[CONF_CHANNEL],
    )

    await output.register_output(var, config)
    await cg.register_parented(var, config[CONF_MCP4XXX_ID])