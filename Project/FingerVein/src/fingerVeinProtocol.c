#include "fingerVeinProtocol.h"
#include <stdio.h>
#include "stateManager.h"

struct XgPacket xgPacket;

uint16_t checkSum(uint8_t* pBuf, uint32_t len){
  uint16_t sum=0;
  uint16_t temp=0;
  uint32_t i=0;
  for(i=0;i<len;i++){
    sum+=pBuf[i];
  }
  temp=sum&0xFF;
  sum>>=8;
  sum|=(temp<<8);
  return sum;
}

void initFingerVeinPacket(struct XgPacket* xgPacket, uint8_t bCmd, uint8_t bDataLen, uint8_t* bData){
  uint8_t i=0;
  xgPacket->wPrefix=0xBBAA;
  xgPacket->bAddress=0x00;
  xgPacket->bCmd=bCmd;
  xgPacket->bEncode=0x00;
  xgPacket->bDataLen=bDataLen;
  for(i=0;i<bDataLen;i++)
    xgPacket->bData[i]=bData[i];
  for(i;i<16;i++)
    xgPacket->bData[i]=0;
  xgPacket->wCheckSum=checkSum((uint8_t*)xgPacket,22);

}

void initFingerVeinPacketNobData(struct XgPacket* xgPacket, uint8_t bCmd, uint8_t bDataLen){
  uint8_t i=bDataLen;
  xgPacket->wPrefix=0xBBAA;
  xgPacket->bAddress=0x00;
  xgPacket->bCmd=bCmd;
  xgPacket->bEncode=0x00;
  xgPacket->bDataLen=bDataLen;
  for(i;i<16;i++)
    xgPacket->bData[i]=0;
  xgPacket->wCheckSum=checkSum((uint8_t*)xgPacket,22);
}

uint8_t getStateForFiVePacket(struct XgPacket* xgPacket){
  return xgPacket->bData[0];
}

void initFingerProtocol(void){
  fingerQueue=createQueue(4,sizeof(struct XgPacket));
}

void deInitFingerProtocol(void){
  deleteQueue(&fingerQueue);
}

void receiveFingerProtocolData(uint8_t temp){
  static uint8_t count=0;
  uint16_t checkSum=0;
  uint8_t *data;
  static struct XgPacket usart3XgPacket;
  switch(count){
  case 0:

    if(temp!=0xBB) return;
    usart3XgPacket.wPrefix=(temp<<8)|0x0000;
    break;
  case 1:
  
    if(temp!=0xAA){
      count=0;
      return;
    }
    usart3XgPacket.wPrefix|=temp;
    break;
  case 2:
    
    if(temp!=0x00){
      count=0;
      return;
    }
    usart3XgPacket.bAddress=temp;
    break;
  case 3:
 
    usart3XgPacket.bCmd=temp;
    break;
  case 4:
   
    usart3XgPacket.bEncode=temp;
    break;
  case 5:
 
    usart3XgPacket.bDataLen=temp;
    break;
  }
  
  if(count>5&&count<22)
    usart3XgPacket.bData[count-6]=temp;
  
  if(count==22)
    usart3XgPacket.wCheckSum=0x0000|temp;
  
  if(count==23){
    //USART_SendData8(USART1,0x01);
    usart3XgPacket.wCheckSum|=(temp<<8);
    data=(uint8_t*)&usart3XgPacket;
    for(int i=0;i<22;i++){
      checkSum+=data[i];
    }
    //USART_SendData8(USART1,0x02);
    //sendData(USART1,&checkSum,2);
    //sendData(USART1,&usart3XgPacket.wCheckSum,2);
    if(checkSum!=usart3XgPacket.wCheckSum){
      count=0;
      return;
    }
    //USART_SendData8(USART1,0x03);
    enqueue_t(fingerQueue,(void*)&usart3XgPacket);
    //sendData(USART1,(uint8_t*)&usart3XgPacket,24);
    data=(uint8_t*)&usart3XgPacket;
    fingerLog("\nreach data : ");
    for(int i=0;i<24;i++)
      fingerLog("%02X ",data[i]);
    fingerLog("\n");
    count=0;
    return;
  }
    
  count++;
}