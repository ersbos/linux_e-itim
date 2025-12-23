# Cross-Compilation Examples Makefile
# 
# Kullanım:
#   make              - Host için derle
#   make TARGET=bbb   - BeagleBone Black için
#   make TARGET=rpi   - Raspberry Pi 64-bit için
#   make static       - Statik bağlama
#   make deploy       - Target'a kopyala
#   make clean        - Temizle

# Platform seçimi
TARGET ?= host

# Compiler ve flag'ler
ifeq ($(TARGET),bbb)
    CC = arm-linux-gnueabihf-gcc
    CFLAGS = -Wall -O2 -march=armv7-a
    DEPLOY_HOST = debian@192.168.7.2
    DEPLOY_PATH = ~/
else ifeq ($(TARGET),rpi)
    CC = aarch64-linux-gnu-gcc
    CFLAGS = -Wall -O2
    DEPLOY_HOST = pi@raspberrypi.local
    DEPLOY_PATH = ~/
else
    CC = gcc
    CFLAGS = -Wall -O2
endif

LDFLAGS ?=

# Programlar
SOURCES = hello_arm.c
TARGET_BIN = hello_arm

# Ana hedef
all: $(TARGET_BIN)
	@echo "========================================"
	@echo "Derleme tamamlandi!"
	@echo "Target: $(TARGET)"
	@echo "Binary: $(TARGET_BIN)"
	@echo "========================================"
	@file $(TARGET_BIN)

$(TARGET_BIN): $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Platform kısayolları
bbb:
	$(MAKE) TARGET=bbb

rpi:
	$(MAKE) TARGET=rpi

# Statik derleme
static:
	$(MAKE) LDFLAGS=-static

static-bbb:
	$(MAKE) TARGET=bbb LDFLAGS=-static

static-rpi:
	$(MAKE) TARGET=rpi LDFLAGS=-static

# Target'a deploy
deploy: $(TARGET_BIN)
ifndef DEPLOY_HOST
	$(error Platform secilmedi. Kullanim: make TARGET=bbb deploy)
endif
	scp $(TARGET_BIN) $(DEPLOY_HOST):$(DEPLOY_PATH)
	@echo "$(TARGET_BIN) -> $(DEPLOY_HOST):$(DEPLOY_PATH)"

# Binary bilgisi
info: $(TARGET_BIN)
	@echo "=== Binary Info ==="
	file $(TARGET_BIN)
	@echo ""
	@echo "=== Size ==="
	ls -lh $(TARGET_BIN)
	@echo ""
	@echo "=== Sections ==="
	size $(TARGET_BIN) 2>/dev/null || true

# Temizlik
clean:
	rm -f $(TARGET_BIN) *.o
	@echo "Temizlendi"

# Yardım
help:
	@echo "Cross-Compile Makefile"
	@echo "====================="
	@echo ""
	@echo "Hedefler:"
	@echo "  make              - Host icin derle"
	@echo "  make TARGET=bbb   - BBB icin cross-compile"
	@echo "  make TARGET=rpi   - RPi icin cross-compile"
	@echo "  make bbb          - BBB kisayolu"
	@echo "  make rpi          - RPi kisayolu"
	@echo "  make static       - Statik baglama"
	@echo "  make static-bbb   - BBB icin statik"
	@echo "  make deploy       - Target'a kopyala"
	@echo "  make info         - Binary bilgisi"
	@echo "  make clean        - Temizle"

.PHONY: all clean bbb rpi static static-bbb static-rpi deploy info help
