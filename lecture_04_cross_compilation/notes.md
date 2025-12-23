# Ders 4: Cross-Compilation ve Araç Zincirleri

> **Konu:** Farklı Mimariler için Derleme, Araç Zinciri Kavramları
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Araç zinciri (toolchain) kavramı
- [x] Araç zinciri bileşenleri
- [x] Doğal vs çapraz araç zincirleri
- [x] Araç zinciri isimlendirme kuralları
- [x] Araç zinciri edinme yolları
- [x] ARM araç zincirleri (ARM, Linaro, Bootlin)
- [x] Ortam değişkenleri (PATH, CROSS_COMPILE, ARCH)
- [x] Cross-compile işlemi
- [x] Statik vs dinamik bağlama
- [x] Target'a dosya transferi (scp, sshfs, NFS)

---

## 🔧 Araç Zinciri (Toolchain) Nedir?

Gömülü Linux sistemlerinde C/C++ ile yazılım geliştirmek için kullanılan araçlardan oluşan topluluğa **araç zinciri (toolchain)** denilmektedir. Araç zinciri kavramı, aşağı seviyeli programlama dillerinde (C, C++, Assembly) yazılım geliştirmek için gerekli olan tüm araçları kapsar.

Bir araç zincirindeki araçlar birbirleriyle entegre çalışır ve bir zincir gibi birbirini takip eder: Kaynak kod önce derleyiciden geçer, sonra assembler tarafından işlenir, ardından linker tarafından birleştirilir ve son olarak çalıştırılabilir dosya oluşturulur. Bu süreçte her araç bir öncekinin çıktısını alır ve işler.

### Araç Zinciri Bileşenleri

Tipik bir araç zinciri aşağıdaki bileşenlerden oluşur:

**C ve C++ Derleyicileri (Compilers):** Kaynak kodunu makine diline dönüştüren temel araçlardır. GCC (GNU Compiler Collection) en yaygın kullanılan derleyicidir. Derleyici, `.c` veya `.cpp` dosyalarını alır ve `.o` (object) dosyaları üretir.

**Sembolik Makine Dili Derleyicileri (Assemblers):** Assembly dilinde yazılmış kodları makine koduna dönüştürür. GCC araç zincirinde bu araç `as` olarak adlandırılır.

**Bağlayıcılar (Linkers):** Object dosyalarını ve kütüphaneleri birleştirerek çalıştırılabilir dosya oluşturur. GCC araç zincirinde `ld` (linker) bu görevi üstlenir.

**Binary Araçlar (Binutils):** Çeşitli aşağı seviyeli işlemler için kullanılan yardımcı programlardır:
- `objdump`: Object dosyalarını analiz eder
- `readelf`: ELF dosya formatını inceler
- `nm`: Sembolleri listeler
- `strip`: Debug bilgilerini temizler
- `ar`: Statik kütüphaneler oluşturur

**Debugger'lar:** Hataların tespit edilmesi ve çözümlenmesi için kullanılır. GDB (GNU Debugger) en yaygın kullanılan debugger'dır.

**Temel Kütüphaneler:** Standart C kütüphanesi (glibc, musl, uClibc), POSIX kütüphanesi ve diğer sistem kütüphaneleri.

**Sysroot Dizini:** Hedef sistem için gerekli olan başlık dosyaları ve kütüphanelerin bulunduğu dizin yapısı.

> [!IMPORTANT]
> Araç zinciri kavramı yalnızca C/C++ gibi aşağı seviyeli dilleri kapsar. Java, Python, .NET gibi yüksek seviyeli dillerin araçları bu kavrama dahil değildir.

---

## 🔄 Cross-Compilation Nedir?

Cross-compilation (çapraz derleme), bir programın **bir sistemde derlenerek başka bir sistemde çalıştırılması** anlamına gelir. Derlemenin yapıldığı sisteme **host** (ana bilgisayar), programın çalıştırılacağı sisteme **target** (hedef) denir.

Örneğin, Intel x86_64 işlemcili bir PC'de BeagleBone Black için (ARM Cortex-A8) kod derlemek bir cross-compilation işlemidir. Derlenen program x86 bilgisayarda çalışmaz, yalnızca ARM sistemde çalışır.

