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
    // datbit = 5 数据位5位(此时停止位自动为1.5位)
    // datbit = 6 数据位6位
    // datbit = 7 数据位7位
    // datbit = 8 数据位8位
    // 默认数据位为8位
    int databits;		// 数据位

    // parity = 'n' 'N' // 无校验
    // parity = 'o' 'O' // 奇校验
    // parity = 'e' 'E' // 偶校验
    // parity = 'a' 'A' // 校验位恒为1
    // parity = 'z' 'Z' // 校验位恒为0
    // 默认无校验
    char parity;		// 校验位

    // stopbits = 1 停止位为1
    // stopbits = 2 停止位为2
    // 默认1位停止位
    int stopbits;		// 停止位
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
