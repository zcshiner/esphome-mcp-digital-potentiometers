import esphome.codegen as cg
from esphome.components import output
import esphome.config_validation as cv
from esphome.const import CONF_CHANNEL, CONF_ID, CONF_LEVEL
from .. import mcp4xxx_digipot_base as dp
from esphome import automation

DEPENDENCIES = ["mcp4xxx_digipot_i2c"]
CONF_MCP4XXX_ID = "mcp446x_id"

mcp4xxx_wiper = dp.mcp4xxx_digipot_base_ns.class_(
    "MCP4XXXWiper", output.FloatOutput, cg.Parented.template(dp.mcp4xxx_digipot_base_component)
)

CHANNEL_OPTIONS = {
    '0': dp.MCP4XXXWiperID.WIPER_0,
    '1': dp.MCP4XXXWiperID.WIPER_1,
    '2': dp.MCP4XXXWiperID.WIPER_2,
    '3': dp.MCP4XXXWiperID.WIPER_3,
}

CONFIG_SCHEMA = (
    output.FLOAT_OUTPUT_SCHEMA
    .extend(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(mcp4xxx_wiper),
            cv.Required(CONF_MCP4XXX_ID): cv.use_id(dp.mcp4xxx_digipot_base_component),            
            cv.Required(CONF_CHANNEL): cv.enum(CHANNEL_OPTIONS, upper=True)
        }
    )
    .extend(dp.OUTPUT_OPTIONAL_CONFIG_SCHEMA)
)

WIPER_ONLY_ACTION_SCHEMA = automation.maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
        }
    )

SET_FLOAT_LEVEL_ACTION_SCHEMA = cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
            cv.Required(CONF_LEVEL): cv.templatable(cv.float_range(min=0, max=1)),
        }
    )

SET_TERMINALS_ACTION_SCHEMA = cv.Schema(
        {
        cv.Required(CONF_ID): cv.use_id(mcp4xxx_wiper),
        cv.Optional(dp.CONF_TERMINAL_A, default=True): cv.templatable(cv.boolean),
        cv.Optional(dp.CONF_TERMINAL_B, default=True): cv.templatable(cv.boolean),
        cv.Optional(dp.CONF_TERMINAL_W, default=True): cv.templatable(cv.boolean),
        }
    )

@automation.register_action("output.increment_wiper", dp.IncreaseAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_increase_wiper(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("output.decrement_wiper", dp.DecreaseAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_decrease_wiper(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("output.set_wiper_level", dp.SetWiperValueAction, SET_FLOAT_LEVEL_ACTION_SCHEMA)
async def output_set_wiper_level(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_LEVEL], args, float)
    cg.add(var.set_level(template_))
    return var

@automation.register_action("output.set_nonvolatile_wiper_level", dp.SetNonvolatileWiperValueAction,
                            SET_FLOAT_LEVEL_ACTION_SCHEMA)
async def output_set_nonvolatile_wiper_level(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_LEVEL], args, float)
    cg.add(var.set_level(template_))
    return var

@automation.register_action("output.set_terminals", dp.SetTerminalsAction, SET_TERMINALS_ACTION_SCHEMA)
async def output_set_terminals(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    term_a = await cg.templatable(config[dp.CONF_TERMINAL_A], args, bool)
    term_b = await cg.templatable(config[dp.CONF_TERMINAL_B], args, bool)
    term_w = await cg.templatable(config[dp.CONF_TERMINAL_W], args, bool)
    cg.add(var.set_terminal_a(term_a))
    cg.add(var.set_terminal_b(term_b))
    cg.add(var.set_terminal_w(term_w))
    return var

@automation.register_action("output.enter_shutdown", dp.EnterShutdownAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_enter_shutdown(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("output.exit_shutdown", dp.ExitShutdownAction, WIPER_ONLY_ACTION_SCHEMA)
async def output_exit_shutdown(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_MCP4XXX_ID])
    var = cg.new_Pvariable(
        config[CONF_ID],
        parent,
        config[CONF_CHANNEL],
    )

    await output.register_output(var, config)
    await cg.register_parented(var, config[CONF_MCP4XXX_ID])
    await dp.set_initial_conditions(var, config)