#ifndef STATEMANAGER_H
#define STATEMANAGER_H

struct XgPacket
{
	unsigned short  wPrefix; //包标识xAABB
	unsigned char   bAddress; //设备地址，0为广播
	unsigned char   bCmd; //命令码
	unsigned char   bEncode; //数据编码，默认为0 
	unsigned char   bDataLen; //有效数据长度（字节）
	unsigned char   bData[16]; //包数据
	unsigned short  wCheckSum; //包检验，0-22字节和
};


void stateMachine(void);
void connectFingerVein(void);
#endif