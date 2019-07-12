#include "stm8l15x.h"
#include "button.h"

void button(void)
{
    //设置GPIOE，GPIO_Pin_6作为中断源。Input pull-up, external interrupt
    GPIO_Init(GPIOE,GPIO_Pin_6,GPIO_Mode_In_PU_IT);
    //Sets the external interrupt sensitivity of the selected pin.
    //EXTI_Trigger_Falling:Interrupt on Falling edge only 
    EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);
    GPIO_Init(GPIOE,GPIO_Pin_7,GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
    enableInterrupts();//ENABLE
}