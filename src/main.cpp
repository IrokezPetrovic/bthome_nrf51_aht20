#include <Arduino.h>
#include <ble.h>
#include <Wire.h>
#include <AHT20.hpp>

#include <BLEPeripheral.h>

#define SLEEP_SEC 60*10
#define ONLINE_SEC 2
#define DEVICE_NAME "AHT-20"
#define VBAT_MAX_IN_MV 3000

struct __attribute__((packed)) AdvFlagsData {
  const uint8_t type = 0x1;
  uint8_t flags = 0x6;
};

struct __attribute__((packed)) AdvFlags {
  const uint8_t size = sizeof(AdvFlagsData);
  AdvFlagsData data;
};

struct __attribute__((packed)) AdvNameData {
  const uint8_t type = 0x8;
  const char name[sizeof(DEVICE_NAME)] = DEVICE_NAME;
};

struct __attribute__((packed))  AdvName {
  uint8_t size = sizeof(AdvNameData);
  AdvNameData data;
};

struct __attribute__((packed)) Battery {
  uint8_t type = 0x1;
  uint8_t level = 79;
};

struct __attribute__((packed)) Humidity {
  uint8_t type = 0x3;
  uint16_t level = 1234;
};

struct __attribute__((packed)) Temperature {
  uint8_t type = 0x2;
  int16_t level = -5678;
};

struct __attribute__((packed)) BTHomePacketData {
  uint16_t sign = 0xFCD2;
  uint8_t flags = 0x40;
  Battery battery;
  Humidity humidity;
  Temperature temperature;
};

struct __attribute__((packed)) BTHomePacket {
  const uint8_t size = sizeof(BTHomePacket)-1;
  const uint8_t type = 0x16;
  BTHomePacketData data;
};

struct __attribute__((packed)) Adv {
  AdvFlags flags;
  AdvName name;
  BTHomePacket bthome;
};

Adv advert;
AHT20 aht20;

BLEPeripheral blePeripheral = BLEPeripheral();

uint8_t measurePowerCurrecy(){
  NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos) |
                    (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) |
                    (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos) |
                    (ADC_CONFIG_PSEL_Disabled << ADC_CONFIG_PSEL_Pos) |
                    (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
  NRF_ADC->EVENTS_END = 0;
  NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;

  NRF_ADC->EVENTS_END = 0; // Stop any running conversions.
  NRF_ADC->TASKS_START = 1;

  while (!NRF_ADC->EVENTS_END)
  {
  }

  uint16_t vbg_in_mv = 1200;
  uint8_t adc_max = 255;
  uint16_t vbat_current_in_mv = (NRF_ADC->RESULT * 3 * vbg_in_mv) / adc_max;

  NRF_ADC->EVENTS_END = 0;
  NRF_ADC->TASKS_STOP = 1;
  NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;

  return (uint8_t)((vbat_current_in_mv * 100) / VBAT_MAX_IN_MV);
}

void measureAht20(){
  Wire.begin();
  Wire.setClock(TWI_FREQUENCY_FREQUENCY_K100);

  //aht20.softReset();
  delay(100);
  while (!aht20.begin())
  {
    delay(100);
  }
  while(!aht20.available()){
    delay(100);
  }
  float temperature = aht20.getTemperature()*100;
  float humidity = aht20.getHumidity()*100;
  

  advert.bthome.data.temperature.level = (int16_t)temperature;
  advert.bthome.data.humidity.level = (uint16_t) humidity;
}

void setup() {
    pinMode(22, OUTPUT);
    digitalWrite(22, HIGH);
    advert.bthome.data.battery.level = measurePowerCurrecy();
    measureAht20();
    
    blePeripheral.setConnectable(false);
    blePeripheral.setTxPower(4);
    blePeripheral.begin();
    blePeripheral.setAdvertisingInterval(20);
    sd_ble_gap_adv_data_set((const uint8_t*)&advert, sizeof(advert), NULL, 0);
}



void doSleep(){
  blePeripheral.end();
       // Configure nRF51 RAM retention parameters. Set for System On 16kB RAM retention
    NRF_POWER->RAMON = POWER_RAMON_ONRAM0_RAM0On   << POWER_RAMON_ONRAM0_Pos
                     | POWER_RAMON_ONRAM1_RAM1On   << POWER_RAMON_ONRAM1_Pos
                     | POWER_RAMON_OFFRAM0_RAM0Off << POWER_RAMON_OFFRAM0_Pos
                     | POWER_RAMON_OFFRAM1_RAM1Off << POWER_RAMON_OFFRAM1_Pos;
    NRF_POWER->RAMONB = POWER_RAMONB_ONRAM2_RAM2Off   << POWER_RAMONB_ONRAM2_Pos
                      | POWER_RAMONB_ONRAM3_RAM3Off   << POWER_RAMONB_ONRAM3_Pos	
                      | POWER_RAMONB_OFFRAM2_RAM2Off  << POWER_RAMONB_OFFRAM2_Pos
                      | POWER_RAMONB_OFFRAM3_RAM3Off  << POWER_RAMONB_OFFRAM3_Pos;

    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CRV = SLEEP_SEC*32768;             //ca 3 sek. timout
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
    NRF_WDT->TASKS_START = 1;           //Start the Watchdog timer

    __WFI();
    
}

void loop() {
  // digitalWrite(22, HIGH);
  // delay(100);
  // digitalWrite(22, LOW);
  // delay(100);

  if (millis()>(ONLINE_SEC*1000)){
    digitalWrite(22, LOW);
    doSleep();
  }
}