```
┌─────────────────────────────────────┐      ┌─────────────────────────────────────┐
│           HOST SYSTEM               │      │          TARGET SYSTEM               │
│       (x86_64 Linux PC)             │      │       (ARM Linux SBC)                │
│                                     │      │                                      │
│   ┌─────────────────────────────┐   │      │   ┌─────────────────────────────┐   │
│   │      Kaynak Kod (.c)        │   │      │   │    Çalıştırılabilir (ELF)   │   │
│   │      main.c, utils.c        │   │      │   │       ARM Binary            │   │
│   └──────────────┬──────────────┘   │      │   └──────────────▲──────────────┘   │
│                  │                   │      │                  │                   │
│                  ▼                   │      │                  │                   │
│   ┌─────────────────────────────┐   │      │                  │                   │
│   │    Cross-Compiler           │   │ ───► │      SCP/NFS     │                   │
│   │  arm-linux-gnueabihf-gcc    │   │      │      Transfer    │                   │
│   └─────────────────────────────┘   │      │                  │                   │
│                                     │      │   ┌──────────────┴──────────────┐   │
│   Derleme hızı: ÇOK YÜKSEK         │      │   │    ./program                 │   │
│   RAM/Disk: BOL                     │      │   │    Execution                 │   │
│   IDE/Araçlar: TAM                  │      │   └─────────────────────────────┘   │
└─────────────────────────────────────┘      └─────────────────────────────────────┘
```

### Neden Cross-Compile?

Gömülü sistemlerde cross-compilation tercih edilmesinin birçok önemli nedeni vardır:

**Performans:** Masaüstü bilgisayarlar, gömülü sistemlere kıyasla çok daha güçlüdür. Bir x86_64 PC, ARM tabanlı bir SBC'den 10-50 kat daha hızlı derleme yapabilir. Linux kernel'ını Raspberry Pi üzerinde derlemek saatler alırken, güçlü bir PC'de dakikalar içinde tamamlanır.

**Kaynak Kısıtlamaları:** Gömülü sistemlerde RAM ve depolama sınırlıdır. BeagleBone Black yalnızca 512MB RAM ve 4GB eMMC'ye sahiptir. Bu kaynaklar, büyük projeleri derlemek için yetersiz kalabilir. Ayrıca derleme sırasında oluşan geçici dosyalar için yeterli alan olmayabilir.

**Geliştirme Araçları:** IDE'ler (VS Code, CLion), debug araçları ve diğer geliştirme ortamları genellikle güçlü masaüstü sistemler için tasarlanmıştır. Bu araçları gömülü sistemde çalıştırmak pratik değildir.

**Otomasyon:** CI/CD pipeline'ları ve otomatik build sistemleri genellikle güçlü sunucularda çalışır. Cross-compilation, bu sistemlerin gömülü hedefler için kod üretmesini sağlar.

**Headless Sistemler:** Birçok gömülü sistem ekran veya klavyeye sahip değildir. Bu sistemlerde doğrudan geliştirme yapmak pratik olarak mümkün değildir.

---

## 📊 Doğal vs Çapraz Araç Zincirleri

Araç zincirleri, kullanım amacına göre iki ana kategoriye ayrılır:

### Doğal Araç Zincirleri (Native Toolchains)

Doğal araç zincirleri, derlemenin yapıldığı sistemde çalışacak kod üreten araç zincirleridir. Yani host ve target aynı sistemdir. Örneğin, bir x86_64 Linux bilgisayarındaki standart `gcc` derleyicisi, yine x86_64 Linux için kod üretir.

Raspberry Pi gibi güçlü SBC'ler için doğal araç zincirleri iyi bir seçenek olabilir. Raspberry Pi 4 veya 5, yeterince güçlü donanıma sahiptir ve doğrudan üzerinde geliştirme yapılabilir. Bu durumda derlenen programlar hemen test edilebilir, dosya transfer işlemleri gerekmez.

### Çapraz Araç Zincirleri (Cross Toolchains)

