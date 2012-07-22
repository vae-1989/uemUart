
/*
 *	uemUart driver for OMAP-3530
 *	Copyright (C) 2012  Vae
 *
 *
 */
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
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>  
#include <linux/kfifo.h>
#include <linux/delay.h>

#include "mach/vaedebug.h"
#include "mach/types.h"

#include "uemUartDriver.h"
#include "uemUartFunc.h"


/***********ȫ�ֱ��� start*********/
sGpmcData *pUemUartDev = NULL;
static int uemUartMajor = UEM_UART_MAJOR;
static int uemUartMinor = 0;
static int uemUartNrDevs = UEM_UART_NR_DEVS;

/***********ȫ�ֱ��� end  *********/

/* �Բ����ķ�ʽ����ָ���豸�����豸��*/
module_param(uemUartMajor, int, S_IRUGO);
module_param(uemUartMinor, int, S_IRUGO);
module_param(uemUartNrDevs, int, S_IRUGO);


int uartInit(sGpmcData* pGpmc);

/**********************************************************
 * ������:uemUartOpen
 *
 * ����:
 * ���豸�ļ�
 *
 * �������:
 * @inode �����ļ��Ľṹ
 * @file �ļ�������
 *
 * ����ֵ
 *
 *
 **********************************************************/

static int uemUartOpen(struct inode *inode, struct file *filp)
{
	sGpmcData *pDev;
	int myMinor, myMajor;
	pDev = container_of(inode->i_cdev,sGpmcData, cdev);
    filp->private_data = pDev;		/* ���豸�ṹ��ָ�븳ֵ���ļ�˽������ָ��*/
	myMajor = imajor(inode);
	myMinor = iminor(inode);
	pDev = &pUemUartDev[myMinor];
	//pDev->devno = MKDEV(myMajor, myMinor);
//	printk(KERN_DEBUG "pdev = %p, filp = %p", pDev, filp);
//	printk(KERN_DEBUG "devno = 0x%x, Major = %d, myMinor = %d\r\n", pDev->devno, myMajor, myMinor);
//    printk(KERN_DEBUG "uem uart open\r\n");
//	printk(KERN_DEBUG "pdev = %p, filp = %p", pDev, filp);
	
    return 0;
}

/**********************************************************
 * ������:uemUartRelease
 *
 * ����:
 * �ͷ�gpmc�豸
 *
 * �������:
 * @inode �����ļ��Ľṹ
 * @file �ļ�������
 *
 * ����ֵ
 *
 *
 **********************************************************/
static int uemUartRelease(struct inode *inode, struct file *filp)
{
//    printk(KERN_DEBUG "uem uart Close\n");
    return 0;
}


/**********************************************************
 * ������:uemUartIoctl
 *
 * ����:
 * �豸��ioctl����
 *
 * �������:
 * @inode �����ļ��Ľṹ
 * @file �ļ�������
 * @cmd ��������
 * @arg ��������
 *
 * ����ֵ
 * ��
 *
 **********************************************************/
static int uemUartIoctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = -1;
    sGpmcData *dev = (sGpmcData *)(filp->private_data);
    switch (cmd)
    {
    case eCfgSpeed:
        ret = CfgSpeed(dev, arg);
        break;

    case eCfgTermios:
        ret = CfgTermios(dev, arg);
        break;
		
	default:
		break;
    }

    return ret;
}


/**********************************************************
 * ������:uemUartRead
 *
 * ����:
 * ���ں��ж��ļ����û�  kernel-->user
 *
 * �������:
 * @file �ļ�������
 * @buf �û�����
 * @size ��ȡ����
 * @ppos ��ȡƫ��
 *
 * ����ֵ
 * ��
 *
 ************************************************************/
