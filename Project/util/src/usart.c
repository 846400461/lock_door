#include "stm8l15x.h"
#include "usart.h"


void USART1_Config(void)
{
  /*Enables or disables the specified peripheral clock. */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1,ENABLE);
  /*Configures the external pull-up on GPIOx pins*/
  GPIO_ExternalPullUpConfig(GPIOC,GPIO_Pin_2|GPIO_Pin_3,ENABLE);
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - USART Clock disabled
  */
  USART_Init(USART1,
             (uint32_t)115200,
             USART_WordLength_8b,
             USART_StopBits_1,
             USART_Parity_No,
             USART_Mode_Tx | USART_Mode_Rx);
  /* Enable general interrupts */
  enableInterrupts();

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
  //USART_ITConfig(USART1, USART_IT_TC, ENABLE);

  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}
void USART2_Config(void)
{
  //USART_DeInit(USART2);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2,ENABLE);
  GPIO_ExternalPullUpConfig(GPIOE,GPIO_Pin_3|GPIO_Pin_4,ENABLE);
  USART_Init(USART2,
             (uint32_t)115200,
             USART_WordLength_8b,
             USART_StopBits_1,
             USART_Parity_No,
             USART_Mode_Tx | USART_Mode_Rx);
  enableInterrupts();
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART2, USART_IT_TC, ENABLE);
  USART_Cmd(USART2, ENABLE);
  
}

void USART3_Config(void)
{
  USART_DeInit(USART3);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3,ENABLE);
  GPIO_ExternalPullUpConfig(GPIOG,GPIO_Pin_1|GPIO_Pin_0,ENABLE);
  USART_Init(USART3,
             (uint32_t)115200,
             USART_WordLength_8b,
             USART_StopBits_1,
             USART_Parity_No,
             USART_Mode_Tx | USART_Mode_Rx);
  enableInterrupts();
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART2, USART_IT_TC, ENABLE);
  USART_Cmd(USART3, ENABLE);
}

void sendData(USART_TypeDef* USARTx, uint8_t *data,uint32_t length){
  uint32_t i=0;
  for(i=0;i<length;i++){
    while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));
     USART_SendData8(USARTx,data[i]);
  }
}