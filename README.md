# General
Firmware for Nordic Semuconductor NRF51822 module with AHT-20 temperature and humidity sensor, working by BTHome protocol.
Additionaly, it has battery voltage measurement by internal ADC.

Using SYSTEM_ON mode for save power and WDT for wake-up.

# Hardware
  * AHT-20
  * NRF51822 module (Waveshare 400)
  * Indication (optional):
    * LED
    * 10K resistor
  * 3v battery case (for example cr2430)
## Wiring
  * AHT-20 [**GND**] --> NRF51822 [**GND**]
  * AHT-20 [**VCC**] --> NRF51822 [**P0.22**]
  * AHT-20 [**SDA**] --> NRF51822 [**P0.0**]
  * AHT-20 [**SCL**] --> NRF51822 [**P0.1]**
  * LED [+] --> NRF51822 [**P0.22**]
  * LED [-] --> NRF51822 [**GND**] via 10k resistor

# Software
1. Flash SoftDevice S110
2. Compile and upload firmware to NRF module
3. PROFIT
