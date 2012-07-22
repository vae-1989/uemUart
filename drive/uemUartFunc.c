#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <plat/gpmc.h>
#include <linux/clk.h>
#include <plat/clock.h>
#include <linux/ioport.h>
#include <linux/ioctl.h>

#include "mach/vaedebug.h"
//#include "mach/registers.h"
#include "mach/types.h"

#include "uemUartDriver.h"
#include "uemUartFunc.h"




inline u16 gpmc_reg_read16(sGpmcData *dev, u32 reg)
{
    u16 data;
    u16* address;
    address = (u16*)dev->ioaddr + reg;
    data = ioread16(address);
    //VaeDebug("data = 0x%x\r\n", data);
    return data;

}
//write data to gpmc
inline void gpmc_reg_write16(sGpmcData *dev, u32 reg, u16 val)
{
    u16* address ;
    address = (u16*)dev->ioaddr + reg;
    iowrite16(val ,address);
    return ;
}

inline u16 gpmc_reg_read8(sGpmcData *dev, u32 reg)
{
    u8 data;
    u8* address;
    address = (u8*)dev->ioaddr + reg;
    data = ioread8(address);
    //VaeDebug("data = %d\r\n", data);
    return data;

}
//write data to gpmc
inline void gpmc_reg_write8(sGpmcData *dev, u32 reg, u16 val)
{
    u8* address ;
    address = (u8*)dev->ioaddr + reg;
    iowrite8(val ,address);
    return ;
}


int ioctlReadReg8(sGpmcData *dev, unsigned long arg)
{
    sGpmcRegRW *reg;
    u32 offset;

    reg = (sGpmcRegRW *)arg;

    offset = reg->offset;
    reg->val.reg8 = gpmc_reg_read8(dev, offset);

    return 0;
}

int ioctlWriteReg8(sGpmcData *dev, unsigned long arg)
{
    sGpmcRegRW *reg;
    u32 offset;

    reg = (sGpmcRegRW *)arg;

    offset = reg->offset;
    gpmc_reg_write8(dev, offset, reg->val.reg8);

    return 0;
}


int ioctlReadReg16(sGpmcData *dev, unsigned long arg)
{
    sGpmcRegRW *reg;
    u32 offset;

    reg = (sGpmcRegRW *)arg;

    offset = reg->offset;
    reg->val.reg16 = gpmc_reg_read16(dev, offset);

    return 0;
}

int ioctlWriteReg16(sGpmcData *dev, unsigned long arg)
{
    sGpmcRegRW *reg;
    u32 offset;

    reg = (sGpmcRegRW *)arg;

    offset = reg->offset;
    gpmc_reg_write16(dev, offset, reg->val.reg16);

    return 0;
}


int CfgSpeed(sGpmcData *dev, unsigned long arg)
{
    sBaudInfo *pBaud;
    sGpmcRegRW cfg;
    unsigned long cfgAdd;
    int tport;
    u16 regTmp;

    pBaud = (sBaudInfo *)arg;
    cfgAdd = (unsigned long)&cfg;
    tport = MINOR(dev->devno)*0x08;

    cfg.offset = tport + 0x03;
    cfg.val.reg16 = 0x0;
    ioctlReadReg16(dev , cfgAdd);
    regTmp = cfg.val.reg16;

    cfg.offset = tport + 0x03;
    cfg.val.reg16 = regTmp| (1<<7);
    ioctlWriteReg16(dev , cfgAdd);

    printk(KERN_DEBUG"baud = %d, tport = %d\r\n", pBaud->baud, tport);
    switch (pBaud->baud)
    {
    case UEMB50:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0009;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B75:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0006;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B110:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0017;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0004;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B134_5:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0059;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0003;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B150:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0003;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B300:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0080;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0001;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B600:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x00c0;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B1200:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0060;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B1800:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0040;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B2000:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x003a;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B2400:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0030;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B3600:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0020;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B4800:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0018;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B7200:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0010;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B9600:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x000c;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B19200:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0006;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B38400:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0003;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B57600:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0002;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    case B115200:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0001;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;

    default:
        cfg.offset = tport + 0x00;
        cfg.val.reg16 = 0x0001;
        ioctlWriteReg16(dev , cfgAdd);

        cfg.offset = tport + 0x01;
        cfg.val.reg16 = 0x0000;
        ioctlWriteReg16(dev , cfgAdd);
        break;
    }

    cfg.offset = tport + 0x03;
    cfg.val.reg16 = regTmp;
    cfg.val.reg16 = 0x03;
    ioctlWriteReg16(dev , cfgAdd);

    return 0;
}

int CfgTermios(sGpmcData *dev, unsigned long arg)
{
    sTermiosInfo *pTermios;
    sGpmcRegRW cfg;
    unsigned long cfgAdd;
    int tport;
    u16 regTmp;

    cfgAdd = (unsigned long)&cfg;
    tport = MINOR(dev->devno)*0x08;

    pTermios = (sTermiosInfo *)arg;

    cfg.offset = tport + 0x03;
    cfg.val.reg16 = 0x0;
    ioctlReadReg16(dev , cfgAdd);
    regTmp = cfg.val.reg16 && 0x3f;
    //printk("cfg.val.reg16 0x%x\r\n" ,cfg.val.reg16);
    printk("pTermios d=%d, p=%c, s=%d\r\n", pTermios->databits, pTermios->parity, pTermios->stopbits);


    switch (pTermios->databits)
    {
    case 5:
        // 数据位5位(此时停止位自动为1.5位)
        regTmp |= 0x00;
        break;

    case 6:
        // 数据位6位
        regTmp |= 0x01;
        break;

    case 7:
        // 数据位7位
        regTmp |= 0x02;
        break;

    case 8:
        // 数据位8位
        regTmp |= 0x3;
        break;

    default:
        // 默认是8位停止位
        regTmp |= 0x3;
        break;
    }
    //printk("regTmp 0x%x\r\n" ,regTmp);

    switch (pTermios->parity)
    {
    case 'n':
    case 'N':
        // 无校验
        regTmp |=0x0;
        break;

    case 'o':
    case 'O':
        // 奇校验
        regTmp |=0x08;
        break;

    case 'e':
    case 'E':
        // 偶校验
        regTmp |=0x18;
        break;

    case 'a':
    case 'A':
        // 校验恒为1
        regTmp |= 0x28;
        break;

    case 'z':
    case 'Z':
        // 校验恒为0
        regTmp |=0x38;
        break;

    default:
        // 默认是无停止位
        regTmp |=0x0;
        break;

    }
    //printk("regTmp 0x%x\r\n" ,regTmp);

    switch (pTermios->stopbits)
    {
    case 1:
        // 1停止位
        regTmp |=0x00;
        break;

    case 2:
        // 2停止位
        regTmp |=0x04;
        break;

    default:
        // 默认1停止位
        regTmp |=0x00;
        break;
    }


    cfg.offset = tport + 0x03;
    cfg.val.reg16 = regTmp;
    ioctlWriteReg16(dev , cfgAdd);
    //printk("regTmp 0x%x\r\n" ,regTmp);

    return 0;
}