Çapraz araç zincirleri, derlemenin yapıldığı sistemden farklı bir sistem için kod üreten araç zincirleridir. Host ve target farklı mimarilerdedir. Örneğin, x86_64 PC'de ARM için kod üreten `arm-linux-gnueabihf-gcc` bir çapraz derleyicidir.

BeagleBone Black gibi kaynak kısıtlı sistemler için çapraz araç zincirleri neredeyse zorunludur. BBB'nin 512MB RAM'i, büyük projeleri derlemek için yetersizdir.

### Hangisini Seçmeli?

| Durum | Öneri |
|-------|-------|
| Raspberry Pi 4/5 (güçlü SBC) | Doğal araç zinciri tercih edilebilir |
| BeagleBone Black (sınırlı kaynak) | Çapraz araç zinciri önerilir |
| Büyük projeler (kernel, Qt) | Çapraz araç zinciri şart |
| Hızlı prototipleme | Doğal araç zinciri |
| CI/CD entegrasyonu | Çapraz araç zinciri |

---

## 📛 Araç Zinciri İsimlendirme Kuralları

Araç zincirlerinin isimlendirmesi genellikle hedef sisteme göre yapılır. İsimlendirmede tire (-) karakterleriyle ayrılmış üç veya dört alan kullanılır:

### Üçlü İsimlendirme (Triple)

```
CPU-Tedarikçi-İşletimSistemi
```

### Dörtlü İsimlendirme (Quadruple)

```
CPU-Tedarikçi-İşletimSistemi-ABI
```

Bu alanların anlamları şöyledir:

**CPU:** Hedef işlemci mimarisi. Örnekler:
- `arm`: 32-bit ARM
- `aarch64`: 64-bit ARM
- `x86_64`: 64-bit Intel/AMD
- `i686`: 32-bit Intel/AMD
- `mips`: MIPS mimarisi

**Tedarikçi (Vendor):** Araç zincirini hazırlayan kurum. `none` veya `unknown` yazılabilir. Örnekler:
- `apple`: Apple tarafından hazırlanmış
- `none`: Belirli bir tedarikçi yok
- `unknown`: Bilinmiyor

**İşletim Sistemi:** Hedef işletim sistemi. Örnekler:
- `linux`: Linux işletim sistemi
- `darwin`: macOS (Darwin kernel)
- `mingw32`: Windows (MinGW)
- `eabi`: İşletim sistemi yok (bare metal)

**ABI (Application Binary Interface):** Uygulama ikili arayüzü. Örnekler:
- `gnu`: GNU C kütüphanesi (glibc)
- `gnueabi`: Gömülü ABI
- `gnueabihf`: Hardware floating point ile gömülü ABI
- `musl`: musl C kütüphanesi

### Örnek İsimler ve Anlamları

| Araç Zinciri İsmi | Açıklama |
|-------------------|----------|
| `arm-linux-gnueabihf` | ARM 32-bit, Linux, GNU EABI, Hard Float |
| `aarch64-linux-gnu` | ARM 64-bit, Linux, GNU ABI |
| `arm-none-eabi` | ARM 32-bit, İşletim sistemi yok (bare metal) |
| `x86_64-linux-gnu` | Intel/AMD 64-bit, Linux, GNU ABI |
| `arm-none-linux-gnueabihf` | ARM 32-bit, Linux, GNU EABI, Hard Float, tedarikçi yok |

### Hard Float vs Soft Float

ARM sistemlerinde iki farklı floating point yöntemi vardır:

**Hard Float (hf):** Donanımsal FPU kullanılır. Daha hızlıdır ancak FPU olmayan işlemcilerde çalışmaz. Modern ARM SBC'lerde (RPi, BBB) hard float kullanılır.

**Soft Float:** FPU işlemleri yazılımsal olarak simüle edilir. Daha yavaştır ancak FPU'suz sistemlerde de çalışır.

---

## 📦 Araç Zinciri Edinme Yolları

Araç zincirlerini üç farklı yolla edinebilirsiniz:

### 1. Hazır Araç Zincirleri (Pre-built)

