
/*
 * vae uemUart
 *
 */

#include <linux/cdev.h>
#include <linux/kfifo.h>

#ifndef _GPMC_DRIVER_
#define _GPMC_DRIVER_

#define SINOROCK_FPGA_MAGIC  'P'

#define IOCTRL_FPGA_GET_FIREWAREV  _IOW(SINOROCK_FPGA_MAGIC,1,int)
#define IOCTRL_FPGA_BUS_TEST  _IOW(SINOROCK_FPGA_MAGIC,2,int)
#define IOCTRL_FPGA_SET_LEN  _IOW(SINOROCK_FPGA_MAGIC,3,int)
#define IOCTRL_FPGA_GET_LEN  _IOW(SINOROCK_FPGA_MAGIC,4,int)

#define GPMC_DRV_VERSION	"2012-7-11"
#define DEVICE_NAME 		"uem-uart"
#define UEM_UART_MAJOR		234	// 0 为动态分派
#define UEM_UART_NR_DEVS	4
#define UART_BUF_SIZE	1024
#define UART_FIFO_SIZE	(1024)

#define OMAP_GPMC_CS  3

typedef struct __GPMC_DATA__
{
    void* __iomem ioaddr;
    struct cdev cdev;
    int gpmcCS;
	int devno;			// 设备号
    spinlock_t dev_lock;
    unsigned long cs_mem_base;
	unsigned int dateCnt;
    unsigned char  *data_buf;
	struct kfifo *pFifo;
	spinlock_t fifoLock;
}sGpmcData;

enum IotclCmd
{
    eCfgSpeed,		// 设置波特率
    eCfgTermios     // 配置停止位,校验位,数据长度
};





#endif 	/*_GPMC_DRIVER_*/

