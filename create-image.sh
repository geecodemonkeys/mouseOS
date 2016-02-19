#!/bin/bash
nasm -f bin boot.asm -o boot.bin
mkdir cd_root
cp boot.bin cd_root/boot.bin
genisoimage -o ./os.iso -b boot.bin -no-emul-boot ./cd_root
