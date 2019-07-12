#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "stm8l15x.h"

void stateMachine(void);

void connectFingerVein(void);

void initFingerVein(void);

void deInitFingerVein(void);

void lowPowerState(void);

void freeIdentification(void);

void identifySuccess(uint8_t *data);

void identifyFailed(uint8_t *data);
#endif