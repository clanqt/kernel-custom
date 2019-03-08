cmd_fs/cifs/cifs.ko := arm-poky-linux-gnueabi-ld -EL -r  -T /home/elinux/7510BuildSystem/Kernel/scripts/module-common.lds --build-id  -o fs/cifs/cifs.ko fs/cifs/cifs.o fs/cifs/cifs.mod.o
