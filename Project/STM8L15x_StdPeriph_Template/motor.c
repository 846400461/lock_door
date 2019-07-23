#include "motor.h"
#include "stm8l15x.h"
#include <stdio.h>

uint8_t motorCount=0;
uint8_t unLockSignal=0;
enum MOTOR_STATE{
  OFF,
  WAIT_SIGNAL,
  MOTOR_MOVING
}motorState=OFF;

void turnDirection(uint8_t direct);
void departLock(void);
void waitSignal(void);
void motorMoving(void);


void initMotor(void){
  GPIO_Init(GPIOA,GPIO_Pin_2 | GPIO_Pin_3,GPIO_Mode_Out_PP_Low_Fast);
}

void motorProcess(void){
  switch(motorState){
  case OFF:
    departLock();
    break;
  case WAIT_SIGNAL:
    waitSignal();
    break;
  case MOTOR_MOVING:
    motorMoving();
    break;
  }
}

void motorTime(void){
  motorCount++;
}

static void turnDirection(uint8_t direct){
  if(direct==0){
    GPIO_SetBits(GPIOA,GPIO_Pin_2);
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);
  }else if(direct==1){
    GPIO_SetBits(GPIOA,GPIO_Pin_3);
    GPIO_ResetBits(GPIOA,GPIO_Pin_2);
  }else{
    GPIO_ResetBits(GPIOA,GPIO_Pin_2);
    GPIO_ResetBits(GPIOA,GPIO_Pin_3);
  }
}

static void departLock(void){
  static uint8_t time500ms=0;
  if(!time500ms){
    motorCount = 0;
    time500ms = 1;
    turnDirection(0);
  }
  if(time500ms&&motorCount>=5){
    time500ms = 0;
    turnDirection(2);
    motorState = WAIT_SIGNAL;
  }
}

static void waitSignal(void){
  if(unLockSignal){
    unLockSignal = 0;
    motorState = MOTOR_MOVING;
  }
}

void unlockEvent(void){
  unLockSignal=1;
}

static void motorMoving(void){
  static uint8_t state=0,moving=0;
  switch(state){
  case 0:
    if(!moving){
      turnDirection(1);
      moving=1;
      motorCount=0;
      printf("state : %d\n",state);
    }
    if(moving&&motorCount>=5){
      turnDirection(2);
      moving=0;
      state=1;
    }
    break;
  case 1:
    if(!moving){
      turnDirection(2);
      moving=1;
      motorCount=0;
      printf("state : %d\n",state);
    }
    if(moving&&motorCount>=50){
      moving=0;
      state=2;
    }
    break;
  case 2:
    if(!moving){
      turnDirection(0);
      moving=1;
      motorCount=0;
      printf("state : %d\n",state);
    }
    if(moving&&motorCount>=5){
      moving=0;
      state=0;
      turnDirection(2);
      motorState=WAIT_SIGNAL;
      printf("turn over\n");
    }
    break;
  }
}