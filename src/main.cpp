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
    
    NVIC_EnableIRQ(RTC0_IRQn);
    //NRF_RTC0->POWER = 1;
    NRF_RTC0->PRESCALER = 3276;
    NRF_RTC0->CC[1] = 50;
    NRF_RTC0->EVTEN = 0x1<<17;    
    NRF_RTC0->INTENSET = 1<<17;    
    NRF_RTC0->TASKS_START = 1;    
    init();
}

int i = 0;
void loop() {  
  digitalWrite(22, HIGH);
  delay(100);
  digitalWrite(22, LOW);
  delay(100);
  __WFE();
  __SEV();
  __WFE();            
}

#ifdef __cplusplus
extern "C" {
#endif
void RTC0_IRQHandler(void){
  //NRF_RTC0->TASKS_STOP= 1;
  NRF_RTC0->TASKS_CLEAR= 1;
  //NRF_RTC0->TASKS_START= 1;  
  NRF_RTC0->EVENTS_COMPARE[1] = 0;
  
}
#ifdef __cplusplus
}
#endif