static ssize_t uemUartRead(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	sGpmcData *pDev;
    int ret = 1;
    unsigned int count = size;

	pDev = filp->private_data;

//	printk(KERN_DEBUG "pdev = %p dev = 0x%x, filp=%p", pDev, filp->f_mapping->host->i_cdev->dev, filp);
//    printk(KERN_DEBUG "uemUart Read count = %d\n", count);
//	VaeDebug("devno = 0x%x, MAJOR = %d, minor = %d ", pDev->devno, MAJOR(pDev->devno), MINOR(pDev->devno)) ;

	count = __kfifo_get(pDev->pFifo, pDev->data_buf, count);
	if (copy_to_user(buf,(pDev->data_buf), count))
    {
        ret = -EFAULT;
    }
    else
    {
        ret = count;
 //       printk(KERN_DEBUG "read %d bytes(s) \r\n", count);
    }

    return ret;
}


/**********************************************************
 * ������:uemUartWrite
 *
 * ����:
 * ���û�д�ļ����ں�  user-->kernel
 *
 * �������:
 * @file �ļ�������
 * @buf �ں�
 * @size ��ȡ����
 * @ppos ��ȡƫ��
 *
 * ����ֵ
 * ��
 *
 ************************************************************/
static ssize_t uemUartWrite(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	sGpmcRegRW val;
	sGpmcData *pDev;
	int ret , i;
	size_t count ;
	u16 transmit_empty;
	int tport;
	unsigned long arg;

	pDev = filp->private_data;
	arg = (unsigned long )&val;
	count = size;

//	printk(KERN_DEBUG "uemUartWrite\n");
//	printk(KERN_DEBUG "pdev = %p dev = 0x%x, filp=%p", pDev, filp->f_mapping->host->i_cdev->dev, filp);
//	VaeDebug("devno = 0x%x, MAJOR = %d, minor = %d ", pDev->devno, MAJOR(pDev->devno), MINOR(pDev->devno)) ;

	// ��ȡ�˿ں�
	tport = MINOR(pDev->devno) * 0x08;

	if (count > UART_BUF_SIZE ) 	// ���Ҫд������ݹ���
	{
		count = UART_BUF_SIZE ;
	}

	if (copy_from_user(pDev->data_buf , buf , count))
	{
		ret =  -EFAULT;
	}
	else
	{
		ret = count;
		pDev->dateCnt = count;
//		printk(KERN_DEBUG "written %d bytes(s) \r\n", count);
	}

	for (i = 0; i< ret; i++)
	{
		do
		{// ����Ƿ��д
			val.offset = tport + 0x05;
		  	val.val.reg16 = 0x0080;
			ioctlReadReg16(pDev , arg);
			transmit_empty = val.val.reg16;
		}while( (transmit_empty & 0x0040) != 0x0040 );
	
		val.offset = tport + 0x00;
	  	val.val.reg16 = pDev->data_buf[i];
		ioctlWriteReg16(pDev , arg);
	}

	return ret;

}

/***********************************************************
 * ������:uemUartLlseek
 *
 * ���ܣ�
 * ����lseek���������ļ�ƫ��
 *
 * ���������
 * @file �ļ�������
 * @offsetƫ����
 * @origƫ�ƹ���
 *
 * ����ֵ��
 * 0:�ɹ�
 * ����ʧ��
 **********************************************************/
