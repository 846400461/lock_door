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

static uint8_t enrollEvent=0,count100ms=0,deleteEvent=0,deleteAllEvent=0,resetEvent=0;
static uint8_t fingerReached=0;
static uint8_t emptyId[4]={0};
static uint8_t userId[4]={0};
static uint8_t sending=0;
//static uint8_t level=0;;
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
void obtainIded(uint8_t *data,uint8_t length);
void putIntoFinger(void);
void takeAwayFinger(void);
void timeoutHandler(void);
void MachineTimeOut(void);
void deleteUser(void);
void deleteUserAll(void);
void deleteSuccess(void);
void reset(void);
void resetSuccess(void);
//void setSecurityLevel(void);
enum FINGER_VEIN_STATE{
  OFF=0,
  LOW_POWER_CHECK,
  OBTAIN_ID,
  REGISTRATION,
  IDENTIFICATION,
  TIMEOUT,
  DELETE,
  DELETEALL,
  RESET_ALL
}machineState=OFF;
xQueue fingerQueue;


void initFingerVein(struct FingerVeinConfig config){
   initFingerProtocol();
   fingerVeinConfig=config;
}

void deInitFingerVein(void){
  deInitFingerProtocol();
}

void stateMachine(void){
  
  
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
  case DELETE:
    deleteUser();
    break;
  case DELETEALL:
    deleteUserAll();
    break;
  case RESET_ALL:
    reset();
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
  }else if(deleteEvent){
    machineState=DELETE;
    deleteEvent=0;
  }else if(deleteAllEvent){
    machineState=DELETEALL;
    deleteAllEvent=0;    
  }else if(resetEvent){
    machineState=RESET_ALL;
    resetEvent=0;    
  }
//    else if(setlevelEvent){
//    machineState=SETLEVEL;
//    setlevelEvent=0;
// }
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
      for(int i=0;i<4;i++)
        emptyId[i]=xgPacket.bData[i+1];
      machineState = REGISTRATION;
      obtainIded(emptyId,4);
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
    initFingerVeinPacket(&xgPacket,XG_CMD_ENROLL,0x04,emptyId); 
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

static void obtainIded(uint8_t *data,uint8_t length){
  
  
  
  fingerLog("\n obtainIded\n");
  if(fingerVeinConfig.obtainIdHandler)
    fingerVeinConfig.obtainIdHandler(data,length);
}
void deleteUserid(uint8_t *data){
  for(int i =0;i<4;i++ )
  {
      userId[i]=data[i]; 
      printf("");
  }
  deleteEvent=1;
}

void deleteAllUser(void){
  deleteAllEvent=1;
}

static void deleteUser(void){  
  
  
    if(sending==0){
    initFingerVeinPacket(&xgPacket,XG_CMD_CLEAR_ENROLL,0x04,userId); 
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);
  
    sending=1;
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  machineState=LOW_POWER_CHECK;
  if(xgPacket.bData[0]==0){
      deleteSuccess();
  }  
  sending=0;
}
static void deleteUserAll(void){
  
  
  
    if(sending==0){
    initFingerVeinPacket(&xgPacket,XG_CMD_CLEAR_ALL_ENROLL,0x00,"0"); 
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);  
    sending=1;
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  machineState=LOW_POWER_CHECK;
    if(xgPacket.bData[0]==0){
      deleteSuccess();
  }  
  sending=0;
}
static void deleteSuccess(void){    
  
    fingerLog("\n deleteSuccess\n");
    if(fingerVeinConfig.idDeleteSuccess)
     fingerVeinConfig.idDeleteSuccess();
  
}
static void reset(void){  
    if(sending==0){
    initFingerVeinPacket(&xgPacket,XG_CMD_FACTORY_SETTING,0x00,"0"); 
    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);  
    sending=1;
  }
  if(queueLength(fingerQueue)==0) return;
  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
  machineState=LOW_POWER_CHECK;
    if(xgPacket.bData[0]==0){
      resetSuccess();
  }  
  sending=0;
}
static void resetSuccess(void){    
    fingerLog("\n resetSuccess\n");
    if(fingerVeinConfig.ResetSuccess)
     fingerVeinConfig.ResetSuccess();  
}
void resetevent(void){
 resetEvent=1;
}

//void setlevel(uint8_t data){
//  level=data;
//  setlevelEvent=1;
//  
//}
//
//static void setSecurityLevel(void){  
//    if(sending==0){      
//    initFingerVeinPacket(&xgPacket,XG_CMD_SET_SECURITYLEVEL,0x01,&level); 
//    sendData(fingerVeinConfig.proUsart,(uint8_t*)&xgPacket,24);  
//    sending=1;
//  }
//  if(queueLength(fingerQueue)==0) return;
//  dequeue_tOnFinger(fingerQueue,(void*)&xgPacket);
//  machineState=LOW_POWER_CHECK;
//  sending=0;
//    
//
//}
//
//
//







