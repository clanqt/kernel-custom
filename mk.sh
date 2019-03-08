source /home/elinux/nuvoton-cc

mkimage -A arm -O linux -C none -T kernel -a 0x7fc0 -e 0x8000 -n "Linux kernel" -d arch/arm/boot/zImage uImage

cp uImage ../SD_Flasher/fs/usr/kernel/uImage 

rm ../Filesystem/MATCHBOX_14112016/lib/modules/* -rf
make modules_install INSTALL_MOD_PATH=../Filesystem/MATCHBOX_14112016/
mv ../Filesystem/MATCHBOX_14112016/lib/modules/3.10.108/kernel/drivers/misc/nxp-pn5xx/pn5xx_i2c.ko ../Filesystem/MATCHBOX_14112016/opt/daemon_files/

cd printer_testing/
make clean
make
cp $PWD/printer.ko ../../Filesystem/MATCHBOX_14112016/opt/daemon_files/
cd ..

cd wifi_mt7601u/
make clean
make
cp $PWD/os/linux/mt7601Usta.ko ../../Filesystem/MATCHBOX_14112016/opt/daemon_files/
cp $PWD/os/linux/mt7601Usta.ko ../../Filesystem/MATCHBOX_14112016/opt/os/
cp $PWD/RT2870STA.dat ../../Filesystem/MATCHBOX_14112016/etc/Wireless/RT2870STA/
cd ..

cd backlightctl/
make clean
make
cp $PWD/backlightctl.ko ../../Filesystem/MATCHBOX_14112016/opt/daemon_files/
cd ..
