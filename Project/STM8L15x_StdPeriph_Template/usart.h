#ifndef USART_H
#define USART_H

void USART1_Config(void);
void USART2_Config(void);
void USART3_Config(void);
void sendData(USART_TypeDef* USARTx, uint8_t *data,uint32_t length);
#endif