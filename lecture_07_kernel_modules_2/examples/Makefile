# Makefile for Lecture 7 Kernel Module Examples
# Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)

obj-m += simple_char.o
obj-m += ioctl_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Cross-compile için (BBB)
# ARCH := arm
# CROSS_COMPILE := arm-linux-gnueabihf-
# KDIR := /path/to/bbb/kernel

all: modules userspace

modules:
	@echo "=== Kernel modulleri derleniyor ==="
	make -C $(KDIR) M=$(PWD) modules

userspace:
	@echo "=== User-space programlari derleniyor ==="
	gcc -Wall -o ioctl_test ioctl_test.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f ioctl_test

# === Yardımcı hedefler ===

load-simple:
	@echo "=== simple_char.ko yukleniyor ==="
	sudo insmod simple_char.ko
	@sleep 1
	ls -la /dev/simplechar

unload-simple:
	@echo "=== simple_char.ko kaldiriliyor ==="
	sudo rmmod simple_char

test-simple: load-simple
	@echo "=== simple_char test ==="
	echo "Merhaba Kernel" | sudo tee /dev/simplechar
	sudo cat /dev/simplechar
	@make unload-simple

load-ioctl:
	@echo "=== ioctl_driver.ko yukleniyor ==="
	sudo insmod ioctl_driver.ko
	@sleep 1
	ls -la /dev/ioctldev

unload-ioctl:
	@echo "=== ioctl_driver.ko kaldiriliyor ==="
	sudo rmmod ioctl_driver

test-ioctl: load-ioctl userspace
	@echo "=== ioctl_driver test ==="
	sudo ./ioctl_test
	@make unload-ioctl

log:
	dmesg | tail -20

info:
	@echo "=== Modul bilgileri ==="
	@if [ -f simple_char.ko ]; then modinfo simple_char.ko; fi
	@if [ -f ioctl_driver.ko ]; then modinfo ioctl_driver.ko; fi

.PHONY: all modules userspace clean load-simple unload-simple test-simple \
        load-ioctl unload-ioctl test-ioctl log info
