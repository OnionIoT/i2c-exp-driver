# i2c-exp-driver
Project for drivers to program Omega Expansions that are I2C-based.

## ubus Integration
The drivers for the I2C-based Onion Expansions have been integrated with the ubus service via RPCD. They can be accessed through the i2c_exp service.

### pwm-exp ubus Commands
The Servo Expansion can be programmed using the `pwm-exp` command in the i2c_exp ubus service:
```bash
ubus call i2c_exp pwm-exp '{"command":"String", "params": {"key": "value" } }'
```

The following commands are available:
* init
* sleep
* set
* set-period

#### Init Command
The `init` command will reset the chip on the Servo Expansion and enable the oscillator.
```bash
ubus call i2c_exp pwm-exp '{"command":"init"}'
```

#### Sleep Command
The `sleep` command will put the oscillator into sleep mode, disabling all PWM signals
```bash
ubus call i2c_exp pwm-exp '{"command":"sleep"}'
```

#### Set Command
The `set` command is used to generate a PWM signal on a specific channel based on a **duty cycle percentage**:
```bash
ubus call i2c_exp pwm-exp '{"command":"set", "params":{"channel":"<CHANNEL>", "duty":"<DUTY CYCLE>"} }'
```

The CHANNEL can be:
* 0 - 15
  * control an individual channel
* all
  * control all channels at once

The DUTY CYCLE can be an integer or **floating point** number between 0 and 100

Optional parameters:
* `"frequency":"<signal frequency>"`
  * Sets the PWM signal frequency
  * If not specified, default of 50 Hz is used
  * Frequency range is 24 Hz to 1526 Hz
* `"delay":"<delay percentage>"`
  * Adds a delay in the PWM signal, can be an integer or **floating point** number between 0 and 100
  * 0% delay by default

#### Set-Period Command
The `set-period` command is used to generate a PWM signal on a specific channel based on a **PWM period and pulse width** (both can be integer ot floating point numbers):
```bash
ubus call i2c_exp pwm-exp '{"command":"set-period", "params":{"channel":9, "pulse":"1.5", "periods":20}}'
```


### relay-exp ubus Commands
The Relay Expansion can be programmed using the `relay-exp` command in the i2c_exp ubus service:

#### Set Command
The only command is the `set` command, it allows the relays to be programmed:
```bash
ubus call i2c_exp relay-exp '{"command":"set", "params":{"channel":"<CHANNEL>", "state":"RELAY STATE", "address":"<SWITCH STATES>"}}'
```

The CHANNEL can be one of:
* 0		- controls Relay0
* 1		- controls Relay1
* all 	- controls both relays

The RELAY STATE can be one of:
* off	- relay is switched off
* on	- relay is switched on

The DIP-SWITCH STATES should reflect the dip switch settings in binary starting with switch 1, then switch 2, then switch 3. The 0 position is when the switch is close the numbers.

Not required when all switches are 0.

Examples: 
* Switches 1 and 2 are 0 (close to the printed numbers), switch 3 is 1:
  * `"address":"001"`
* Switches 1 and 3 are 1, switch 2 is 0:
  * `"address":"101"`
* All switches are 1:
  * `"address":"111"`

#### Return Values
The ubus call will return different values based on if the command was successful or not.

Successful Command:
```bash
{"status":"success", "state":"<NEW RELAY STATE>"}
```

Unsuccessful Command:
```bash
{"status":"success", "error":"<INFO ON ERROR>"}
```
