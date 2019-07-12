#include "stm8l15x.h"
#include "extern_exit_it.h"

uint8_t fingerReached=0;

void fingerExternExitConfig(void)
{
    disableInterrupts();
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_0,EXTI_Trigger_Rising);
    enableInterrupts();//ENABLE
}