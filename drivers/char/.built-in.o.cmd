cmd_drivers/char/built-in.o :=  arm-poky-linux-gnueabi-ld -EL    -r -o drivers/char/built-in.o drivers/char/mem.o drivers/char/random.o drivers/char/misc.o drivers/char/nuc970_mem.o drivers/char/nuc970_dma_proxy.o 