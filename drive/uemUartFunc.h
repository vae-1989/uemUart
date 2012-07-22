#ifndef GPMC_FUNC_H
#define GPMC_FUNC_H

#pragma   pack(push)
#pragma   pack(1)

typedef union _gpmc_reg_
{
    u8 reg8;
    u16 reg16;
}uGpmcRegVal;

typedef struct __GPMC_REG_RW__
{
    uGpmcRegVal val;
    u32 offset;
}sGpmcRegRW;



enum uartBaud
{
    UEMB50,
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


#pragma pack(pop)



extern inline u16 gpmc_reg_read16(sGpmcData *dev, u32 reg);
extern inline void gpmc_reg_write16(sGpmcData *dev, u32 reg, u16 val);
extern inline u16 gpmc_reg_read8(sGpmcData *dev, u32 reg);
extern inline void gpmc_reg_write8(sGpmcData *dev, u32 reg, u16 val);
extern int ioctlReadReg8(sGpmcData *dev, unsigned long arg);
extern int ioctlWriteReg8(sGpmcData *dev, unsigned long arg);
extern int ioctlReadReg16(sGpmcData *dev, unsigned long arg);
extern int ioctlWriteReg16(sGpmcData *dev, unsigned long arg);
extern int CfgSpeed(sGpmcData *dev, unsigned long arg);
extern int CfgTermios(sGpmcData *dev, unsigned long arg);


#endif // GPMC_FUNC_H
