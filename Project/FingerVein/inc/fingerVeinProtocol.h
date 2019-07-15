#ifndef FINGER_VEIN_PROTOCOL_H
#define FINGER_VEIN_PROTOCOL_H

#include "stm8l15x.h"
#include "myqueue.h"

extern xQueue fingerQueue;

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

/*�豸���ָ��*/
#define XG_CMD_CONNECTION             0x01 //�����豸,�����λ�������룬Ĭ������ȫΪ(0x30)
#define XG_CMD_CLOSE_CONNECTION       0x02 //�ر�����
#define XG_CMD_GET_SYSTEM_INFO        0x03 //��ȡ�汾�ź�������Ϣ
#define XG_CMD_FACTORY_SETTING        0x04 //�ָ���������
#define XG_CMD_SET_DEVICE_ID          0x05 //�����豸���-255
#define XG_CMD_SET_BAUDRATE           0x06 //���ò�����-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //���ð�ȫ�ȼ�-4
#define XG_CMD_SET_TIMEOUT            0x08 //���õȴ���ָ���볬ʱ-255��
#define XG_CMD_SET_DUP_CHECK          0x09 //�����ظ���¼���-1
#define XG_CMD_SET_PASSWORD           0x0A //����ͨ������
#define XG_CMD_CHECK_PASSWORD         0x0B //��������Ƿ���ȷ
#define XG_CMD_REBOOT                 0x0C //��λ�����豸
#define XG_CMD_SET_SAME_FV            0x0D //�Ǽǵ�ʱ�����Ƿ�Ϊͬһ����ָ

/*ʶ�����ָ��*/
#define XG_CMD_FINGER_STATUS          0x10 //�����ָ����״̬
#define XG_CMD_CLEAR_ENROLL           0x11 //���ָ��ID��¼����
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //�������ID��¼����
#define XG_CMD_GET_EMPTY_ID           0x13 //��ȡ�գ��޵�¼���ݣ�ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //��ȡ�ܵ�¼�û�����ģ����
#define XG_CMD_GET_ID_INFO            0x15 //��ȡָ��ID��¼��Ϣ
#define XG_CMD_ENROLL                 0x16 //ָ��ID��¼
#define XG_CMD_VERIFY                 0x17 //1:1��֤��:Nʶ��
#define XG_CMD_IDENTIFY_FREE          0x18 //FREEʶ��ģʽ���Զ�ʶ�𲢷���״̬
#define XG_CMD_CANCEL                 0x19 //ȡ��FREEʶ��ģʽ
#define XG_CMD_RAM_MODE               0x1A //�ڴ����ģʽ

/******************************�߼�ָ��****************************************/
/*���ݶ�д���ָ��*/
#define XG_CMD_READ_DATA              0x20 //���豸��ȡ����
#define XG_CMD_WRITE_DATA             0x21 //д�����ݵ��豸
#define XG_CMD_READ_ENROLL            0x22 //��ȡָ��ID��¼����
#define XG_CMD_WRITE_ENROLL           0x23 //д�루���ǣ�ָ��ID��¼����

/******************************�������****************************************/
#define	XG_ERR_SUCCESS               0x00 //�����ɹ�
#define	XG_ERR_FAIL                  0x01 //����ʧ��
#define XG_ERR_COM                    0x02 //ͨѶ����
#define XG_ERR_DATA                   0x03 //����У�����
#define XG_ERR_INVALID_PWD            0x04 //�������
#define XG_ERR_INVALID_PARAM          0x05 //��������
#define XG_ERR_INVALID_ID             0x06 //ID����
#define XG_ERR_EMPTY_ID               0x07 //ָ��IDΪ�գ��޵�¼���ݣ�
#define XG_ERR_NOT_ENOUGH             0x08 //���㹻��¼�ռ�
#define XG_ERR_NO_SAME_FINGER         0x09 //����ͬһ����ָ
#define XG_ERR_DUPLICATION_ID         0x0A //����ͬ��¼ID
#define XG_ERR_TIME_OUT               0x0B //�ȴ���ָ���볬ʱ
#define XG_ERR_VERIFY                 0x0C //��֤ʧ��
#define XG_ERR_NO_NULL_ID             0x0D //���޿�ID
#define XG_ERR_BREAK_OFF              0x0E //�����ж�
#define XG_ERR_NO_CONNECT             0x0F //δ����

/******************************״̬����****************************************/
#define XG_INPUT_FINGER               0x20 //���������ָ
#define XG_RELEASE_FINGER             0x21 //�����ÿ���ָ


uint16_t checkSum(uint8_t* pBuf, uint32_t len);

void initFingerVeinPacket(struct XgPacket* xgPacket, uint8_t bCmd, uint8_t bDataLen, uint8_t* bData);

void deInitFingerProtocol(void);

void initFingerProtocol(void);

uint8_t getStateForFiVePacket(struct XgPacket* xgPacket);

void initFingerVeinPacketNobData(struct XgPacket* xgPacket, uint8_t bCmd, uint8_t bDataLen);

void receiveFingerProtocolData(uint8_t temp);

#endif