En kolay yöntem, önceden derlenmiş hazır araç zincirlerini indirmektir. Bu araç zincirleri kurulum gerektirmez, sadece arşivden çıkarılır ve kullanılır.

**Ubuntu/Debian Paket Yöneticisi:**

```bash
# ARM 32-bit (BBB, eski RPi)
sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# ARM 64-bit (RPi 4/5)
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Kontrol
arm-linux-gnueabihf-gcc --version
aarch64-linux-gnu-gcc --version
```

**ARM Resmi Araç Zincirleri:**

ARM firması, kendi resmi araç zincirlerini ücretsiz olarak sunmaktadır. Bu araç zincirleri en güncel GCC sürümlerini içerir ve ARM mimarisi için optimize edilmiştir.

İndirme adresi: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

```bash
# İndir (BBB için)
wget https://developer.arm.com/-/media/Files/downloads/.../arm-gnu-toolchain-*-x86_64-arm-none-linux-gnueabihf.tar.xz

# Çıkart
tar -xf arm-gnu-toolchain-*-x86_64-arm-none-linux-gnueabihf.tar.xz -C /opt/

# PATH'e ekle
export PATH=$PATH:/opt/arm-gnu-toolchain-*/bin

# Test
arm-none-linux-gnueabihf-gcc --version
```

**Linaro Araç Zincirleri:**

Linaro, kar amacı gütmeyen bir kuruluştur ve ARM ekosistemi için araç zincirleri sağlar. Linaro araç zincirleri, endüstri standardı olarak kabul edilir.

İndirme adresi: https://releases.linaro.org/components/toolchain/binaries

**Bootlin Araç Zincirleri:**

Bootlin, gömülü Linux konusunda uzmanlaşmış bir firmadır ve çeşitli mimariler için hazır araç zincirleri sunar.

İndirme adresi: https://toolchains.bootlin.com/

### 2. Araç Zinciri Oluşturma Araçları

Daha özelleştirilmiş araç zincirleri için crosstool-NG, Buildroot veya Yocto kullanılabilir. Bu araçlar, belirli bir hedef sistem için optimize edilmiş araç zincirleri oluşturmanıza olanak tanır.

### 3. Manuel Oluşturma

Araç zincirlerini sıfırdan manuel olarak oluşturmak mümkündür ancak oldukça zorlu bir süreçtir. Binutils, GCC ve glibc kaynak kodlarının derlenmesi gerekir. Bu yöntem, çok özel gereksinimler olmadığı sürece önerilmez.

---

## ⚙️ Ortam Değişkenleri

Cross-compilation yaparken bazı ortam değişkenlerinin doğru ayarlanması kritik öneme sahiptir:

### PATH

Araç zincirinin `bin` dizini PATH ortam değişkenine eklenmelidir. Bu sayede derleyici ve diğer araçlar doğrudan çağrılabilir.

```bash
# Geçici (mevcut terminal oturumu için)
export PATH=$PATH:/opt/arm-gnu-toolchain/bin

# Kalıcı (~/.bashrc dosyasına ekle)
echo 'export PATH=$PATH:/opt/arm-gnu-toolchain/bin' >> ~/.bashrc
source ~/.bashrc
```

### CROSS_COMPILE

CROSS_COMPILE ortam değişkeni, araç zincirinin **prefix'ini** (ön ekini) belirtir. Bu değişken kernel ve U-Boot derlemelerinde kullanılır. Prefix, tire (-) karakteri ile biter.

```bash
# BBB için (ARM 32-bit)
export CROSS_COMPILE=arm-linux-gnueabihf-

# RPi için (ARM 64-bit)
export CROSS_COMPILE=aarch64-linux-gnu-
```

Bu değişken ayarlandığında, `make` komutu otomatik olarak doğru derleyiciyi bulur:
- `${CROSS_COMPILE}gcc` → `arm-linux-gnueabihf-gcc`
- `${CROSS_COMPILE}ld` → `arm-linux-gnueabihf-ld`
- `${CROSS_COMPILE}objcopy` → `arm-linux-gnueabihf-objcopy`

