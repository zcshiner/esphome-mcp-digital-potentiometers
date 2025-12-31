import esphome.codegen as cg
from esphome.components import output
import esphome.config_validation as cv
from esphome.const import CONF_CHANNEL, CONF_ID, CONF_LEVEL
from .. import mcp4xxx_digipot_base
from esphome import automation
from . import DIGIPOT_TAPS

DEPENDENCIES = ["mcp4xxx_digipot_i2c"]
CONF_MCP4XXX_ID = "mcp446x_id"

mcp4xxx_wiper = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_(
    "MCP4XXXWiper", output.FloatOutput, cg.Parented.template(mcp4xxx_digipot_base.mcp4xxx_digipot_base_component)
)
IncreaseAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("IncreaseAction", automation.Action)
DecreaseAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("DecreaseAction", automation.Action)
SetWiperValueAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("SetWiperValueAction", automation.Action)
SetTerminalsAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("SetTerminalsAction", automation.Action)
EnterShutdownAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("EnterShutdownAction", automation.Action)
ExitShutdownAction = mcp4xxx_digipot_base.mcp4xxx_digipot_base_ns.class_("ExitShutdownAction", automation.Action)

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
            cv.GenerateID(CONF_ID): cv.declare_id(mcp4xxx_wiper), ## apply to other series swap of gen vs req
            cv.Required(CONF_MCP4XXX_ID): cv.use_id(mcp4xxx_digipot_base.mcp4xxx_digipot_base_component),            
            cv.Required(CONF_CHANNEL): cv.enum(CHANNEL_OPTIONS, upper=True)
        }
    )
    .extend(mcp4xxx_digipot_base.OUTPUT_OPTIONAL_CONFIG_SCHEMA)
)

WIPER_ONLY_ACTION_SCHEMA = automation.maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
        }
    )

SET_LEVEL_ACTION_SCHEMA = cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
            cv.Required(CONF_LEVEL): cv.templatable(cv.int_range(min=0, max=DIGIPOT_TAPS)),
        }
    )

SET_TERMINALS_ACTION_SCHEMA = cv.Schema(
        {
        cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
        cv.Optional(mcp4xxx_digipot_base.CONF_TERMINAL_A, default=True): cv.boolean,
        cv.Optional(mcp4xxx_digipot_base.CONF_TERMINAL_B, default=True): cv.boolean,
        cv.Optional(mcp4xxx_digipot_base.CONF_TERMINAL_W, default=True): cv.boolean,
        }
    )

@automation.register_action("output.increment_wiper", IncreaseAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_increase_wiper(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

@automation.register_action("output.decrement_wiper", DecreaseAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_decrease_wiper(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

@automation.register_action("output.set_wiper_level", SetWiperValueAction, SET_LEVEL_ACTION_SCHEMA)
async def output_set_wiper_level(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

@automation.register_action("output.set_terminals", SetTerminalsAction, SET_TERMINALS_ACTION_SCHEMA)
async def output_set_terminals(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

@automation.register_action("output.enter_shutdown", EnterShutdownAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_enter_shutdown(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

@automation.register_action("output.exit_shutdown", ExitShutdownAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_exit_shutdown(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_MCP4XXX_ID])
    var = cg.new_Pvariable(
        config[CONF_ID],
        parent,
        config[CONF_CHANNEL],
    )

    await output.register_output(var, config)
    await cg.register_parented(var, config[CONF_MCP4XXX_ID])
    await mcp4xxx_digipot_base.set_initial_conditions(var, config)