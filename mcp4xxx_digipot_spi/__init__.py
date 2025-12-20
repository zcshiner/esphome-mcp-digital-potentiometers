import esphome.codegen as cg
from esphome.components import spi
from .. import mcp4xxx_digipot_base

CODEOWNERS = ["@zcshiner"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["mcp4xxx_digipot_base"]

mcp4xxx_digipot_spi_ns = cg.esphome_ns.namespace("mcp4xxx_digipot_spi")
mcp4xxx_digipot_spi_component = mcp4xxx_digipot_spi_ns.class_("mcp4xxx_digipot_spi_component",
										mcp4xxx_digipot_base.mcp4xxx_digipot_base_component, spi.SPIDevice)
