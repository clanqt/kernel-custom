cmd_arch/arm/lib/io-writesb.o := arm-poky-linux-gnueabi-gcc -Wp,-MD,arch/arm/lib/.io-writesb.o.d  -nostdinc -isystem /opt/poky/1.4.4/sysroots/x86_64-pokysdk-linux/usr/lib/armv5te-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/4.7.2/include -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi -Iinclude/generated/uapi -include /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-nuc970/include  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float         -c -o arch/arm/lib/io-writesb.o arch/arm/lib/io-writesb.S

source_arch/arm/lib/io-writesb.o := arch/arm/lib/io-writesb.S

deps_arch/arm/lib/io-writesb.o := \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/linkage.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/hwcap.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi/asm/hwcap.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/opcodes-virt.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \

arch/arm/lib/io-writesb.o: $(deps_arch/arm/lib/io-writesb.o)

$(deps_arch/arm/lib/io-writesb.o):
