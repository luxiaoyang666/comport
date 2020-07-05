#ifndef  _COMPORTOPEN_H_
#define  _COMPORTOPEN_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define SERIALNAME_LEN 128

typedef struct _st_MyAttr {

    int               fd;        //串口文件描述符
    int               BaudRate;  //波特率
    int               DataBits;  //数据位
    char              Parity;    //奇偶校验位
    int               StopBits;  //停止位
    int               mSend_Len; //单次最大发送长度
    char              SerialName[SERIALNAME_LEN];  //串口名称
    struct termios    OldTermios;  //串口的原始属性
}MyAttr;

int comport_open(MyAttr *attr);
int comport_close(MyAttr *attr);

#endif   /* ----- #ifndef _COMPORTOPEN_H_  ----- */
