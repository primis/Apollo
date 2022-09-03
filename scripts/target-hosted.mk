# Hosted Makefile for doing testing on hosted systems

STRIP	:= $(shell which strip)
TARGET_DEFS	:= -DHOSTED=1

$(BUILD)/%.iso : $(BUILD)/apollo/$(BIN)
@printf "Dummy ISO Creation\n"
@touch $@

# That's it!
