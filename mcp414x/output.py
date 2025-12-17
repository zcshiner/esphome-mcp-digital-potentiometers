import esphome.codegen as cg
from esphome.components import output
import esphome.config_validation as cv
from esphome.const import CONF_CHANNEL, CONF_ID, CONF_INITIAL_VALUE
from ..mcp4xxx_digipot_base import CONF_TERMINAL_A, CONF_TERMINAL_B, CONF_TERMINAL_W, mcp4xxx_digipot_base_component, mcp4xxx_digipot_base_ns, MCP4XXXWiperID

DEPENDENCIES = ["mcp4xxx_digipot_base"]
CONF_MCP4XXX_ID = "mcp414x_id"

mcp4xxx_wiper = mcp4xxx_digipot_base_ns.class_(
    "MCP4XXXWiper", output.FloatOutput, cg.Parented.template(mcp4xxx_digipot_base_component)
)

CHANNEL_OPTIONS = {
    '0': MCP4XXXWiperID.WIPER_0,
    '1': MCP4XXXWiperID.WIPER_1,
}

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.declare_id(mcp4xxx_wiper),
        cv.GenerateID(CONF_MCP4XXX_ID): cv.use_id(mcp4xxx_digipot_base_component),            
        cv.Required(CONF_CHANNEL): cv.enum(CHANNEL_OPTIONS, upper=True),
        cv.Optional(CONF_TERMINAL_A, default=True): cv.boolean,
        cv.Optional(CONF_TERMINAL_B, default=True): cv.boolean,
        cv.Optional(CONF_TERMINAL_W, default=True): cv.boolean,
        cv.Optional(CONF_INITIAL_VALUE, default=0.5): cv.float_range(min=0.0, max=1.0),
    }
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