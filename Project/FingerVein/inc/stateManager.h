#ifndef STATEMANAGER_H
#define STATEMANAGER_H

struct XgPacket
{
	unsigned short  wPrefix; //����ʶxAABB
	unsigned char   bAddress; //�豸��ַ��0Ϊ�㲥
	unsigned char   bCmd; //������
	unsigned char   bEncode; //���ݱ��룬Ĭ��Ϊ0 
	unsigned char   bDataLen; //��Ч���ݳ��ȣ��ֽڣ�
	unsigned char   bData[16]; //������
	unsigned short  wCheckSum; //�����飬0-22�ֽں�
};


void stateMachine(void);
void connectFingerVein(void);
#endif