### ARCH

ARCH ortam değişkeni, hedef mimariyi belirtir. Kernel derlemelerinde kullanılır.

```bash
export ARCH=arm      # ARM 32-bit
export ARCH=arm64    # ARM 64-bit
```

### Örnek ~/.bashrc Yapılandırması

```bash
# ARM Cross-Compile Environment
# ===============================

# ARM 32-bit (BBB)
export CROSS_COMPILE_ARM32=arm-linux-gnueabihf-
export PATH_ARM32=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin

# ARM 64-bit (RPi)
export CROSS_COMPILE_ARM64=aarch64-linux-gnu-
export PATH_ARM64=/opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin

# Kısayol fonksiyonları
bbb-setup() {
    export ARCH=arm
    export CROSS_COMPILE=$CROSS_COMPILE_ARM32
    export PATH=$PATH:$PATH_ARM32
    echo "BBB environment configured"
}

rpi-setup() {
    export ARCH=arm64
    export CROSS_COMPILE=$CROSS_COMPILE_ARM64
    export PATH=$PATH:$PATH_ARM64
    echo "RPi environment configured"
}

# Kısayol alias'lar
alias arm32-gcc='arm-linux-gnueabihf-gcc'
alias arm64-gcc='aarch64-linux-gnu-gcc'
```

Kullanım:

```bash
source ~/.bashrc
bbb-setup    # BBB için ortamı hazırla
rpi-setup    # RPi için ortamı hazırla
```

---

## 💻 Cross-Compile İşlemi

Cross-compilation işlemi, normal derleme işlemiyle hemen hemen aynıdır. Tek fark, native `gcc` yerine cross-compiler kullanılmasıdır.

### Basit Bir Örnek

```c
/* hello_arm.c */
#include <stdio.h>

int main(void)
{
    printf("Merhaba ARM!\n");
    printf("Bu program %s mimarisinde çalışıyor.\n",
#if defined(__arm__)
           "ARM 32-bit"
#elif defined(__aarch64__)
           "ARM 64-bit"
#elif defined(__x86_64__)
           "x86_64"
#else
           "bilinmeyen"
#endif
    );
    return 0;
}
```

### Derleme

```bash
# Host için (normal derleme)
gcc -o hello hello_arm.c
./hello  # Çalışır

# BBB için (ARM 32-bit)
arm-linux-gnueabihf-gcc -o hello-bbb hello_arm.c
./hello-bbb  # ÇALIŞMAZ! "cannot execute binary file"

# RPi için (ARM 64-bit)
aarch64-linux-gnu-gcc -o hello-rpi hello_arm.c
./hello-rpi  # ÇALIŞMAZ! "cannot execute binary file"
```

### File Komutu ile Doğrulama

`file` komutu, bir dosyanın hangi mimari için derlendiğini gösterir:

```bash
file hello
# hello: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV)...

file hello-bbb
# hello-bbb: ELF 32-bit LSB pie executable, ARM, EABI5, version 1 (SYSV)...

file hello-rpi
# hello-rpi: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV)...
```

---

## 📦 Statik vs Dinamik Bağlama

C programları çalışırken standart kütüphane fonksiyonlarına (printf, malloc, vb.) ihtiyaç duyar. Bu fonksiyonlar ya çalışma zamanında dinamik olarak yüklenir ya da derleme zamanında programa gömülür.

### Dinamik Bağlama (Dynamic Linking)

Varsayılan bağlama yöntemi dinamiktir. Program, çalışma zamanında paylaşımlı kütüphaneleri (.so dosyaları) yükler.

**Avantajları:**
- Küçük binary boyutu
- Kütüphane güncellemeleri tüm programlara yansır
- Bellek verimliliği (kütüphaneler paylaşılır)

**Dezavantajları:**
- Hedef sistemde aynı kütüphaneler olmalı
- Versiyon uyumsuzlukları sorun çıkarabilir
- Kütüphane bulunamazsa program çalışmaz

