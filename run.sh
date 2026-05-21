#!/bin/bash


cp dl_test ../initramfs/
cp dl_test ../initramfs/bin/
cd ../initramfs/
find . -print0 | cpio --null -ov --format=newc | gzip -9 > initramfs.cpio.gz
cd ../linux-6.1.90/
qemu-system-x86_64   -kernel arch/x86/boot/bzImage   -initrd ../initramfs/initramfs.cpio.gz   -append "console=ttyS0 rdinit=/init"   -nographic -smp 4 -enable-kvm