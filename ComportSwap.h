#ifndef  _COMPORTSWAP_H_
#define  _COMPORTSWAP_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "ComportOpen.h"

int comport_send(MyAttr *attr,char *sbuf,int sbuf_len);
int comport_recv(MyAttr *attr,char *rbuf,int rbuf_len,int timeout);

#endif   /* ----- #ifndef _COMPORTSWAP_H_  ----- */
