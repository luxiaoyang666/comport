/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportSwap.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(04/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "04/07/20 06:23:43"
 *                 
 ********************************************************************************/
#include "ComportSwap.h"

int comport_send(MyAttr *attr,char *sbuf,int sbuf_len)
{
    char     *ptr,*end;
    int       retval;

    if(!attr || !sbuf || sbuf_len <= 0)
    {
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }

    if(sbuf_len > attr->mSend_Len)
    {
        ptr = sbuf;
        end = sbuf + sbuf_len;

        do
        {
            if(attr->mSend_Len < (end - ptr))
            {
                retval = write(attr->fd,ptr,attr->mSend_Len);
                if(retval <= 0 || retval != attr->mSend_Len)
                {
                    printf("Write to com port[%d] failed:%s\n",attr->fd,strerror(errno));
                    return -2;
                }
           
                ptr += attr->mSend_Len;
            }
            else 
            {
                retval = write(attr->fd,ptr,(end - ptr));
                if(retval <= 0 || retval != (end - ptr))
                {
                    printf("Write to com port[%d] failed:%s\n",attr->fd,strerror(errno));
                    return -3;
                }

                ptr += (end - ptr);
            }
        }while(end > ptr);
       
    }  
       
    else 
    {  
        retval = write(attr->fd,sbuf,sbuf_len);
        if(retval <= 0 || retval != sbuf_len)
        {
            printf("Write to com port[[%d] failed:%s\n",attr->fd,strerror(errno));
            return -4;
        }
    }  
       
    return retval;
}      
       
int comport_recv(MyAttr *attr,char *rbuf,int rbuf_len,int timeout)
{      
    int                   retval;
    fd_set                rset;
    struct timeval        time_out;
       
    if(!rbuf || rbuf_len <= 0)
    {  
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }  

    if(timeout) //指定延时等待
    {    
        time_out.tv_sec = (time_t)(timeout / 1000);
        time_out.tv_usec = 0;

        FD_ZERO(&rset);
        FD_SET(attr->fd,&rset);

        retval = select(attr->fd + 1,&rset,NULL,NULL,&time_out);
        if(retval < 0)
        {
            printf("%s,Select failed:%s\n",strerror(errno));
            return -2;
        }

        else if(0 == retval)
        {
            printf("Time Out.\n");
            return 0;
        }

    }

    usleep(1000);

    retval = read(attr->fd,rbuf,rbuf_len);
    if( retval <= 0)
    {
        printf("%s,Read failed:%s\n",__func__,strerror(errno));
        return -3;
    }

    return retval;
                         
}                    
