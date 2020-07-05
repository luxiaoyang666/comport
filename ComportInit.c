#include "ComportInit.h"


int comport_init(MyAttr *attr)
{
    int                   retval;
    char                  baudrate[32] = {0};
    struct termios        NewTermios;


    memset(&NewTermios,0,sizeof(struct termios));
    memset(&(attr->OldTermios),0,sizeof(struct termios));
    if(!attr)
    {
        printf("Invalid parameter.\n");
        return -1;
    }

    if(tcgetattr(attr->fd,&(attr->OldTermios)))
    {
        printf("%s,Get termios to OldTermios failure:%s\n",__func__,strerror(errno));
        return -2;
    }

    if(tcgetattr(attr->fd,&NewTermios))
    {    
        printf("%s,Get termios to NewTermios failure:%s\n",__func__,strerror(errno));
        return -3;
    }  


    /* 修改控制模式，保证程序不会占用串口 */
    NewTermios.c_cflag |= CLOCAL;

/*  For example:
 *   
 *      c_cflag:   0 0 0 0 1 0 0 0
 *      CLOCAL:  | 0 0 0 1 0 0 0 0
 *              --------------------
 *                 0 0 0 1 1 0 0 0
 *                
 *  Finally:
 *
 *     c_flag = 0 0 0 1 1 0 0 0;
 *
 * */


    /* 启动接收器，能够从串口中读取输入数据 */
    NewTermios.c_cflag |= CREAD;


    /*  CSIZE字符大小掩码，将与设置databits相关的标致位置零 */
    NewTermios.c_cflag &= ~CSIZE;


/*  For example:
 *
 *      CSIZE = 0 1 1 1 0 0 0 0 ---> ~CSIZE = 1 0 0 0 1 1 1 1
 *
 *      c_cflag:    0 0 1 0 1 1 0 0
 *      ~CSIZE:  &  1 0 0 0 1 1 1 1     
 *              -----------------------
 *                  0 0 0 0 1 1 0 0
 *
 * Finally:
 *
 *     c_cflag = 0 0 0 0 1 1 00
 *
 * */

    NewTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* 
     * ICANON: 标准模式
     * ECHO: 回显所输入的字符
     * ECHOE: 如果同时设置了ICANON标志，ERASE字符删除前一个所输入的字符，WERASE删除前一个输入的单词
     * ISIG: 当接收到INTR/QUIT/SUSP/DSUSP字符，生成一个相应的信号
     *
     * */

    NewTermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* 
     * BRKINT: BREAK将会丢弃输入和输出队列中的数据(flush)，并且如果终端为前台进程组的控制终端，则BREAK将会产生一个SIGINT信号发送到这个前台进程组
     * ICRNL: 将输入中的CR转换为NL
     * INPCK: 允许奇偶校验
     * ISTRIP: 剥离第8个bits
     * IXON: 允许输出端的XON/XOF流控
     *
     * */

    /* OPOST: 表示处理后输出，按照原始数据输出 */ 
    NewTermios.c_oflag &= ~(OPOST);

    if(attr->BaudRate)
    {
        sprintf(baudrate,"B%d",attr->BaudRate);
        cfsetispeed(&NewTermios,(int)baudrate); //设置输入输出波特率
        cfsetospeed(&NewTermios,(int)baudrate);
    }
    else 
    {
        cfsetispeed(&NewTermios,B115200);
        cfsetospeed(&NewTermios,B115200);
    }

    /* 设置数据位 */
    switch(attr->DataBits)
    {
        case '5':
            NewTermios.c_cflag |= CS5;
            break;

        case '6':
            NewTermios.c_cflag |= CS6;
            break;

        case '7':
            NewTermios.c_cflag |= CS7;
            break;

        case '8':
            NewTermios.c_cflag |= CS8;
            break;

        default:
            NewTermios.c_cflag |= CS8;  //默认数据位为8
            break;
    }

    /* 设置校验方式 */
    switch(attr->Parity)
    {
        /* 无校验 */
        case 'n':
        case 'N':
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_iflag &= ~INPCK;
            break;

        /* 偶校验 */
        case 'e':
        case 'E':
            NewTermios.c_cflag |= PARENB;
            NewTermios.c_cflag &= ~PARODD;
            NewTermios.c_iflag |= INPCK;
            break;

        /* 奇校验 */
        case 'o':
        case 'O':
            NewTermios.c_cflag |= PARENB;
            NewTermios.c_cflag |= PARODD;
            NewTermios.c_iflag |= INPCK;

        /* 设置为空格 */
        case 's':
        case 'S':
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_cflag &= ~CSTOPB;

        /* 默认无校验 */
        default:
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_iflag &= ~INPCK;
            break;


    }

    /* 设置停止位 */
    switch(attr->StopBits)
    {
        case '1':
            NewTermios.c_cflag &= ~CSTOPB;
            break;

        case '2':
            NewTermios.c_cflag |= CSTOPB;
            break;

        default:
            NewTermios.c_cflag &= ~CSTOPB;
            break;
    }

    NewTermios.c_cc[VTIME] = 0;  //最长等待时间
    NewTermios.c_cc[VMIN] = 0;  //最小接收字符 

    attr->mSend_Len = 128;  //若命令长度大于mSend_Len,则每次最多发送为mSend_Len

    if(tcflush(attr->fd,TCIFLUSH))
    {
        printf("%s,Failed to clear the cache:%s\n",__func__,strerror(errno));
        return -4;
    }

    if(tcsetattr(attr->fd,TCSANOW,&NewTermios) != 0)
    {
        printf("%s,tcsetattr failure:%s\n",__func__,strerror(errno));
        return -5;
    }

    printf("Comport Init Successfully......\n");

    return 0;

}
