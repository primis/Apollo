#!/bin/bash

set -e
rpi_url=https://github.com/raspberrypi/firmware/blob/master/boot
rpi_boot_files=("start4.elf" "bootcode.bin" "fixup4.dat" "bcm2711-rpi-4-b.dtb")

mkdir -p isodir

cp $1 isodir/kernel8.img

# Grab Raspberry Pi 4 required boot files
for FILE in ${rpi_boot_files[@]}; do
  if [ ! -f isodir/$FILE ]; then
    wget $rpi_url/$FILE\?raw=true -O isodir/$FILE
  else
    echo "Cached $FILE"
  fi
done

cat > isodir/config.txt << EOF
enable_uart=1
core_freq_min=500
arm_64bit=1
EOF

cat > isodir/cmdline.txt << EOF
debug=y
EOF

# Create a 128 Megabyte iso file
dd if=/dev/zero of=apollo.iso bs=1M count=128
# Format it as FAT
mformat -i apollo.iso -v Apollo ::
# Copy all the files in isodir to the iso
for FILE in isodir/*; do
  mcopy -i apollo.iso $FILE ::
done
