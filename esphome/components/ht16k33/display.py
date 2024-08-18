import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, i2c
from esphome.const import CONF_ID, CONF_LAMBDA

DEPENDENCIES = ["i2c"]

ht16k33_ns = cg.esphome_ns.namespace("ht16k33")
HT16K33Component = ht16k33_ns.class_(
    "HT16K33Component", cg.Component, i2c.I2CDevice
)

HT16K33ComponentRef = HT16K33Component.operator("ref")

CONFIG_SCHEMA = (
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(HT16K33Component),
            cv.Required('default_brightness'): cv.int_range(0, 15),
            # default intensity
        }
    )
    .extend(cv.polling_component_schema('1s'))
    .extend(i2c.i2c_device_schema(0x70))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_brightness(config['default_brightness']))
    await i2c.register_i2c_device(var, config)
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(HT16K33ComponentRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