```bash
# Dinamik derleme (varsayılan)
arm-linux-gnueabihf-gcc -o hello hello.c

# Binary boyutu
ls -lh hello
# -rwxr-xr-x 1 user user 8.2K ... hello

# Bağımlılıkları göster
arm-linux-gnueabihf-readelf -d hello | grep NEEDED
# 0x00000001 (NEEDED) Shared library: [libc.so.6]
```

### Statik Bağlama (Static Linking)

Statik bağlamada, tüm kütüphane kodları programa gömülür. Program hiçbir dış bağımlılığa sahip olmaz.

**Avantajları:**
- Bağımlılık yok, her yerde çalışır
- Kütüphane versiyon sorunları ortadan kalkar
- Minimal sistemlerde ideal

**Dezavantajları:**
- Büyük binary boyutu
- Kütüphane güncellemeleri programa yansımaz
- Daha fazla disk ve bellek kullanımı

```bash
# Statik derleme
arm-linux-gnueabihf-gcc -static -o hello-static hello.c

# Binary boyutu
ls -lh hello-static
# -rwxr-xr-x 1 user user 712K ... hello-static

# Bağımlılık yok
arm-linux-gnueabihf-readelf -d hello-static | grep NEEDED
# (çıktı yok)
```

### Ne Zaman Hangisi?

| Durum | Tercih |
|-------|--------|
| Normal uygulama | Dinamik |
| Buildroot/Yocto ile minimal sistem | Statik |
| Farklı dağıtımlar arası taşıma | Statik |
| BusyBox tarzı araçlar | Statik |
| Debug/test amaçlı | Statik |
| Ürün kalıbı (production) | Dinamik |

---

## 🔧 Cross-Compile Makefile

Makefile, derleme sürecini otomatikleştirmek için kullanılır. Cross-compilation için Makefile'da derleyici ayarları yapılandırılmalıdır.

```makefile
# Cross-Compile Makefile
# ======================
# Kullanım:
#   make              - Host için derle
#   make TARGET=bbb   - BeagleBone Black için
#   make TARGET=rpi   - Raspberry Pi için
#   make static       - Statik bağlama

# Hedef seçimi (varsayılan: host)
TARGET ?= host

# Platform-specific ayarlar
ifeq ($(TARGET),bbb)
    # BeagleBone Black (ARM Cortex-A8, 32-bit)
    CC = arm-linux-gnueabihf-gcc
    CFLAGS = -Wall -O2 -march=armv7-a -mfpu=neon -mfloat-abi=hard
    LDFLAGS =
    DEPLOY_HOST = debian@192.168.7.2
    PLATFORM_NAME = BeagleBone Black
else ifeq ($(TARGET),rpi)
    # Raspberry Pi 4/5 (ARM Cortex-A72, 64-bit)
    CC = aarch64-linux-gnu-gcc
    CFLAGS = -Wall -O2 -march=armv8-a
    LDFLAGS =
    DEPLOY_HOST = pi@raspberrypi.local
    PLATFORM_NAME = Raspberry Pi
else
    # Host sistem (native)
    CC = gcc
    CFLAGS = -Wall -O2
    LDFLAGS =
    PLATFORM_NAME = Host
endif

# Proje ayarları
SOURCES = main.c
TARGET_BIN = myapp

# Ana hedef
all: $(TARGET_BIN)
	@echo "================================"
	@echo "Platform: $(PLATFORM_NAME)"
	@echo "Binary:   $(TARGET_BIN)"
	@echo "================================"
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

# Deploy
deploy: $(TARGET_BIN)
ifndef DEPLOY_HOST
	$(error Hedef belirtilmedi. Kullanim: make TARGET=bbb deploy)
endif
	scp $(TARGET_BIN) $(DEPLOY_HOST):~/
	@echo "$(TARGET_BIN) -> $(DEPLOY_HOST)"

# Temizlik
clean:
	rm -f $(TARGET_BIN) *.o

.PHONY: all clean bbb rpi static static-bbb deploy
```

---

## 📤 Target'a Dosya Transferi

Cross-compile sonucunda oluşan binary, hedef sisteme transfer edilmelidir. Bunun için çeşitli yöntemler kullanılabilir:

