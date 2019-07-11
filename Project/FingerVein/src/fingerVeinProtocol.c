#include "fingerVeinProtocol.h"

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

uint8_t getStateForFiVePacket(struct XgPacket* xgPacket){
  return xgPacket->bData[0];
}

void initFingerProtocol(void){
  fingerQueue=createQueue(3,sizeof(struct XgPacket));
}

void deInitFingerProtocol(void){
  deleteQueue(&fingerQueue);
}