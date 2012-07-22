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
	int databits;		// 数据位
	char parity;			// 校验位
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