### SCP (Secure Copy)

SCP, SSH protokolü üzerinden güvenli dosya transferi yapar. En yaygın kullanılan yöntemdir.

```bash
# Tek dosya gönder
scp myapp debian@192.168.7.2:~/

# Belirli dizine gönder
scp myapp debian@192.168.7.2:/home/debian/bin/

# Dizin gönder (recursive)
scp -r project/ debian@192.168.7.2:~/

# Hedeften dosya al
scp debian@192.168.7.2:~/output.txt ./
```

### rsync (Daha Hızlı)

rsync, yalnızca değişen dosyaları transfer eder. Büyük projeler için SCP'den çok daha hızlıdır.

```bash
# Senkronize et
rsync -avz --progress project/ debian@192.168.7.2:~/project/

# Artımlı güncelleme
rsync -avz --delete project/ debian@192.168.7.2:~/project/
```

### SSHFS (SSH Filesystem)

SSHFS, uzak sistemi yerel bir dizin olarak bağlar. Dosya yazıldığında otomatik olarak hedefe transfer edilir.

```bash
# Kurulum
sudo apt install sshfs

# Mount point oluştur
mkdir ~/bbb

# Bağla
sshfs debian@192.168.7.2:/home/debian ~/bbb

# Artık ~/bbb dizini BBB'deki /home/debian gibi davranır
cp myapp ~/bbb/

# Bağlantıyı kopar
umount ~/bbb
```

### NFS (Network File System)

Geliştirme sırasında NFS kullanmak çok pratiktir. Host'taki bir dizin doğrudan hedef sistemde mount edilir.

```bash
# Host'ta NFS sunucu kur
sudo apt install nfs-kernel-server

# /etc/exports dosyasına ekle
echo "/home/user/embedded 192.168.7.0/24(rw,sync,no_subtree_check)" | sudo tee -a /etc/exports

# Servisi yeniden başlat
sudo exportfs -a
sudo systemctl restart nfs-kernel-server

# Target'ta mount et (BBB üzerinde)
sudo mount -t nfs 192.168.7.1:/home/user/embedded /mnt/nfs
```

---

## 🐛 Hata Ayıklama ve Kontrol

Cross-compile edilen binary'leri analiz etmek için araç zincirindeki yardımcı programlar kullanılır:

### Binary Kontrolü

```bash
# Dosya tipi
file myapp
# ELF 32-bit LSB executable, ARM, EABI5...

# Paylaşımlı kütüphane bağımlılıkları
arm-linux-gnueabihf-readelf -d myapp | grep NEEDED

# ELF header bilgisi
arm-linux-gnueabihf-readelf -h myapp

# Sembol tablosu
arm-linux-gnueabihf-nm myapp

# Binary boyutu analizi
arm-linux-gnueabihf-size myapp
```

### Yaygın Hatalar ve Çözümleri

| Hata | Neden | Çözüm |
|------|-------|-------|
| "cannot execute binary file" | Yanlış mimari | `file` ile kontrol et |
| "libXXX not found" | Dinamik kütüphane eksik | Statik derle veya kütüphaneyi kopyala |
| "No such file or directory" | Linker path sorunu | Statik derle |
| "GLIBC version not found" | glibc versiyon uyumsuzluğu | Eski toolchain kullan veya statik derle |

---

## 🎯 Pratik Ödevler

1. ARM toolchain'i kurun (apt veya manuel)
2. Basit bir "Hello ARM" programı yazın ve cross-compile edin
3. `file` komutu ile binary'yi doğrulayın
4. Binary'yi BBB veya RPi'a SCP ile gönderin
5. Hedef sistemde çalıştırın
6. Statik ve dinamik derleme boyutlarını karşılaştırın
7. Cross-compile destekli Makefile yazın

---

## 📚 Kaynaklar

- [ARM GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [Linaro Toolchains](https://releases.linaro.org/components/toolchain/binaries)
- [Bootlin Toolchains](https://toolchains.bootlin.com/)
- [Crosstool-NG](https://crosstool-ng.github.io/)

---

*Sonraki ders: Linux Kernel Basics*
