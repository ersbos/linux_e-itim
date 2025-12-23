# Ders 4: Cross-Compilation ve Araç Zincirleri

> **Konu:** Farklı mimariler için derleme

---

## 📋 Bu Derste İşlenenler

- [x] Cross-compilation nedir?
- [x] ARM araç zinciri kurulumu
- [x] Ortam değişkenleri (PATH, CROSS_COMPILE, ARCH)
- [x] Makefile şablonları
- [x] Statik vs dinamik bağlama
- [x] Target'a dosya transferi

---

## 🔄 Cross-Compilation Nedir?

**Cross-compilation** = Bir sistemde derleme yapıp, başka bir sistemde çalıştırma.

```
┌─────────────────┐      ┌─────────────────┐
│   HOST SYSTEM   │      │  TARGET SYSTEM  │
│  (x86_64 Linux) │      │   (ARM Linux)   │
│                 │      │                 │
│  ┌───────────┐  │      │  ┌───────────┐  │
│  │  Source   │  │      │  │  Binary   │  │
│  │   Code    │  │  ──► │  │  (ELF)    │  │
│  │  (.c)     │  │      │  │           │  │
│  └───────────┘  │      │  └───────────┘  │
│        │        │      │        │        │
│        ▼        │      │        ▼        │
│  Cross-compiler │      │    Execution    │
│  (arm-gcc)      │      │                 │
└─────────────────┘      └─────────────────┘
```

### Neden Cross-Compile?

| Neden | Açıklama |
|-------|----------|
| **Hız** | x86_64 PC, ARM SBC'den çok daha hızlı derler |
| **Kaynaklar** | Gömülü sistemlerde RAM/disk sınırlı |
| **Araçlar** | IDE ve debug araçları host'ta daha iyi |
| **Otomasyon** | CI/CD pipeline'ları host'ta çalışır |

---

## 🛠️ ARM Araç Zinciri Kurulumu

### Debian/Ubuntu Paket Yöneticisi ile

```bash
# ARM 32-bit (BBB, eski RPi için)
sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# ARM 64-bit (RPi 4/5 için)
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Kontrol
arm-linux-gnueabihf-gcc --version
aarch64-linux-gnu-gcc --version
```

### Manuel Kurulum (Linaro/ARM)

```bash
# İndir
wget https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz

# Çıkart
tar -xf arm-gnu-toolchain-*.tar.xz -C /opt/

# PATH'e ekle (~/.bashrc)
export PATH=$PATH:/opt/arm-gnu-toolchain-*/bin
```

---

## ⚙️ Ortam Değişkenleri

### PATH
Derleyicinin bulunabilmesi için:
```bash
export PATH=$PATH:/path/to/toolchain/bin
```

### CROSS_COMPILE
Derleyici prefix'i (tire dahil!):
```bash
export CROSS_COMPILE=arm-linux-gnueabihf-
```

### ARCH
Hedef mimari:
```bash
export ARCH=arm      # 32-bit ARM
export ARCH=arm64    # 64-bit ARM
```

### Örnek ~/.bashrc

```bash
# ARM Cross-Compile Environment
export CROSS_COMPILE_ARM32=arm-linux-gnueabihf-
export CROSS_COMPILE_ARM64=aarch64-linux-gnu-

# Kısayollar
alias arm32-gcc='arm-linux-gnueabihf-gcc'
alias arm64-gcc='aarch64-linux-gnu-gcc'
```

---

## 💻 Cross-Compile Örneği

### Basit Program

```c
/* hello.c */
#include <stdio.h>

int main(void)
{
    printf("Merhaba ARM!\n");
    return 0;
}
```

### Derleme Komutları

```bash
# Host için (normal)
gcc -o hello hello.c

# ARM 32-bit için (BBB)
arm-linux-gnueabihf-gcc -o hello-arm32 hello.c

# ARM 64-bit için (RPi)
aarch64-linux-gnu-gcc -o hello-arm64 hello.c

# Dosya tipini kontrol et
file hello-arm32
# ELF 32-bit LSB executable, ARM, EABI5...

file hello-arm64
# ELF 64-bit LSB executable, ARM aarch64...
```

