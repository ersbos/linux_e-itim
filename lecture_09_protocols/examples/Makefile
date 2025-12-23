# Makefile for Lecture 9 Protocol Examples
# Ders 9: İletişim Protokolleri (UART, I2C, SPI)

CC = gcc
CFLAGS = -Wall -Wextra -O2
CROSS_CC = arm-linux-gnueabihf-gcc

TARGETS = uart_example i2c_example spi_example
TARGET_IP = 192.168.7.2
TARGET_USER = root

.PHONY: all clean host arm deploy

all: host

host: $(TARGETS)

uart_example: uart_example.c
	$(CC) $(CFLAGS) -o $@ $<

i2c_example: i2c_example.c
	$(CC) $(CFLAGS) -o $@ $<

spi_example: spi_example.c
	$(CC) $(CFLAGS) -o $@ $<

# Cross-compile for ARM (BBB/RPi)
arm:
	$(CROSS_CC) $(CFLAGS) -o uart_example_arm uart_example.c
	$(CROSS_CC) $(CFLAGS) -o i2c_example_arm i2c_example.c
	$(CROSS_CC) $(CFLAGS) -o spi_example_arm spi_example.c

clean:
	rm -f $(TARGETS) *_arm

# Deploy to target
deploy: arm
	scp *_arm $(TARGET_USER)@$(TARGET_IP):~/

# === Test hedefleri ===

test-uart:
	@echo "UART testi icin USB-UART donusturucu bagli olmali"
	@echo "Ornek: sudo ./uart_example /dev/ttyUSB0 115200"

test-i2c:
	@echo "I2C cihaz taramasi"
	i2cdetect -y 1

test-spi:
	@echo "SPI loopback testi icin MOSI-MISO bağlı olmali"
	@echo "Ornek: sudo ./spi_example /dev/spidev0.0"

help:
	@echo "Kullanim:"
	@echo "  make          - Host icin derle"
	@echo "  make arm      - ARM icin cross-compile"
	@echo "  make deploy   - Target'a kopyala"
	@echo "  make clean    - Temizle"
	@echo ""
	@echo "Test komutlari:"
	@echo "  make test-uart"
	@echo "  make test-i2c"
	@echo "  make test-spi"
