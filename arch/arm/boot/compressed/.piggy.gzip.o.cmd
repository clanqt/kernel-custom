cmd_arch/arm/boot/compressed/piggy.gzip.o := arm-poky-linux-gnueabi-gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem /opt/poky/1.4.4/sysroots/x86_64-pokysdk-linux/usr/lib/armv5te-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/4.7.2/include -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi -Iinclude/generated/uapi -include /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-nuc970/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float     -DZIMAGE     -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S

source_arch/arm/boot/compressed/piggy.gzip.o := arch/arm/boot/compressed/piggy.gzip.S

deps_arch/arm/boot/compressed/piggy.gzip.o := \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

arch/arm/boot/compressed/piggy.gzip.o: $(deps_arch/arm/boot/compressed/piggy.gzip.o)

$(deps_arch/arm/boot/compressed/piggy.gzip.o):
