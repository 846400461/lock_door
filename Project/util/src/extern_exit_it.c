#include "stm8l15x.h"
#include "extern_exit_it.h"

void fingerExternExitConfig(void)
{
    disableInterrupts();
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_0,EXTI_Trigger_Rising);
    enableInterrupts();//ENABLE
}