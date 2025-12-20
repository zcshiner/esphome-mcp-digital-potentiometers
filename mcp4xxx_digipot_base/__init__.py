import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_INITIAL_VALUE

CODEOWNERS = ["@zcshiner"]

CONF_NUM_DIGIPOTS = 'num_digipots'
CONF_TERMINAL_A = "terminal_a"
CONF_TERMINAL_B = "terminal_b"
CONF_TERMINAL_W = "terminal_w"

mcp4xxx_digipot_base_ns = cg.esphome_ns.namespace("mcp4xxx_digipot_base")
mcp4xxx_digipot_base_component = mcp4xxx_digipot_base_ns.class_("mcp4xxx_digipot_base_component", cg.Component)

MCP4XXXWiperID = mcp4xxx_digipot_base_ns.enum("MCP4XXXWiperID")

BASE_CONFIG_SCHEMA = (
    cv.Schema({})
    .extend(cv.COMPONENT_SCHEMA)
)

OUTPUT_OPTIONAL_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_TERMINAL_A, default=True): cv.boolean,
        cv.Optional(CONF_TERMINAL_B, default=True): cv.boolean,
        cv.Optional(CONF_TERMINAL_W, default=True): cv.boolean,
        cv.Optional(CONF_INITIAL_VALUE, default=0.5): cv.float_range(min=0.0, max=1.0),
    }
)