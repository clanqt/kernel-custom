cmd_kernel/events/built-in.o :=  arm-poky-linux-gnueabi-ld -EL    -r -o kernel/events/built-in.o kernel/events/core.o kernel/events/ring_buffer.o kernel/events/callchain.o 
