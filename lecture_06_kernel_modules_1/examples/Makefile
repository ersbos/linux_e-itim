# Kernel Module Makefile
# 
# Kullanım:
#   make                      - Tüm modülleri derle
#   make file=helloworld      - Tek modül derle
#   make load file=helloworld - Modülü yükle
#   make unload file=helloworld - Modülü kaldır
#   make clean                - Temizle

# Varsayılan: tüm modüller
ifneq ($(file),)
    obj-m := $(file).o
else
    obj-m := helloworld.o params_module.o
endif

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Ana hedef
all:
	make -C $(KDIR) M=$(PWD) modules
	@echo "Derleme tamamlandi!"

# Temizlik
clean:
	make -C $(KDIR) M=$(PWD) clean

# Modül yükleme
load:
ifndef file
	$(error Kullanim: make load file=modul_adi)
endif
	sudo insmod $(file).ko
	@echo "$(file).ko yuklendi"
	@dmesg | tail -5

# Modül kaldırma
unload:
ifndef file
	$(error Kullanim: make unload file=modul_adi)
endif
	sudo rmmod $(file)
	@echo "$(file) kaldirildi"
	@dmesg | tail -5

# Log görüntüleme
log:
	dmesg | tail -20

# Modül bilgisi
info:
ifndef file
	$(error Kullanim: make info file=modul_adi)
endif
	modinfo $(file).ko

# Yüklü modülleri listele
list:
	lsmod | grep -E "^Module|hello|params"

# Yardım
help:
	@echo "Kullanim:"
	@echo "  make                        - Tum modulleri derle"
	@echo "  make file=NAME              - Tek modul derle"
	@echo "  make load file=NAME         - Modulu yukle"
	@echo "  make unload file=NAME       - Modulu kaldir"
	@echo "  make info file=NAME         - Modul bilgisi"
	@echo "  make log                    - Kernel loglarini goster"
	@echo "  make list                   - Yuklu modulleri listele"
	@echo "  make clean                  - Temizle"

.PHONY: all clean load unload log info list help
