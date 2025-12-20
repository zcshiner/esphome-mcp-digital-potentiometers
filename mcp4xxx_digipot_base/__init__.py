import esphome.codegen as cg
import esphome.config_validation as cv

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

