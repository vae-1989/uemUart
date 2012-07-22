obj-m:=uemUart.o
uemUart-objs	:=uemUartFunc.o uemUartDriver.o 


KERNELDIR=/home/vae/study/kernel/linux-03.00.01.06/

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	cp  uemUart.ko     /home/vae/nfs/

.PHONY:clean default
clean:
	$(RM) *.o *.ko *mod* *.symvers .tmp* .*.cmd ~/nfs/uemUart.ko -rf

