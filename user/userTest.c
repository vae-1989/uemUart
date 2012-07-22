#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "userTest.h"

int uartInit(char* name, int speed, int databits, char parity, int stopbits);


// argc[1] 设备名
// argc[2] 如果有，则为向串口写入的数据
int main(int argc, char ** argv)
{
    int fd , cnt, i;
    char tmpBuf[1024];

    // 配置波特率，数据位，校验位，停止位
    fd = uartInit(argv[1], 115200, 8, 'n', 1);

    // 从串口读入数据
    cnt = read(fd, tmpBuf, 1024);
    if (cnt > 0)
    {
        for (i = 0; i < cnt; i++)
        {
            if (i % 16 == 0)
            {
                printf("\r\n");
            }
            printf("0x%02x ", tmpBuf[i]);
        }
        printf("\r\n");
    }

    if (argc > 2)
    {
        // 向串口写数据
        write(fd, argv[2], strlen(argv[2]));
    }
    return 0;
}


int uartInit(char* name, int speed, int databits, char parity, int stopbits)
{
    int fd;
    sBaudInfo baud;
    sTermiosInfo termios;

    fd = open(name, O_RDWR);
    if (fd < 0)
    {
        printf("open %s error\r\n", name);
        return fd;
    }

    switch (speed)
    {
    case 50:
        baud.baud = B50;
        break;

    case 75:
        baud.baud = B50;
        break;

    case 110:
        baud.baud = B110;
        break;

    case 134:
        baud.baud = B134_5;
        break;

    case 150:
        baud.baud = B150;
        break;

    case 300:
        baud.baud = B300;
        break;

    case 600:
        baud.baud = B600;
        break;

    case 1200:
        baud.baud = B1200;
        break;

    case 1800:
        baud.baud = B1800;
        break;

    case 2000:
        baud.baud = B2000;
        break;

    case 2400:
        baud.baud = B2400;
        break;

    case 3600:
        baud.baud = B3600;
        break;

    case 4800:
        baud.baud = B4800;
        break;

    case 7200:
        baud.baud = B7200;
        break;

    case 9600:
        baud.baud = B9600;
        break;

    case 19200:
        baud.baud = B19200;
        break;

    case 38400:
        baud.baud = B38400;
        break;

    case 57600:
        baud.baud = B57600;
        break;

    case 115200:
        baud.baud = B115200;
        break;

    default:
        baud.baud = B115200;
        break;
    }

    ioctl(fd, eCfgSpeed, &baud);

    termios.databits = databits;
    termios.parity = parity;
    termios.stopbits = stopbits;

    ioctl(fd, eCfgTermios, &termios);

    return fd;
}
