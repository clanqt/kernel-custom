cmd_drivers/video/logo/logo_linux_clut224.o := arm-poky-linux-gnueabi-gcc -Wp,-MD,drivers/video/logo/.logo_linux_clut224.o.d  -nostdinc -isystem /opt/poky/1.4.4/sysroots/x86_64-pokysdk-linux/usr/lib/armv5te-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/4.7.2/include -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi -Iinclude/generated/uapi -include /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-nuc970/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -std=gnu89 -Os -Wno-maybe-uninitialized -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -marm -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -Wframe-larger-than=2048 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(logo_linux_clut224)"  -D"KBUILD_MODNAME=KBUILD_STR(logo_linux_clut224)" -c -o drivers/video/logo/logo_linux_clut224.o drivers/video/logo/logo_linux_clut224.c

source_drivers/video/logo/logo_linux_clut224.o := drivers/video/logo/logo_linux_clut224.c

deps_drivers/video/logo/logo_linux_clut224.o := \
  include/linux/linux_logo.h \
    $(wildcard include/config/fb/logo/extra.h) \
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/modules.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/arm/include/generated/asm/types.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/arch/arm/include/uapi/asm/posix_types.h \
  /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/include/uapi/asm-generic/posix_types.h \

drivers/video/logo/logo_linux_clut224.o: $(deps_drivers/video/logo/logo_linux_clut224.o)

$(deps_drivers/video/logo/logo_linux_clut224.o):
