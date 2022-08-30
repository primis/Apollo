# Hosted Makefile for doing testing on hosted systems

STRIP	:= $(shell which strip)
TARGET_DEFS	:= -DHOSTED=1

# That's it!
