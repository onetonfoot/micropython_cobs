# Location of top-level MicroPython directory
MPY_DIR = /opt/micropython

# Name of module
MOD = cobs

# Source files (.c or .py)
SRC = src/cobs.c

# Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
ARCH = armv6m

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk
# include $(MPY_DIR)/py/py.mk

CFLAGS += -Wno-unused-variable