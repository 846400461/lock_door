#include "fingerVeinProtocol.h"

uint16_t checkSum(uint8_t* pBuf, uint32_t len){
  uint16_t sum=0;
  uint32_t i=0;
  for(i=0;i<len;i++){
    sum+=pBuf[i];
  }
  return sum;
}

void initFingerVeinPacket(struct XgPacket* xgPacket, uint8_t bCmd, uint8_t bDataLen, uint8_t* bData){
  uint8_t i=0;
  xgPacket->wPrefix=0xAABB;
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