#include "stateManager.h"
#include "stm8l15x.h"
#include "stm8l15x_usart.h"
#include "usart.h"
#include "stm8l15x_it.h"
#include "fingerVeinProtocol.h"
#include "tim4.h"
#include "button.h"
#include "extern_exit_it.h"
#include <stdio.h>

#define FREE_NUM 3

enum FINGER_VEIN_STATE{
  OFF=0,
  LOW_POWER_CHECK,
  REGISTRATION,
  IDENTIFICATION
}machineState=OFF;
xQueue fingerQueue;


void initFingerVein(void){
  tim4config();
  fingerExternExitConfig();
   //button();
  USART1_Config();
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
  case LOW_POWER_CHECK:
    lowPowerState();
    break;
  case IDENTIFICATION:
    freeIdentification();
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
        GPIO_SetBits(GPIOG,GPIO_Pin_4);
        machineState=LOW_POWER_CHECK;
    }
    return;
  }
  count100ms=0;
  sending=1;
  initFingerVeinPacket(&xgPacket,XG_CMD_CONNECTION,0x08,"00000000");
  sendData(USART3,(uint8_t*)&xgPacket,24);
  
}
void lowPowerState(void){
  if(fingerReached) {
    machineState=IDENTIFICATION;
    fingerReached=0;
  }
}

void freeIdentification(void){
  static uint8_t sending=0;
  struct XgPacket xgPacket;
  uint8_t state;
  
  if(sending==1&&queueLength(fingerQueue)==0)return;
  if(sending==1){
    dequeue_t(fingerQueue,(void*)&xgPacket);
    state=xgPacket.bData[0];
    if(state>1)return;
    xgPacket.bData[0]==0?identifySuccess((uint8_t*)&xgPacket):identifyFailed((uint8_t*)&xgPacket);
    machineState=LOW_POWER_CHECK;
    sending=0;
    return;
  }
  
  sending=1;
  initFingerVeinPacket(&xgPacket,XG_CMD_IDENTIFY_FREE,0x00,"0");
  sendData(USART3,(uint8_t*)&xgPacket,24);
}

void identifySuccess(uint8_t *data){
  
  GPIO_ToggleBits(GPIOG,GPIO_Pin_6);
  printf("\nidentifySuccess\n");
  for(int i=0;i<24;i++)
  printf("%02X ",data[i]);
}
  
void identifyFailed(uint8_t *data){
  GPIO_ToggleBits(GPIOG,GPIO_Pin_7);
  printf("\nidentifyFailed\n");
  for(int i=0;i<24;i++)
  printf("%02X ",data[i]);
}