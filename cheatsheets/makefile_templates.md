# Makefile Şablonları

## 1. Basit Host Derleme

```makefile
# Basit Makefile
CC = gcc
CFLAGS = -Wall -O2
TARGET = program

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
```

---

## 2. Cross-Compilation Makefile

```makefile
# Cross-compile destekli Makefile
# Kullanım: make TARGET=bbb veya make TARGET=rpi

TARGET_PLATFORM ?= host

# Compiler seçimi
ifeq ($(TARGET_PLATFORM),bbb)
    CC = arm-linux-gnueabihf-gcc
    ARCH = arm
else ifeq ($(TARGET_PLATFORM),rpi)
    CC = aarch64-linux-gnu-gcc
    ARCH = arm64
else
    CC = gcc
    ARCH = native
endif

CFLAGS = -Wall -O2
TARGET = program
SOURCES = main.c utils.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)
	@echo "Derlendi: $(TARGET) [$(TARGET_PLATFORM)]"

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Kısayollar
bbb:
	$(MAKE) TARGET_PLATFORM=bbb

rpi:
	$(MAKE) TARGET_PLATFORM=rpi

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean bbb rpi
```

---

## 3. Kernel Modül Makefile (Host)

```makefile
# Kernel modül derleme - Host sistem için
obj-m += mymodule.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

# Yükleme/kaldırma
load:
	sudo insmod mymodule.ko

unload:
	sudo rmmod mymodule

.PHONY: all clean load unload
```

---

## 4. Kernel Modül Makefile (Cross-Compile - BBB)

```makefile
# Kernel modül derleme - BBB için cross-compile
CROSS_COMPILE = arm-linux-gnueabihf-
ARCH = arm

# BBB kernel kaynak dizini (güncelleyin!)
KDIR := /path/to/bbb/linux-source

PWD := $(shell pwd)

obj-m += mymodule.o

all:
	make -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	make -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

# BBB'ye kopyala
deploy:
	scp mymodule.ko debian@192.168.7.2:~/

.PHONY: all clean deploy
```

---

## 5. Çoklu Kaynak Dosyalı Proje

```makefile
# Çoklu dosyalı proje
CC = gcc
CFLAGS = -Wall -O2 -I./include
LDFLAGS = -lpthread

TARGET = myapp
SRCDIR = src
OBJDIR = obj
INCDIR = include

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
```

---

## 6. Debug ve Release Modları

```makefile
# Debug ve Release destekli Makefile
CC = gcc
TARGET = program
SOURCES = main.c

# Build modu (debug veya release)
BUILD ?= debug

ifeq ($(BUILD),debug)
    CFLAGS = -Wall -g -O0 -DDEBUG
else
    CFLAGS = -Wall -O2 -DNDEBUG
endif

all: $(TARGET)
	@echo "Build mode: $(BUILD)"

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	rm -f $(TARGET)

.PHONY: all clean debug release
```

---

## 7. Sysfs GPIO Programı Makefile

```makefile
# GPIO programları için Makefile
CC = gcc
CFLAGS = -Wall -O2

# Hedef platform
TARGET_PLATFORM ?= host

ifeq ($(TARGET_PLATFORM),bbb)
    CC = arm-linux-gnueabihf-gcc
endif

TARGETS = led_blink button_read gpio_interrupt

all: $(TARGETS)

led_blink: led_blink.c
	$(CC) $(CFLAGS) -o $@ $^

button_read: button_read.c
	$(CC) $(CFLAGS) -o $@ $^

gpio_interrupt: gpio_interrupt.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

# BBB'ye kopyala
deploy: $(TARGETS)
	scp $(TARGETS) debian@192.168.7.2:~/

.PHONY: all clean deploy
```

---

## Makefile Değişken Referansı

| Değişken | Açıklama | Örnek |
|----------|----------|-------|
| `$@` | Hedef ismi | `program` |
| `$^` | Tüm bağımlılıklar | `main.o utils.o` |
| `$<` | İlk bağımlılık | `main.c` |
| `$*` | Kök isim (uzantısız) | `main` |

## Yaygın CFLAGS

| Flag | Açıklama |
|------|----------|
| `-Wall` | Tüm uyarılar |
| `-Werror` | Uyarıları hata yap |
| `-g` | Debug bilgisi |
| `-O0` | Optimizasyon yok |
| `-O2` | Orta optimizasyon |
| `-O3` | Agresif optimizasyon |
| `-I./include` | Include dizini ekle |
| `-DDEBUG` | DEBUG makro tanımla |

---

*Son güncelleme: 2025-12-23*
