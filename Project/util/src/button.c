#include "stm8l15x.h"
#include "button.h"

#define LED_GPIO_PORT  GPIOC
#define LED_GPIO_PINS  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
void buttonInit(){
    disableInterrupts();
    GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_4,EXTI_Trigger_Falling);
    enableInterrupts();//ENABLE
}