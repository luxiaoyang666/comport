/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportOpen.c
 *    Description:  This file Open or close the serial port
 *                 
 *        Version:  1.0.0(03/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "03/07/20 17:50:09"
 *                 
 ********************************************************************************/
#include "ComportOpen.h"

int comport_open(MyAttr *attr)
{
    int                i;
    int                retval = -1;

    if(NULL == attr)
    {
        printf("%s,Invalid parameter\n",__func__);
        return retval;
    }

    /* O_NOCTTY表示打开的是一个终端设备，程序不会成为该
     * 端口的控制终端,O_NONBLOCK使得read处于非阻塞模式 */
    attr->fd = open(attr->SerialName,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(attr->fd < 0)
    {
        printf("%s,Open %s failed:%s\n",__func__,attr->SerialName,strerror(errno));
        return -1;
    }

    /* 检查串口是否处于阻塞态 */
    if((retval = fcntl(attr->fd,F_SETFL,0)) < 0)
    {
        printf("%s,Fcntl check faile.\n",__func__);
        return -2;
    }

    printf("Starting serial communication process ");

    for(i = 0;i < 6;i++)
    {
        printf(" . ");
        fflush(stdout);
        usleep(10000);
    }
    printf("\n");  //这部分纯属搞笑


    if(0 == isatty(attr->fd))
    {
        printf("%s:[%d] is not a Terminal equipment.\n",attr->SerialName,attr->fd);
        return -3;
    }

    printf("Open %s successfully.\n",attr->SerialName);

    return 0;
}

int comport_close(MyAttr *attr)
{
    if(tcflush(attr->fd,TCIOFLUSH))  //清零用于串口通信的缓冲区
    {
        printf("%s,Tcflush faile:%s\n",__func__,strerror(errno));
        return -1;
    }

    /* 将串口设置为原有属性 */
    if(tcsetattr(attr->fd,TCSANOW,&(attr->OldTermios)))
    {
        printf("%s,Set old options failed:%s\n",__func__,strerror(errno));
        return -2;
    }

    close(attr->fd);

    free(attr);

    return 0;
}

