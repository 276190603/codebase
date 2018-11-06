//-------------------------------------------------------------
// �ļ����� "Communication.h"
//-------------------------------------------------------------
//���ܣ�	1.����Communication.c����ͷ�ļ�
//		2.������غ궨��
//      3.����ȫ�ֱ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2017��1��9��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef _COMMUNICATION
#define _COMMUNICATION

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "ComStruct.h"
#include "RS232.h"
#include "Inverter.h"
#include "GlobalDefine.h"
#include "RS485.h"
#include "CAN.h"

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------

//Modbus�и������ݡ�����ļĴ�����ַ(������)
#define INV_START   0           //������A���ѹ

#define UA_INVOUT_ADD	1			//������A���ѹ

#define IA_INVOUT_ADD	4			//������A�����

#define FRA_BP_ADD		7			//��·A��Ƶ��

#define FRA_INV_ADD      8           //��·A��Ƶ��

#define UA_BP_ADD 9           //������A���ѹ��Чֵ

#define UA_INVOUTR_ADD 10			//������A���ѹ��Чֵ

#define Phase_INV_BP 12           //������A���ѹ��Чֵ

//-------------------------------------------------------------
// �ṹ��������ö������
//-------------------------------------------------------------
typedef enum ComModule
{
	RS232Mdl = 0
	,RS485Mdl
	,CanMdl
}ComModule;

typedef struct ComEnableBits	//ʹ��λλ����
{								//	bit	description
	Uint16 Enable:1;			//	0	ȫ��ͨѶʹ��
	Uint16 Rs232Enable:1;		//	1	232ʹ��λ
	Uint16 Rs485Enable:1;		//	2	484ʹ��λ
	Uint16	CanEnable:1;		//	3	Canʹ��λ
}ComEnableBits;
typedef union ComEnable
{
	ComEnableBits bit;
	Uint16 all;
}ComEnable;


typedef struct Communication
{
	//����
	ComEnable Enable;		//ģ��ʹ��
	SCICom RS232Com;		//232ģ��
	SCICom RS485Com;		//485ģ��
	CANCom CANCom;			//Canģ��

	//����
	Uint16 (*Init)(void);
	Uint16 (*ChangeState)(State state);
}Communication;

//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern Communication myCommunication;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UsemyCommunication(void);
Uint16 Write2Buf(float _data,ComModule _md);
Uint16 Write06Buf(ComModule _md);
Uint16 Write16Buf(ComModule _md);
void Float2Uint16(float _fData,Uint16 *_u16Data);
Uint16 VerifyFrame(ComModule _md);

#endif
