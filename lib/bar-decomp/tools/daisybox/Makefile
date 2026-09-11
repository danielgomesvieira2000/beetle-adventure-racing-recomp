CC := gcc
CFLAGS := -I include -ggdb3 -O2 -Wall -Wextra -std=c11 -Wno-multichar -DLOG_USE_COLOR
CPPFLAGS := -I.
LDFLAGS :=
BUILD_DIR := build

TARGET := daisybox
SRC_DIRS := $(shell find src -type d)
C_FILES  := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES  := $(foreach f,$(C_FILES:.c=.o),$(BUILD_DIR)/$f) 

$(shell mkdir -p $(foreach dir,$(SRC_DIRS),$(BUILD_DIR)/$(dir)))

LIBDIR := libs
LIBS := \
	$(LIBDIR)/libbfd.a \
	$(LIBDIR)/libopcodes.a \
	$(LIBDIR)/libsframe.a \
	$(LIBDIR)/libiberty.a \
	$(LIBDIR)/libz.a \
	-lzstd -lm -ldl -lcjson

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(O_FILES)
	$(CC) $(LDFLAGS) -o $@ $(O_FILES) $(LIBS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

genForm0:
	$(CC) $(CFLAGS) -DSTB_LOG genForm0.c $(LIBS)  -o genForm0

dependencies:
	sh resolve_deps.sh

clean:
	rm -fr $(BUILD_DIR)