static loff_t uemUartLlseek(struct file *filp , loff_t offset , int orig)
{
#if 0
    loff_t ret = 0 ;
    printk(KERN_DEBUG "orig = %x offset = %x\n" , orig , offset);

    switch (orig)
    {
        case SEEK_SET:	/* ����ļ���ʼλ��ƫ�� */
            if( offset < 0 )
            {
                ret = - EINVAL;
                break;
            }
            if ((unsigned int)offset > FPGA_BUF_SIZE)
            {
                ret = -EINVAL;
                break;
            }
            filp->f_pos = (unsigned int)offset;
            ret = filp ->f_pos;
            break;
        case SEEK_CUR:	/* ����ļ���ǰλ�õ�ƫ�� */
            if((filp ->f_pos + offset ) > FPGA_BUF_SIZE )
            {
                ret = -EINVAL;
                break;
            }
            if ((filp ->f_pos + offset ) < 0)
            {
                ret = - EINVAL;
                break;
            }
            filp ->f_pos += offset;
            ret = filp ->f_pos ;
            break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
#endif
    return 0;

}


int readFromUart(int index)
{		
	sGpmcData *pDev;
	sGpmcRegRW val;
	int cnt;
	unsigned long arg;
	u16 data_ready;
	int tport;
	unsigned char cbuf[1024];
// read from uem uart 1

	cnt = 0;
	pDev = &pUemUartDev[index];
	arg = (unsigned long )&val;
	tport = index * 0x08;
	
	while (1)
	{
		val.offset = tport + 0x05;
 		val.val.reg16 = 0x0000;
		ioctlReadReg16(pDev, arg);
 		data_ready = val.val.reg16;
		if ( (data_ready & 0x0001) == 0x0001)
		{// ѭ����������
			val.offset = tport;
   			val.val.reg16 = 0x0000;
	 		ioctlReadReg16(pDev, arg);
			cbuf[cnt++] = val.val.reg16;	
			if (cnt >= 1024)
			{
				break;
			}
			
		}
		else
		{
			break;
		}
	}


	__kfifo_put(pUemUartDev[index].pFifo, cbuf, cnt);
	return cnt;
	
}


irqreturn_t uemUartIrqFunc(int irqNo, void* dev_id)
{
	sGpmcRegRW val;
	sGpmcData *pDev;
//	int cnt;
	u32 tport;
	unsigned long arg;
//	u16 data_ready;
//	unsigned char cbuf[1024];

	tport = 0;
	arg = (unsigned long )&val;

	switch (irqNo)
	{
		case OMAP_GPIO_IRQ(168):
			readFromUart(0);
		
		break;
			
		case OMAP_GPIO_IRQ(127):
			readFromUart(1);

		break;
		
		case OMAP_GPIO_IRQ(128):
			readFromUart(2);
			
		break;

		case OMAP_GPIO_IRQ(129):
			readFromUart(3);
		
		break;
		
		default:
			pDev = NULL;
			VaeDebug("error");
		break;
	}
	
	return 0;
}


// �����ж�
int uartIrqTestInit(void)
{
    int ret, i;
	char nameBuf[64];

	for (i = 127; i <= 129; i++)
	{
		sprintf(nameBuf, "gpio-%d", i);
		ret = gpio_request(i,nameBuf);
		if (ret < 0)
		{
			VaeDebug("ret = %d\r\ngpio_request error\r\n", ret);
		}

		gpio_direction_input(i);

		set_irq_type(OMAP_GPIO_IRQ(i),IRQ_TYPE_EDGE_FALLING);	// �½��ش���

		sprintf(nameBuf, "gpio-%d-irq", i);
		ret = request_irq(OMAP_GPIO_IRQ(i), uemUartIrqFunc, 0, "uemUartIrq", NULL);	
		if (ret < 0)
		{
			VaeDebug("ret = %d irq error\r\n", ret);
		}
	}

	i = 168;
	sprintf(nameBuf, "gpio-%d", i);
	ret = gpio_request(i,nameBuf);
	if (ret < 0)
	{
		VaeDebug("ret = %d\r\ngpio_request error\r\n", ret);
	}

	gpio_direction_input(i);
	set_irq_type(OMAP_GPIO_IRQ(i),IRQ_TYPE_EDGE_FALLING);	// �½��ش���
	sprintf(nameBuf, "gpio-%d-irq", i);
	ret = request_irq(OMAP_GPIO_IRQ(i), uemUartIrqFunc, 0, "uemUartIrq", NULL);	
	if (ret < 0)
	{
		VaeDebug("ret = %d irq error\r\n", ret);
	}

	return 0;
}

// �ͷ��ж�
int uartIrqRelease(void)
{
	int i; 	
	for (i = 127; i <= 129; i++)
	{
		
		free_irq(OMAP_GPIO_IRQ(i), NULL);
		gpio_free(i);
	}
	
	free_irq(OMAP_GPIO_IRQ(168), NULL);
	gpio_free(168);

	return 0;
}


int uartInit(sGpmcData* myPGpmc)
{
	sGpmcRegRW cfg;
	int i;
	u32 tport;
	unsigned long arg;
	sGpmcData* pGpmc;

	tport = 0;
	arg = (unsigned long )&cfg;

	if (1)
	{
		for (i = 0; i < 4; i++)
		{
		tport = i * 0x08;
		pGpmc = pUemUartDev;
		// ����������
		// LCR BIT7 H
		cfg.offset = tport + 0x03;
	  	cfg.val.reg16 = 0x0080;
		ioctlWriteReg16(pGpmc , arg);
		
		cfg.offset = tport + 0x00;
	  	cfg.val.reg16 = 0x0001;
		ioctlWriteReg16(pGpmc , arg);

		cfg.offset = tport + 0x01;
	  	cfg.val.reg16 = 0x0000;
		ioctlWriteReg16(pGpmc , arg);

		// ��·����
		cfg.offset = tport + 0x03;
	  	cfg.val.reg16 = 0x0003;
		ioctlWriteReg16(pGpmc , arg);

		// �������ж�
		cfg.offset = tport + 0x04;
	  	cfg.val.reg16 = 0x0b;
		ioctlWriteReg16(pGpmc , arg);
		
		cfg.offset = tport + 0x01;
	  	cfg.val.reg16 = 0x01;
		ioctlWriteReg16(pGpmc , arg);

		// ����fifo
		cfg.offset = tport + 0x02;
	  	cfg.val.reg16 = (0x1<<1) | (0x1<<2);
		ioctlWriteReg16(pGpmc , arg);
		
		cfg.offset = tport + 0x02;
	  	cfg.val.reg16 = (0x1<<0) | (0x11<<6);
		ioctlWriteReg16(pGpmc , arg);
		
		
		}
	}
	else
	{
	
	}
	return 0;
}

/* ����ṹ���ַ��豸��������ĺ���
 * ��Ӧ�ó�������豸�ļ�ʱ�����õ�open��read��write�Ⱥ�����
 * ���ջ��������ṹ��ָ���Ķ�Ӧ����
 */
static struct file_operations uemUartfops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module���� */
    .open   =   uemUartOpen,
    .ioctl  =   uemUartIoctl,
    .read = uemUartRead,
    .llseek = uemUartLlseek,
    .write = uemUartWrite,
    .release = uemUartRelease,
};


