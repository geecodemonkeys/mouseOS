#!/bin/bash
nasm -f bin boot.asm -o BOOT.SYS
mkdir cd_root
cp boot.bin cd_root/BOOT.SYS
genisoimage -o ./os.iso -b BOOT.SYS -no-emul-boot ./cd_root
