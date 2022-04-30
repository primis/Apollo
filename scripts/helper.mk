include $(TOP_DIR)/.config
include Makefile

CUR_DIR		:= $(shell pwd)
CUR_DIR		:= $(patsubst $(TOP_DIR)/%,./%, $(CUR_DIR))

ifeq ($(CONFIG_TESTING),y)
	obj_y 	+= $(test_y) 
endif

obj_y		:= $(patsubst %.c,$(CUR_DIR)/%.c.o,$(obj_y))
obj_y		:= $(patsubst %.s,$(CUR_DIR)/%.s.o,$(obj_y))

all:
	@echo $(obj_y)