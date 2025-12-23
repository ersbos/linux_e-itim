# GPIO Examples Makefile
# 
# Kullanım:
#   make              - Host için derle
#   make TARGET=bbb   - BeagleBone Black için cross-compile
#   make deploy       - BBB'ye kopyala
#   make clean        - Temizle

TARGET ?= host

# Programlar
PROGRAMS = led_blink button_led gpio_interrupt

# Compiler seçimi
ifeq ($(TARGET),bbb)
    CC = arm-linux-gnueabihf-gcc
    DEPLOY_HOST = debian@192.168.7.2
else
    CC = gcc
endif

CFLAGS = -Wall -O2

# Ana hedef
all: $(PROGRAMS)
	@echo "Derleme tamamlandi [$(TARGET)]"

# Programları derle
led_blink: led_blink.c
	$(CC) $(CFLAGS) -o $@ $^

button_led: button_led.c
	$(CC) $(CFLAGS) -o $@ $^

gpio_interrupt: gpio_interrupt.c
	$(CC) $(CFLAGS) -o $@ $^

# Cross-compile kısayolu
bbb:
	$(MAKE) TARGET=bbb

# BBB'ye kopyala
deploy: $(PROGRAMS)
ifndef DEPLOY_HOST
	$(error DEPLOY_HOST tanimlanmamis. BBB icin: make TARGET=bbb deploy)
endif
	scp $(PROGRAMS) $(DEPLOY_HOST):~/gpio/
	@echo "Dosyalar $(DEPLOY_HOST):~/gpio/ dizinine kopyalandi"

# Temizlik
clean:
	rm -f $(PROGRAMS)
	@echo "Temizlendi"

# Yardım
help:
	@echo "GPIO Ornekleri Makefile"
	@echo "======================="
	@echo ""
	@echo "Hedefler:"
	@echo "  make              - Host icin derle"
	@echo "  make TARGET=bbb   - BBB icin cross-compile"
	@echo "  make bbb          - BBB kisayolu"
	@echo "  make deploy       - BBB'ye kopyala (TARGET=bbb gerekli)"
	@echo "  make clean        - Temizle"
	@echo ""
	@echo "Ornekler:"
	@echo "  led_blink     - LED yakip sondurme"
	@echo "  button_led    - Button ile LED kontrol"
	@echo "  gpio_interrupt - Kesme ile button okuma"

.PHONY: all clean bbb deploy help
