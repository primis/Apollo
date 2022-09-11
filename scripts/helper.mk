include $(TOP_DIR)/.config
include Makefile

CUR_DIR		:= $(shell pwd)
CUR_DIR		:= $(patsubst $(TOP_DIR)/%,./%, $(CUR_DIR))

ifeq ($(CONFIG_TESTING),y)
	obj_y 	+= $(test_y) 
endif

obj_y		:= $(patsubst %,$(CUR_DIR)/%.o,$(obj_y))

all:
	@echo $(obj_y)
