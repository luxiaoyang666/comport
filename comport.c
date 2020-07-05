/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(04/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "04/07/20 03:02:28"
 *                 
 ********************************************************************************/
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include "ComportOpen.h"
#include "ComportInit.h"
#include "ComportSwap.h"


int g_stop = 0;

void sig_handler(int sig_num)
{
    if(sig_num == SIGINT)
        g_stop = 1;
}

void adjust_buf(char* buf);
void help_information();

int main(int argc, char *argv[])
{
    int              retval;
    int              ch;
    char             sbuf[128] = {0};
    char             rbuf[128] = {0};
    fd_set           rset;
    MyAttr*          attr = NULL;

    struct option    options[] = {
        {"help",no_argument,NULL,'h'},
        {"baudrate",required_argument,NULL,'b'},
        {"databits",required_argument,NULL,'d'},
        {"parity",required_argument,NULL,'p'},
        {"stopbits",required_argument,NULL,'s'},
        {"name",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
    };

    attr = (MyAttr*)malloc(sizeof(MyAttr));
    memset(attr,0,sizeof(MyAttr));
    
    while((ch = getopt_long(argc,argv,"hb:d:p:s:n:",options,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                help_information();
                return 0;

            case 'b':
                attr->BaudRate = atoi(optarg);
                break;

            case 'd':
                attr->DataBits = atoi(optarg);
                break;

            case 'p':
                attr->Parity = optarg[0];
                break;

            case 's':
                attr->StopBits = atoi(optarg);
                break;

            case 'n':
                strncpy(attr->SerialName,optarg,SERIALNAME_LEN);
                break;
                
        }
    }

    if(strlen(attr->SerialName) == 0)
    {
        printf("Parameter warning:\n");
        printf("\tAt least need to enter the serial port name,You can specify the serial port name with -n.\n");
        return 0;
    }

    if(comport_open(attr) != 0)
    {
        printf("Open %s failed!\n",attr->SerialName);
        return -1;
    }

    retval = comport_init(attr);
    if(retval < 0)
        goto cleanup;

    signal(SIGINT,sig_handler);
    
    fflush(stdin);
    printf("Start to communicate with com port......\n");

    while(!g_stop)
    {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO,&rset);
        FD_SET(attr->fd,&rset);

        /*  使用多路复用监听标准输入和串口fd */
        retval = select(attr->fd + 1,&rset,NULL,NULL,NULL);
        if(retval < 0)
        {
            printf("Program exit......\n");
            break;
        }

        if(retval == 0)
        {
            printf("Time Out.\n");
            goto cleanup;
        }

        if(FD_ISSET(STDIN_FILENO,&rset))
        {
            memset(sbuf,0,sizeof(sbuf));

            /* 从标准输入读取命令 */
            fgets(sbuf,sizeof(sbuf),stdin);

            adjust_buf(sbuf);
            
            if(comport_send(attr,sbuf,strlen(sbuf)) < 0)
            {
                printf("Write failed.\n");
                goto cleanup;
            }
            fflush(stdin);

        }

        if(FD_ISSET(attr->fd,&rset))
        {
            memset(rbuf,0,sizeof(rbuf));

            retval = comport_recv(attr,rbuf,sizeof(rbuf),0);
            if(retval <= 0)
            {
                printf("Read failed:%s\n",strerror(errno));
                break;
            }

            printf("%s",rbuf);
            fflush(stdout);
        }

        
    }

cleanup:
    comport_close(attr);

    return 0;

}

void adjust_buf(char *buf)
{
    int i = strlen(buf);
    strcpy(&buf[i-1],"\r");
}

void help_information()
{
    printf("\t-b   Set BaudRate\n");
    printf("\t-d   Set Databits\n");
    printf("\t-p   Set Parity,0 for no parity,1 for Odd parity,2 for Evev parity\n");
    printf("\t-s   Set StopBits\n");
    printf("\t-n   Set the name of the serial port you want to use\n");
    printf("\t     Ctrl + c to exit the program\n");
    printf("\n\tIf you do not specify parameters,The default parameters of the program are as follows:\n");
    printf("\tBaudRate: 1115200\n\tDatabits: 8bits\n\tParity:   no parity\n\tStopBits: 1bits\n");
}

