#ifndef _UART5_H_
#define _UART5_H_

// #define CMD_0X20 0X20
// #define CMD_0X21 0X21
// #define CMD_0X22 0X22

// #define CMD_0X40 0X40
// #define CMD_0X41 0X41
// #define CMD_0X42 0X42

// #define CMD_0X60 0X60
// #define CMD_0X61 0X61
// #define CMD_0X62 0X62

// #define CMD_0XFF 0XFF

// #define CMD_0XA0 0XA0



float  UART5_Read_V (unsigned int id,unsigned int cmd);
float  UART5_Read_T (unsigned int id,unsigned int cmd);
void  UART5_SEND_CMD (unsigned int id,unsigned int cmd);
float UART5_READ_DATA(void);
void  UART5_SEND_JUNHENG_V (unsigned int id, unsigned int data);
char  UART5_SEND_CMD_broadcast (unsigned int id);

 extern int rerror_flag;
#endif
