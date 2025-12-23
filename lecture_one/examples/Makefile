# Lecture 1 Examples - Makefile
# 
# Kullanım:
#   make              - Host için derle
#   make TARGET=bbb   - BeagleBone Black için cross-compile
#   make TARGET=rpi   - Raspberry Pi 64-bit için cross-compile
#   make clean        - Temizle

# Default: host derleme
TARGET ?= host

# Kaynak dosyalar
SOURCES = hello_embedded.c
OBJECTS = $(SOURCES:.c=.o)
BINARY = hello

# Compiler ayarları
ifeq ($(TARGET),bbb)
    CC = arm-linux-gnueabihf-gcc
    CFLAGS = -Wall -O2 -march=armv7-a
    LDFLAGS = 
else ifeq ($(TARGET),rpi)
    CC = aarch64-linux-gnu-gcc
    CFLAGS = -Wall -O2
    LDFLAGS = 
else
    CC = gcc
    CFLAGS = -Wall -O2
    LDFLAGS = 
endif

# Default hedef
all: $(BINARY)

$(BINARY): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Derleme tamamlandı: $(BINARY) ($(TARGET))"

# Cross-compile kısayolları
bbb:
	$(MAKE) TARGET=bbb

rpi:
	$(MAKE) TARGET=rpi

# Temizlik
clean:
	rm -f $(BINARY) $(OBJECTS)
	@echo "Temizlendi"

# Yardım
help:
	@echo "Kullanım:"
	@echo "  make              - Host için derle"
	@echo "  make TARGET=bbb   - BeagleBone Black için"
	@echo "  make TARGET=rpi   - Raspberry Pi 64-bit için"
	@echo "  make bbb          - BBB kısayolu"
	@echo "  make rpi          - RPi kısayolu"
	@echo "  make clean        - Temizle"

.PHONY: all clean help bbb rpi