---

## 📦 Statik vs Dinamik Bağlama

### Dinamik Bağlama (Default)
- Daha küçük binary
- Çalışma zamanında kütüphane gerektirir
- Target'ta aynı kütüphaneler olmalı

```bash
arm-linux-gnueabihf-gcc -o hello hello.c
# Binary boyutu: ~8KB
```

### Statik Bağlama
- Daha büyük binary
- Tüm kütüphaneler dahil
- Bağımlılık yok, her yerde çalışır

```bash
arm-linux-gnueabihf-gcc -static -o hello-static hello.c
# Binary boyutu: ~700KB
```

### Ne Zaman Hangisi?

| Durum | Tercih |
|-------|--------|
| Normal uygulama | Dinamik |
| Minimal sistem (BusyBox) | Statik |
| Farklı dağıtımlar arası | Statik |
| Debug/test | Statik |

---

## 🔧 Cross-Compile Makefile

```makefile
# Cross-compile Makefile şablonu

# Hedef platform seçimi
TARGET ?= host

# Compiler ayarları
ifeq ($(TARGET),bbb)
    # BeagleBone Black (ARM 32-bit)
    CC = arm-linux-gnueabihf-gcc
    CFLAGS = -Wall -O2 -march=armv7-a
    LDFLAGS =
else ifeq ($(TARGET),rpi)
    # Raspberry Pi 4/5 (ARM 64-bit)
    CC = aarch64-linux-gnu-gcc
    CFLAGS = -Wall -O2
    LDFLAGS =
else
    # Host (native)
    CC = gcc
    CFLAGS = -Wall -O2
    LDFLAGS =
endif

# Kaynak ve hedef
SOURCES = main.c utils.c
TARGET_BIN = myapp

all: $(TARGET_BIN)
	@echo "Derlendi: $(TARGET_BIN) [$(TARGET)]"

$(TARGET_BIN): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Kısayollar
bbb:
	$(MAKE) TARGET=bbb

rpi:
	$(MAKE) TARGET=rpi

# Statik derleme
static:
	$(MAKE) LDFLAGS=-static

clean:
	rm -f $(TARGET_BIN)

.PHONY: all clean bbb rpi static
```

### Kullanım

```bash
make              # Host için
make TARGET=bbb   # BBB için
make TARGET=rpi   # RPi için
make bbb         # Kısayol
make static      # Statik bağlama
```

---

## 📤 Target'a Transfer

### SCP ile

```bash
# Tek dosya
scp myapp debian@192.168.7.2:~/

# Dizin
scp -r project/ debian@192.168.7.2:~/
```

### rsync ile (Daha Hızlı)

```bash
# Sadece değişenleri kopyala
rsync -avz --progress myapp debian@192.168.7.2:~/
```

### NFS ile (Geliştirme için)

```bash
# Host'ta NFS paylaşımı yap
# Target'ta mount et
mount -t nfs host_ip:/path /mnt/nfs
```

---

## 🐛 Hata Ayıklama

### Binary Kontrolü

```bash
# Dosya tipi
file myapp

# Bağımlılıklar (dinamik)
arm-linux-gnueabihf-ldd myapp

# Semboller
arm-linux-gnueabihf-nm myapp

# Disassembly
arm-linux-gnueabihf-objdump -d myapp | head -50
```

### Yaygın Hatalar

| Hata | Çözüm |
|------|-------|
| "cannot execute binary" | Yanlış mimari, file ile kontrol et |
| "libXXX not found" | Statik derle veya kütüphaneyi kopyala |
| "No such file or directory" | Linker path sorunu, statik derle |

---

## 🎯 Pratik

1. "Hello ARM" programını cross-compile et
2. BBB veya RPi'a SCP ile kopyala
3. Çalıştır ve çıktıyı gör
4. Statik ve dinamik derlemelerin boyutlarını karşılaştır
5. Makefile ile TARGET değişkeni kullanarak derle

---

*Sonraki ders: Linux Kernel Basics*
