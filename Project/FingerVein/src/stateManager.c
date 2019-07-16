#include "stateManager.h"
#include "stm8l15x.h"
#include "stm8l15x_usart.h"
#include "usart.h"
#include "stm8l15x_it.h"
#include "fingerVeinProtocol.h"
#include "tim4.h"
#include "button.h"
#include "extern_exit_it.h"
#include "myqueue.h"
#include <stdio.h>
#include <string.h>

static uint8_t enrollEvent=0,count100ms=0;
static uint8_t fingerReached=0;
static uint8_t emptyId[4]={0};
static uint8_t sending=0;
static int8_t ReCount=0;
static struct XgPacket xgPacket;
static struct FingerVeinConfig fingerVeinConfig;

void dequeue_tOnFinger(xQueue qu,void *data);
void connected(void);
void connectFingerVein(void);

void lowPowerState(void);
void freeIdentification(void);
void identifySuccess(uint8_t *data,uint8_t length);
void identifyFailed(uint8_t *data,uint8_t length);
void enrollFingerVein(void);
void obtainEmptyUserId(void);
void userEnroll(void);
void userEnrollFailed(uint8_t *data,uint8_t length);
void userEnrollSuccess(uint8_t *data,uint8_t length);
void putIntoFinger(void);
void takeAwayFinger(void);
void timeoutHandler(void);
void MachineTimeOut(void);

enum FINGER_VEIN_STATE{
  OFF=0,
  LOW_POWER_CHECK,
  OBTAIN_ID,
  REGISTRATION,
  IDENTIFICATION,
  TIMEOUT
}machineState=OFF;
xQueue fingerQueue;


void initFingerVein(struct FingerVeinConfig config){
  //tim4config();
  //fingerExternExitConfig();
  //buttonInit();
  //USART1_Config();
  //USART3_Config();
  initFingerProtocol();
  //memset(&fingerVeinConfig,0,sizeof(fingerVeinConfig));
  fingerVeinConfig=config;
}

void deInitFingerVein(void){
  deInitFingerProtocol();
}

void stateMachine(void){
  
  //printf("machineState : %d \n\n",machineState);
  if(machineState!=OFF)
  {
    MachineTimeOut();
  }
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
  case OBTAIN_ID:
    obtainEmptyUserId();
    break;
  case REGISTRATION:
    userEnroll();
    break;
  case TIMEOUT:
    timeoutHandler();
    break;
  }
}

static void connectFingerVein(void){
  if(sending==1&&count100ms>=50){
    sending=0;
    return;
  }
  if(sending==1&&count100ms<50){
    if(queueLength(fingerQueue)>0&&dequeue_t(fingerQueue,(void*)&xgPacket)&&
       !getStateForFiVePacket(&xgPacket)){
        GPIO_SetBits(GPIOG,GPIO_Pin_4);
        machineState=LOW_POWER_CHECK;
        connected();
        sending=0;
    }
    return;
  }
  count100ms=0;
  sending=1;
  initFingerVeinPacket(&xgPacket,XG_CMD_CONNECTION,0x08,"00000000");
  sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
  
}
static void lowPowerState(void){
  if(fingerReached) {
    machineState=IDENTIFICATION;
    fingerReached=0;
  }else if(enrollEvent){
    machineState=OBTAIN_ID;
    enrollEvent=0;
  }
}

