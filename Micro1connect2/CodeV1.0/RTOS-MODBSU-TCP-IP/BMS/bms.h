#ifndef _BMS_H_
#define _BMS_H_

#define CMD_0X20 0X20
#define CMD_0X21 0X21
#define CMD_0X22 0X22

#define CMD_0X40 0X40
#define CMD_0X41 0X41
#define CMD_0X42 0X42

#define CMD_0X60 0X60
#define CMD_0X61 0X61
#define CMD_0X62 0X62

#define CMD_0XFF 0XFF

#define CMD_0XA0 0XA0



float  FUN_Read_V (unsigned int id,unsigned int cmd);
float  FUN_Read_T (unsigned int id,unsigned int cmd);
void  FUN_SEND_CMD (unsigned int id,unsigned int cmd);
float FUN_READ_DATA(void);
void  FUN_SEND_JUNHENG_V (unsigned int id, unsigned int data);
char  FUN_SEND_CMD_broadcast (unsigned int id);

 extern int rerror_flag;
#endif
