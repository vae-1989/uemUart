#ifndef USERTEST_H
#define USERTEST_H

#pragma   pack(push)
#pragma   pack(1)

typedef union _gpmc_reg_
{
    unsigned char reg8;
    unsigned short reg16;
}uGpmcRegVal;

typedef struct __GPMC_REG_RW__
{
    uGpmcRegVal val;
    unsigned int offset;
}sGpmcRegRW;



enum uartBaud
{
    B50,
    B75,
    B110,
    B134_5,
    B150,
    B300,
    B600,
    B1200,
    B1800,
    B2000,
    B2400,
    B3600,
    B4800,
    B7200,
    B9600,
    B19200,
    B38400,
    B57600,
    B115200
};

typedef struct __BAUD_INFO_
{
    int baud;
}sBaudInfo;



typedef struct _TERMIOS_INFO_
{
    // datbit = 5 ����λ5λ(��ʱֹͣλ�Զ�Ϊ1.5λ)
    // datbit = 6 ����λ6λ
    // datbit = 7 ����λ7λ
    // datbit = 8 ����λ8λ
    // Ĭ������λΪ8λ
    int databits;		// ����λ

    // parity = 'n' 'N' // ��У��
    // parity = 'o' 'O' // ��У��
    // parity = 'e' 'E' // żУ��
    // parity = 'a' 'A' // У��λ��Ϊ1
    // parity = 'z' 'Z' // У��λ��Ϊ0
    // Ĭ����У��
    char parity;		// У��λ

    // stopbits = 1 ֹͣλΪ1
    // stopbits = 2 ֹͣλΪ2
    // Ĭ��1λֹͣλ
    int stopbits;		// ֹͣλ
}sTermiosInfo;

enum IotclCmd
{
    eCfgSpeed,		// ���ò�����
    eCfgTermios     // ����ֹͣλ,У��λ,���ݳ���
};

#pragma pack(pop)

#endif // USERTEST_H