static void freeIdentification(void){
  if(sending==0){
    sending=1;
    initFingerVeinPacket(&xgPacket,XG_CMD_IDENTIFY_FREE,0x00,"0");
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
    fingerLog("\nsend freeIdentification data\n");
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  fingerLog("\nbData : %02X\n",xgPacket.bData[0]);
  switch(xgPacket.bData[0]){
  case 0x20:
    putIntoFinger();
    break;
  case 0x21:
    takeAwayFinger();
    break;
  case 0x00:
    machineState=LOW_POWER_CHECK;
    identifySuccess(xgPacket.bData,2);
    sending=0;
    break;
  case 0x01:
    machineState=LOW_POWER_CHECK;
    identifyFailed(xgPacket.bData,2);
    sending=0;
    break;
  }
  
}

static void obtainEmptyUserId(void){
  if(sending==0){
    initFingerVeinPacket(&xgPacket,XG_CMD_GET_EMPTY_ID,0x00,"0");
    sending=1;
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
    fingerLog("\nsend obtainEmptyUserId cmd\n");
  }
  if(queueLength(fingerQueue)){
    dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
    if(xgPacket.bCmd!=XG_CMD_GET_EMPTY_ID) return;
    if(xgPacket.bData[0]==XG_ERR_SUCCESS){
      fingerLog("id %02x\n",xgPacket.bData[1]);
      //emptyId=xgPacket.bData[1];
      for(int i=0;i<4;i++)
        emptyId[i]=xgPacket.bData[i+1];
      //printf("\nemptyUserId: %02X \n",emptyId);
      machineState = REGISTRATION;
    }else{
      machineState = LOW_POWER_CHECK;
      fingerLog("\nno emptyUserId\n");
      userEnrollFailed(xgPacket.bData,2);
    }
    sending=0;
  }
  
}
  
static void userEnroll(void){
  if(sending==0){
//    for(int i=0;i<4;i++)
//      xgPacket.bData[i]=(emptyId>>(i*8))&0xff;
    //memset(xgPacket.bData,0,16);
    //xgPacket.bData[0]=emptyId;
    initFingerVeinPacket(&xgPacket,XG_CMD_ENROLL,0x04,emptyId); 
    //sendData(USART1,(uint8_t*)&xgPacket,24);
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
  
    sending=1;
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  switch(xgPacket.bData[0]){
  case 0x00:
    userEnrollSuccess(xgPacket.bData,2);
    machineState=LOW_POWER_CHECK;
    sending=0;
    break;
  case 0x01:
    userEnrollFailed(xgPacket.bData,2);
    machineState=LOW_POWER_CHECK;
    sending=0;
    break;
  case 0x21:
    takeAwayFinger();
    break;
  case 0x20:
    putIntoFinger();
    break;
  }
  fingerReached=0;
}

static void timeoutHandler(void){
  if(sending==0){
    initFingerVeinPacket(&xgPacket,XG_CMD_CANCEL,0x00,"0");
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
    sending=1;
    fingerLog("\nXG_CMD_CANCEL\n");
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  machineState=LOW_POWER_CHECK;
  sending=0;
}

static void identifySuccess(uint8_t *data,uint8_t length){
  
  GPIO_ToggleBits(GPIOG,GPIO_Pin_6);
  fingerLog("\nidentifySuccess\n");
  if(fingerVeinConfig.identifySuccessHandler)
    fingerVeinConfig.identifySuccessHandler(data,length);
}
  
static void identifyFailed(uint8_t *data,uint8_t length){
  GPIO_ToggleBits(GPIOG,GPIO_Pin_7);
  fingerLog("\nidentifyFailed\n");
  if(fingerVeinConfig.identifyFailHandler)
    fingerVeinConfig.identifyFailHandler(data,length);
}

void enrollFingerVein(void){
  enrollEvent=1;
}

void fingerReach(void){
  fingerReached=1;
}
                         
static void userEnrollFailed(uint8_t *data,uint8_t length){
  fingerLog("\nuserEnrollFailed\n");
  if(fingerVeinConfig.enrollFailHandler)
    fingerVeinConfig.enrollFailHandler(data,length);
}

static void userEnrollSuccess(uint8_t *data,uint8_t length){
  fingerLog("\nuserEnrollSuccess\n");
  if(fingerVeinConfig.enrollSuccessHandler)
    fingerVeinConfig.enrollSuccessHandler(data,length);
}

static void putIntoFinger(void){
   fingerLog("\nput into finger\n");
   if(fingerVeinConfig.putInto)
     fingerVeinConfig.putInto();
}

static void takeAwayFinger(void){
  fingerLog("\ntake away finger\n");
  if(fingerVeinConfig.takeAway)
    fingerVeinConfig.takeAway();
}

void MachineTimeOut(void){
  static uint8_t timeStart=0;
  if(sending==0){
    timeStart=0;
    return;
  }
  if(ReCount) {
    timeStart=0;
    (ReCount-1)<=0 ? ReCount=0 : ReCount--;
    return;
  }
  if(!timeStart) {
    timeStart=1;
    count100ms=0;
    fingerLog("\ntimeStart\n");
  }
  
  if(count100ms>=60){
    machineState=TIMEOUT;
    timeStart=0;
    sending=0;
  }
  
}

static void dequeue_tOnFinger(xQueue qu,void *data){
  dequeue_t(fingerQueue,data);
  ReCount++;
}

void fingerVeinTime(void){
  count100ms++;
}

static void connected(void){
  fingerLog("\nconnectSuccess\n");
  if(fingerVeinConfig.connectSuccess)
    fingerVeinConfig.connectSuccess();
}