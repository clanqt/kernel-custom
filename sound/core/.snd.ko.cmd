cmd_sound/core/snd.ko := arm-poky-linux-gnueabi-ld -EL -r  -T /home/elinux/Gokul_Project/7510BuildSystem-TNSTC-NewAPI/Kernel/scripts/module-common.lds --build-id  -o sound/core/snd.ko sound/core/snd.o sound/core/snd.mod.o