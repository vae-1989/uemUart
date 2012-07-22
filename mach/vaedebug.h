#ifndef __VAE_DEBUG__
#define __VAE_DEBUG__

#if 1
#define VaeInfo(fmt,args...) \
    printk(KERN_DEBUG "%s\b\b\r\n"fmt, __FILE__, ## args)
	
#else
#define VaeInfo(fmt,args...)  

#endif

#define VaeFuncInfo()	\
    VaeInfo("Func:%s, line:%d, Time:%s, File:%s\r\n", __FUNCTION__, __LINE__, __TIME__, __FILE__)

#define dFuncLine() \
    VaeInfo("Func:%s, Line:%d\r\n", __FUNCTION__, __LINE__);


#define VaeDebug(fmt,args...)	\
    printk(KERN_DEBUG"Func:%s, Line:%d\r\n", __FUNCTION__, __LINE__);\
    printk(KERN_DEBUG fmt,## args)
//#define vaeDebug(fmt,args...)

#define fpgaDebugLevel 7
#define fpgaInfoLevel 6

#define VaePrint(Level,fmt, args...)\
	printk("<%d>fpgaDriver: "fmt,Level,## args)


#endif
