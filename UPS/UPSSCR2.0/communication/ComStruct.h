//-------------------------------------------------------------
// �ļ����� "ComStruct.h"
//-------------------------------------------------------------
//���ܣ�	1.����ͨѶģ�����еĽṹ
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2017��1��12��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------
#ifndef COMSTRUCT_H_
#define COMSTRUCT_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"

/*------------------------SCIģ�鶨��-----------------------------------*/

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------
#define SCI_TXBUFFER_SIZE 12		//����SCI���ͻ�������С
#define SCI_RXBUFFER_SIZE 28		//����SCI���ջ�������С

//-------------------------------------------------------------
//ö�����͡� �ṹ������
//-------------------------------------------------------------
typedef enum Parity
{
	NoP = 0
	,OddP
	,EvenP
}Parity;
typedef struct UartFlagBits
{								//	bit	description
	Uint16 RecData:1;			//	0	���յ�������λ�������ֽڽ�����ʱ
	Uint16 SendEnd:1;			//	1	������ϱ�־λ�����ڷ�������ӳ�
	Uint16 RecFrameFlag:1;		//	2	���յ�һ�����ݱ�־
	Uint16 SendEmptyFlag:1;		//	3	���ͻ������ձ�־
	Uint16 RecOverflow:1;		//	4	�����������
}UartFlagBits;
typedef union UartFlag
{
	UartFlagBits bit;
	Uint16 all;
}UartFlag;

typedef struct SCICom
{
	//����
	Uint16 BaudRate;		//������(2000~40000 bps)
	Uint16 DataBits;		//����λ(1~8)
	Uint16 StopBits;		//ֹͣλ(1����2λ)
	Parity Parity;			//У��λ
	Uint16 SendBuffer[SCI_TXBUFFER_SIZE];	//���ͻ�����
	Uint16 RecBuffer[SCI_RXBUFFER_SIZE];	//���ջ�����
	Uint16 RecNum;			//�������ݵĸ���
	UartFlag Flag;			//��־λ
	//����
	Uint16 (*Init)(void);						//��ʼ��
	Uint16 (*ChangeState)(State state);			//�ı�״̬
	Uint16 (*SendData)(Uint16 num);				//���ͻ������е�ǰnum������
	Uint16 (*MB03Return)(float _data);			//ʹ��Modbus 03�����һ��������
	Uint16 (*Scan)(void);						//RS232ɨ�躯��
}SCICom;

/*------------------------CANģ�鶨��-----------------------------------*/

//-------------------------------------------------------------
//ö�����͡� �ṹ������
//-------------------------------------------------------------
typedef enum CanBaudRate
{
	br125k = 0
	,br250k
	,br500k
	,br750k
	,br1M
}CanBaudRate;

typedef struct CanId
{					//	bit		description
	Uint32 Id:29;	//	0-28	id
	Uint32 rsv:3;	//	29-31	����
}CanId;

typedef struct CanFlagBit
{									//	bit		description
	Uint16 TxBufEmpty:1;			//	0		���ͻ�������
	Uint16 AutoTxBufEmpty:1;		//	1		�Զ����ͻ�������
	Uint16 RecFrame:1;				//	2		���յ�����֡��־λ
	Uint16 AutoRecFrame:1;			//	3		�Զ����յ�����֡��־λ
}CanFlagBit;

typedef union CanFlag
{
	Uint16 all;
	CanFlagBit bit;
}CanFlag;

typedef enum MailBoxType
{
	SendMailBox = 0
	,AutoSendMailBox
	,RecMailBox
	,AutoRecMailBox
}MailBoxType;

typedef struct CANCom
{
	//����
	CanBaudRate BaudRate;			//������
	CanId SendId;					//��������ID
	CanId RecId;					//��������ID
	CanId RecIdMask;				//������������
	CanId AutoSendId;				//�Զ���������ID
	CanId AutoRecId;				//�Զ���������ID
	Uint16 SendBuf[8];				//�������ݻ�����
	Uint16 RecBuf[8];				//�������ݻ�����
	Uint16 RecNum;					//�������ݸ���
	Uint16 AutoRecBuf[8];			//�Զ��������ݻ�����
	Uint16 AutoRecNum;				//�Զ��������ݸ���
	CanFlag Flag;					//��־λ

	//����
	Uint16 (*Init)(void);														//��ʼ��
	Uint16 (*ChangeState)(State state);											//�ı�״̬
	Uint16 (*ChangeAutoSendState)(State state);									//�ı��Զ�����״̬
	Uint16 (*SendData)(Uint16 num,MailBoxType mbType);			//���ͻ�����ǰnum������
	Uint16 (*SendDataWithPtc)(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType) ;   //����ͨѶЭ�鷢������					//ʹ��Э�鷢�͸�����
	Uint16 (*AutoRecData)(void);												//����һ��Զ��֡���Զ���������
	Uint16 (*Scan)(void);														//ѭ��ɨ�������ж��¼���Ҫ����
}CANCom;

#endif
