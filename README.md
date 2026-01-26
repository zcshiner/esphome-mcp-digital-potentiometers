# MCP Digital Potentiometers

This is an external component for [ESPHome](https://esphome.io/) to control a variety of 4000 series Microchip Digital Potentiometers.  It consists of base components to define core functionality and chip-specific components for each product family.

This component is a rewrite of ESPHome's [MCP4461 Component](https://esphome.io/components/output/mcp4461/) and shares much of it's heritage.

## Usage

Check out the `examples` directory for example configurations.

You can use this component in your ESPHome configuration by importing it with `external_components`:

```yaml
external_components:
  - source: github://zcshiner/esphome-mcp-digital-potentiometers@main

```

To implement a potentiometer, first bring in the base chip that matches your design and optionally specifiy its address.  Then create an output with a platform set to the same type.  You will also need to implement an I2C or SPI component as as appropriate.

```yaml
i2c:
  sda: GPIO22
  scl: GPIO23

mcp456x:
  - id: mcp456x_output
    address: 0x2E

output:
  - platform: mcp456x
    id: digipot_channel_0
    mcp456x_id: mcp456x_output
    channel: 0
```

Note that `mcp456x_id` matches both the base component name and its `id`.

An example with a multi-channel potentiometer:

```yaml
mcp446x:
  - id: mcp446x_output

output:
  - platform: mcp446x
    id: channel_0
    mcp446x_id: mcp446x_output
    channel: 0

  - platform: mcp446x
    id: channel_1
    mcp446x_id: mcp446x_output
    channel: 1
```

You may optionally assign initial conditions that will be commanded to the chip as part of initialization:

```yaml
output:
  - platform: mcp414x
    id: channel_0
    mcp414x_id: mcp414x_output
    channel: 0
    initial_value: 0.1
    terminal_a: True
    terminal_w: True
    terminal_b: False
```

### Base Component - I²C
- **id** (**Required**, ID): The id to use for this component.
- **address** (Optional, int): The I²C address of the driver. Defaults to the address corresponding to all hardware address-pins tied low.

### Base Component - SPI

- **id** (**Required**, ID): The id to use for this component.
- **cs_pin** (**Required**, int): The SPI chip select pin to use.

### Output Component

- **id** (Optional, ID): The id to use for this component.
- **mcpWXYZ_id** (**Required**, ID): The id of the base.component.  Must match component name.  See examples.
- **channel** (**Required**, int): Output channel to use.  Will throw a validation error if channel > max channels of the chip.
- **terminal_a** (Optional, boolean): Enable or disable terminal A at initialization
- **terminal_w** (Optional, boolean): Enable or disable terminal W at initialization
- **terminal_b** (Optional, boolean): Enable or disable terminal B at initialization
- **initial_value** (Optional, int): Initial value to write into RAM at initialization

## Actions

### `output.increment_wiper` Action

This action commands the digital potentiometer to increase by a single tap count.  Going above the maximum tap count will generate a warning.

```yaml
on_...:
  then:
    - output.increment_wiper: channel_0
```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda) and will return the new tap count:

```C++
id(channel_0).increase_wiper();
```

### `output.decrement_wiper` Action

This action commands the digital potentiometer to decrease by a single tap count.  Going below the minimum tap count will generate a warning.

```yaml
on_...:
  then:
    - output.decrement_wiper: channel_0
```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda) and will return the new tap count:

```C++
id(channel_0).decrease_wiper();
```

### `output.set_wiper_level` Action

This action sets the output to the given float value (0-1).
```yaml
on_...:
  then:
    - output.set_level: 
        id: channel_0
        level: 0.5

```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda):

```C++
// integer wiper tap number
id(channel_0).set_wiper_level(64);
```

### `output.set_nonvolatile_wiper_level` Action

For supported devices, this action writes the value to the NonVolatile wiper register.  Read the datasheet for operational specifics.

```yaml
on_...:
  then:
    - output.set_nonvolatile_wiper_level: 
        id: channel_0
        level: 0.5

```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda):

```C++
// integer wiper tap number
id(channel_0).set_nonvolatile_wiper_level(64);
```

### `output.set_terminals` Action

This action writes a new terminal state to the TCON register.  Use this to connect/disconnect terminals A, W, B of a specific output.

```yaml
on_...:
  then:
    - output.set_terminals:
        id: channel_0
        terminal_a: True
        terminal_w: True
        terminal_b: False
```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda):

```C++
id(channel_0).set_terminals(true, true, false);
```

### `output.enter_shutdown` Action

This action commands the output into shutdown mode. Refer to the datasheet for specifics.

```yaml
on_...:
  then:
    - output.enter_shutdown: channel_0
```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda):

```C++
id(channel_0).enter_shutdown();
```

### `output.exit_shutdown` Action

This action commands the output into shutdown mode. Refer to the datasheet for specifics.

```yaml
on_...:
  then:
    - output.exit_shutdown: channel_0
```

This action can also be expressed in [lambdas](https://esphome.io/automations/templates/#config-lambda):

```C++
id(channel_0).exit_shutdown();
```

## lambda calls

From [lambdas](https://esphome.io/automations/templates/#config-lambda), you can call several methods to do some advanced stuff

- `get_tap_count()`: Return the maximum tap count of the device

```C++
// call from the output channel
id(channel_0).get_tap_count();

// or call from the platform/device -- returns all the same
id(mcp456x_output).get_tap_count();
```

Combine with set_wiper_level to recreate float behavior.  Many ESPHome lambdas use `x` as the output of some sensor/state.

```C++
// integer wiper tap number
id(channel_0).set_wiper_level(x * id(channel_0).get_tap_count());
```

- `increase_wiper_fast()`: Send the increment wiper command without checking current or new wiper level. `increase_wiper()` uses this internally after requesting the current level.

```C++
id(channel_0).increase_wiper_fast();
```

- `decrease_wiper_fast()`: Send the decrement wiper command without checking current or new wiper level. `decrease_wiper()` uses this internally after requesting the current level.

```C++
id(channel_0).decrease_wiper_fast();
```

## Supported Models

| Model | Channels | NV Memory | Steps | Type |
| --- | --- | --- | --- | ---|
| [MCP414X](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22059b.pdf) | 1 | Yes | 128 | SPI |
| [MCP446X](https://ww1.microchip.com/downloads/aemDocuments/documents/MSLD/ProductDocuments/DataSheets/MCP444X-446X-Data-Sheet-DS20002265.pdf) | 4 | Yes | 256 | I²C |
| [MCP453X](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22096b.pdf) | 1 | No | 128 | I²C |
| [MCP456X](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22107B.pdf) | 1 | Yes | 256 | I²C |

Microchip differentiates their digital potentiometer families by a few different parametrics as outlined above.  This external component was written to make the addition of other combinations of Channel, NonVolatile Memory, Step, and Communication Type straightforward.

So far only potentiometer chips (XXX1 suffix) have been tested, but this component should theoretically work with rheostats (XXX2 suffix) as well.

## License

Released under the MIT license, see `LICENSE`

### Postscript
*I spent quite a quile writing, testing, and documenting this project.  I will eventually submit a PR to integrate into ESPHome, but that's more work than I'm ready to take on today.*