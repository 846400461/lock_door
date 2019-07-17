#include <stdio.h>
#include "stm8l15x.h"
#include "fingerVeinProtocol.h"
#include "tim4.h"
#include "usart.h"
#include "stateManager.h"
#include "button.h"
#include "extern_exit_it.h"
#include <string.h>
#include "fingerVeinDemo.h"


void testDemo(void)
{
   tim4config();
   fingerExternExitConfig();
   buttonInit();
   USART1_Config();
   USART3_Config();
   
   struct FingerVeinConfig config;  
   memset(&config,0,sizeof(struct FingerVeinConfig));
   /*
    * 处理函数调用 
    */
   config.connectSuccess=printfAct;
   config.enrollFailHandler=fingerHandler;
   config.enrollSuccessHandler=fingerHandler;
   config.identifyFailHandler=fingerHandler;
   config.putInto=printfAct;
   config.takeAway=printfAct;
   config.obtainIdHandler=fingerHandler;
   config.proUsart=USART3;
   config.idDeleteSuccess=printfAct; 
   config.ResetSuccess=printfAct;
   initFingerVein(config);
}
/*处理函数*/
void printfAct(void){
  printf("success\n");
}
void fingerHandler(uint8_t *data,uint8_t length){
  printf("\n");
  for(int i=0;i<length;i++)
    printf("%02X ",data[i]);
  printf("\n");
}
