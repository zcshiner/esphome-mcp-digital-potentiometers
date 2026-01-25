import esphome.codegen as cg
from esphome.components import i2c
from .. import mcp4xxx_digipot_base

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["mcp4xxx_digipot_base"]

mcp4xxx_digipot_i2c_ns = cg.esphome_ns.namespace("mcp4xxx_digipot_i2c")
mcp4xxx_digipot_i2c_component = mcp4xxx_digipot_i2c_ns.class_("mcp4xxx_digipot_i2c_component",
										mcp4xxx_digipot_base.mcp4xxx_digipot_base_component, i2c.I2CDevice)
