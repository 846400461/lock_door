#include "stm8l15x.h"
#include "tim4.h"



void tim4config(void){
  //GPIO_Init(GPIOG,GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); 
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_TimeBaseInit(TIM4_Prescaler_64,250);//1ms
  TIM4_SetCounter(250);//计数器初值为250，产生更新事件
  TIM4_ITConfig(TIM4_IT_Update,ENABLE );
  enableInterrupts();
  TIM4_Cmd(ENABLE);
}
