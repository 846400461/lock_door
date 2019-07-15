#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "stm8l15x.h"

#ifdef DEBUG_FINGERVEIN

#define fingerLog(format, ...) printf(format, ##__VA_ARGS__)

#else

#define fingerLog(format, ...)

#endif

typedef void (*actionHandler)();
typedef void (*identifyHandler)(uint8_t*,uint8_t);


void stateMachine(void);

void connectFingerVein(void);

void initFingerVein(void);

void deInitFingerVein(void);

void lowPowerState(void);

void freeIdentification(void);

void identifySuccess(uint8_t *data);

void identifyFailed(uint8_t *data);

void enrollFingerVein(void);

void obtainEmptyUserId(void);

void userEnroll(void);

void enrollFingerVein(void);

void userEnrollFailed(void);

void userEnrollSuccess(void);

void putIntoFinger(void);

void takeAwayFinger(void);

void timeoutHandler(void);

void MachineTimeOut(void);
#endif