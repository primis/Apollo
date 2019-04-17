#!/bin/sh

# (c) 2019 Apollo Developers
# For terms, see LICENSE
# Script for updating floppy.img
# Should work on linux
# Must be run as root for mount

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root"
    exit 1
fi

echo '> Mounting Floppy Image'
mkdir -p /var/tmp-image
losetup /dev/loop0 floppy.img
mount /dev/loop0 /var/tmp-image
echo '> Cleaning kernel folder'
rm /var/tmp-image/Apollo/*.mod
echo '> Copying kernel modules'
cp *.mod /var/tmp-image/Apollo/
echo '> Unmounting Floppy Image'
umount /dev/loop0
losetup -d /dev/loop0
rm -rf /var/tmp-image