/*��ʼ����ע��cdev*/
static void uemUartSetupCdev(sGpmcData *dev, int index)
{
    int err, devno = MKDEV(uemUartMajor, index);

    cdev_init(&dev->cdev, &uemUartfops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &uemUartfops;
	dev->devno = devno;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
    {
        printk(KERN_DEBUG " gpmc setup up cdev error\n ");
    }
}

/*�豸�������غ���*/
static int __init omap3530UemUartInit(void)
{
    int result = 0, i = 0, res_size=1024;
    int ret;
    dev_t devno ;
    struct clk *l3ck;
    unsigned int rate;
    unsigned int regval;
    unsigned long cs_mem_base;
    //void *pRet;

	devno = MKDEV(uemUartMajor, uemUartMinor);

    /* �����豸��*/
    if (uemUartMajor)
    {
        result = register_chrdev_region(devno, uemUartNrDevs, DEVICE_NAME);
    }
	else  /* ��̬�����豸�� */
    {
        result = alloc_chrdev_region(&devno, 0, uemUartNrDevs, DEVICE_NAME);
        uemUartMajor = MAJOR(devno);
		VaeDebug("uemUartMajor = %d\r\n",uemUartMajor);
    }
    if(result <0)
    {
        return result;
    }
	
    /* ��̬�����豸�ṹ����ڴ�*/
    pUemUartDev = kmalloc(sizeof(sGpmcData)*uemUartNrDevs, GFP_KERNEL);
    if (!pUemUartDev)    /* ����ʧ�� */
    {
    	
		VaeDebug("mem err \r\n");
       	result =  - ENOMEM;
        goto fail_malloc;
    }
    //memset(pUemUartDev, 0, sizeof(sGpmcData));
	
	uemUartSetupCdev(&pUemUartDev[i], 0);

	for (i = 0; i < UEM_UART_NR_DEVS; i++)
	{	
		uemUartSetupCdev(&pUemUartDev[i], i);
    	pUemUartDev[i].data_buf = kmalloc(UART_BUF_SIZE, GFP_KERNEL);

		// fifo ������
		spin_lock_init(&(pUemUartDev[i].fifoLock));

		// ����ʧ�ܵĲ�����δ����
		pUemUartDev[i].pFifo = kfifo_alloc(UART_FIFO_SIZE, GFP_KERNEL, &pUemUartDev[i].fifoLock);

	}

    l3ck = clk_get(NULL, "l3_ck");

    if (IS_ERR(l3ck))
    {
        rate = 100000000;
    }
    else
    {
        rate = clk_get_rate(l3ck);
    }

    pUemUartDev->gpmcCS = OMAP_GPMC_CS;
    regval = gpmc_cs_read_reg(pUemUartDev->gpmcCS , GPMC_CS_CONFIG6);
    regval |= (1<<6);
    gpmc_cs_write_reg(pUemUartDev->gpmcCS , GPMC_CS_CONFIG6 , regval);

    for(i=0;i<7;i++)
    {
        result=gpmc_cs_read_reg(pUemUartDev->gpmcCS,i*4);
        //printk(KERN_DEBUG " gpmc cs3 config reg%d for fpga %08x\n",i+1,result);
    }
    for(i=0;i<7;i++)
    {
        result=gpmc_cs_read_reg(5,i*4);
        //printk(KERN_DEBUG " gpmc cs5 config reg%d for fpga %08x\n",i+1,result);
    }

    //write config_i
    ret = gpmc_cs_request(pUemUartDev->gpmcCS, res_size, &cs_mem_base);
    if (ret < 0)
    {
        printk(KERN_ERR "Failed to request GPMC mem for fpga\n");
        VaeDebug("ret = %d\r\n", ret);
        return - ENOMEM;
    }
    //printk(KERN_ERR " GPMC mem for fpga %08lx\n",cs_mem_base);

    pUemUartDev->cs_mem_base = cs_mem_base;
    pUemUartDev->ioaddr = ioremap(cs_mem_base, res_size);
    printk(KERN_DEBUG " GPMC mem for fpga %p\n", pUemUartDev->ioaddr);
    if (pUemUartDev->ioaddr == NULL)
    {
        printk(KERN_ERR "Failed to ioremap for fpga\n");
        VaeDebug("\r\n");
        return -ENOMEM;
    }

	for (i = 1; i < uemUartNrDevs; i++)
	{
		pUemUartDev[i].ioaddr = pUemUartDev[0].ioaddr;
		pUemUartDev[i].gpmcCS = pUemUartDev[0].gpmcCS;
		pUemUartDev[i].cs_mem_base = pUemUartDev[0].cs_mem_base;
		pUemUartDev[i].dateCnt = pUemUartDev[0].dateCnt;
		pUemUartDev[i].devno= pUemUartDev[0].devno + i;
	}

	uartIrqTestInit(); 

	uartInit(NULL);
	
    return 0;
    fail_malloc:
        unregister_chrdev_region(devno, 1);
    return result;
}

/*�豸����ж�غ���*/
static void __exit omap3530UemUartExit(void)
{
	int i;
    dev_t devno ;
    //iounmap(dev->FpgaDDRVirtualAddr);

	for (i = 0; i < uemUartNrDevs; i++)
	{
		kfree(pUemUartDev[i].data_buf);
		kfifo_free(pUemUartDev[i].pFifo);
	}

    /* ж���������� */
	for (i = 0; i < uemUartNrDevs; i ++)
	{
		cdev_del(&(pUemUartDev[i].cdev));
		devno = MKDEV(uemUartMajor, i);
		unregister_chrdev_region(devno, uemUartNrDevs);
	}	
	
    gpmc_cs_free(pUemUartDev->gpmcCS);
    //release_mem_region(pUemUartDev->cs_mem_base, 1024);
    iounmap((void __iomem *)pUemUartDev->ioaddr);
    if (pUemUartDev)
    {
        kfree(pUemUartDev);
    }
	
	uartIrqRelease();
	
    VaeDebug("uem uart driver cleaned up\n");
}

module_init(omap3530UemUartInit);
module_exit(omap3530UemUartExit);

MODULE_AUTHOR("Vae");
MODULE_DESCRIPTION("uem uart driver");
MODULE_LICENSE("GPL");

