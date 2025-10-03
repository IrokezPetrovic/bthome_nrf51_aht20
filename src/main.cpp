#include <Arduino.h>
#include <ble.h>


void onBtn(){

}

void setup() {
  pinMode(22, OUTPUT);
      // Configure nRF51 RAM retention parameters. Set for System On 16kB RAM retention
    NRF_POWER->RAMON = POWER_RAMON_ONRAM0_RAM0On   << POWER_RAMON_ONRAM0_Pos
                     | POWER_RAMON_ONRAM1_RAM1On   << POWER_RAMON_ONRAM1_Pos
                     | POWER_RAMON_OFFRAM0_RAM0Off << POWER_RAMON_OFFRAM0_Pos
                     | POWER_RAMON_OFFRAM1_RAM1Off << POWER_RAMON_OFFRAM1_Pos;
		NRF_POWER->RAMONB = POWER_RAMONB_ONRAM2_RAM2Off   << POWER_RAMONB_ONRAM2_Pos
                      | POWER_RAMONB_ONRAM3_RAM3Off   << POWER_RAMONB_ONRAM3_Pos	
	                    | POWER_RAMONB_OFFRAM2_RAM2Off  << POWER_RAMONB_OFFRAM2_Pos
	                    | POWER_RAMONB_OFFRAM3_RAM3Off  << POWER_RAMONB_OFFRAM3_Pos;
    pinMode(21, INPUT_PULLUP);
    attachInterrupt(21, &onBtn, FALLING);    
    
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
	  NRF_WDT->CRV = 2*32768;             //ca 3 sek. timout
	  NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
	  NRF_WDT->TASKS_START = 1;           //Start the Watchdog timer
}


void loop() {
  digitalWrite(22, HIGH);
  delayMicroseconds(100000);
  digitalWrite(22, LOW);
  delayMicroseconds(100000);

  // __WFE();
  // __SEV();
  // __WFE();            
  __WFI();
}

#ifdef __cplusplus
extern "C" {
#endif
void RTC0_IRQHandler(void){
  // NRF_RTC0->TASKS_CLEAR= 1;
  // NRF_RTC0->EVENTS_COMPARE[1] = 0;
  
}
#ifdef __cplusplus
}
#endif