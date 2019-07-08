#include "stateManager.h"
#include "stm8l15x.h"
#include "stm8l15x_usart.h"
#include "usart.h"

enum FINGER_VEIN_STATE{
  OFF=0,
  ON,
  REGISTRATION,
  LOW_POWER_CHECK,
  IDENTIFICATION
}machineState=OFF;

uint32_t ms100Count=0;
struct XgPacket xgPacket;

void stateMachine(void){
  
  switch(machineState){
  case OFF:
    
    break;
  }
}

void connectFingerVein(void){
  static uint8_t sent=0;
  
  if(sent==1&&ms100Count>=50){
    sent=0;
    return;
  }
  if(sent==1&&ms100Count<50){
    return;
  }
  
  sent=1;
  
}