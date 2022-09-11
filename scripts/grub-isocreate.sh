#!/bin/sh

set -e

mkdir -p isodir/boot/grub

cp $1 isodir/boot/kernel.mod

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "Apollo" {
  multiboot /boot/kernel.mod
}

menuentry "Apollo Debugging" {
  multiboot /boot/kernel.mod debug=y
}
EOF

grub-mkrescue -o apollo.iso isodir
