#include "stateManager.h"
#include "stm8l15x.h"
#include "stm8l15x_usart.h"
#include "usart.h"
#include "stm8l15x_it.h"
#include "fingerVeinProtocol.h"
#include "tim4.h"

enum FINGER_VEIN_STATE{
  OFF=0,
  ON,
  REGISTRATION,
  LOW_POWER_CHECK,
  IDENTIFICATION
}machineState=OFF;
xQueue fingerQueue;


void initFingerVein(void){
  tim4config();
  USART3_Config();
  initFingerProtocol();
}

void deInitFingerVein(void){
  deInitFingerProtocol();
}

void stateMachine(void){
  
  switch(machineState){
  case OFF:
    connectFingerVein();
    break;
  case ON:
    break;
  }
}

void connectFingerVein(void){
  static uint8_t sending=0;
  struct XgPacket xgPacket;
  
  if(sending==1&&count100ms>=50){
    sending=0;
    return;
  }
  
  if(sending==1&&count100ms<50){
    if(queueLength(fingerQueue)>0&&dequeue_t(fingerQueue,(void*)&xgPacket)&&
       !getStateForFiVePacket(&xgPacket)){
      machineState=ON;
    }
    return;
  }
  
  count100ms=0;
  sending=1;
  initFingerVeinPacket(&xgPacket,XG_CMD_CONNECTION,0x08,"00000000");
  sendData(USART3,(uint8_t*)&xgPacket,24);
  
}