#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "stm8l15x.h"

#ifdef DEBUG_FINGERVEIN

#define fingerLog(format, ...) printf(format, ##__VA_ARGS__)

#else

#define fingerLog(format, ...)

#endif
typedef void (*ActionHandler)(void);
typedef void (*IdentifyHandler)(uint8_t*,uint8_t);
typedef void (*EnrollHandler)(uint8_t*,uint8_t);

struct FingerVeinConfig{
  USART_TypeDef* proUsart;
  ActionHandler putInto;
  ActionHandler takeAway;
  ActionHandler connectSuccess;
  ActionHandler idDeleteSuccess;
  ActionHandler ResetSuccess;
  IdentifyHandler identifyFailHandler;
  IdentifyHandler identifySuccessHandler;
  EnrollHandler enrollFailHandler;
  EnrollHandler enrollSuccessHandler;
  EnrollHandler obtainIdHandler;  
};


void initFingerVein(struct FingerVeinConfig config);

void stateMachine(void);

void deInitFingerVein(void);

void enrollFingerVein(void);

void fingerVeinTime(void);

void fingerReach(void);

void deleteUserid(uint8_t *data);

void deleteAllUser(void);

void resetevent(void);

//void setlevel(uint8_t data);


#endif