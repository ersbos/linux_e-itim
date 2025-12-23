# Ders 10: Buildroot ve Yocto

> **Konu:** Özel Gömülü Linux Sistemleri Oluşturma
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Neden özel Linux sistemi?
- [x] Araç zinciri oluşturma yöntemleri
- [x] Buildroot temelleri ve kullanımı
- [x] Yocto Project mimarisi
- [x] Karşılaştırma ve seçim kriterleri
- [x] Özel paket ekleme

---

## ❓ Neden Özel Linux Sistemi?

Masaüstü Linux dağıtımları (Debian, Ubuntu, Fedora gibi) genel amaçlı kullanım için tasarlanmıştır ve binlerce paketi içerir. Ancak gömülü sistemlerde bu yaklaşım birçok sorun oluşturur:

**Boyut Sorunu:** Tipik bir Debian kurulumu gigabyte'lar düzeyinde disk alanı kaplar. Oysa birçok gömülü sistem megabyte'lar düzeyinde depolama alanına sahiptir. Örneğin 4MB Flash belleğe sahip bir sistemde standart bir Linux dağıtımı çalıştırmak imkansızdır.

**Boot Süresi:** Masaüstü dağıtımlar onlarca servis başlatır ve boot süresi dakikalar alabilir. Gömülü sistemlerde genellikle saniyeler içinde çalışır duruma geçilmesi beklenir. Örneğin bir araç kamerasının açılışta hemen kayda başlaması gerekir.

**Kaynak Tüketimi:** Gereksiz paketler RAM ve CPU kaynağı tüketir. Sınırlı kaynaklara sahip gömülü sistemlerde bu kabul edilemez.

**Güvenlik:** Her ek paket potansiyel bir güvenlik açığıdır. Yalnızca gerekli bileşenleri içeren bir sistem, saldırı yüzeyini minimize eder.

**Lisans Kontrolü:** Ticari ürünlerde hangi yazılımların dahil edildiğini ve lisans gereksinimlerini kontrol etmek kritiktir. Özel bir sistem oluşturarak tam kontrol sağlanır.

---

## 🔧 Araç Zinciri Oluşturma Yöntemleri

Gömülü Linux sistemi oluşturmanın temelinde araç zinciri (toolchain) vardır. Araç zinciri elde etmenin üç temel yolu bulunmaktadır:

### 1. Hazır (Pre-built) Araç Zincirleri

Çeşitli kurumlar ve topluluklar tarafından oluşturulmuş hazır araç zincirleri kullanılabilir. Bu yöntem en hızlı başlangıç yoludur ancak bazı dezavantajları vardır:

- Hedef sisteminizle tam uyumlu olmayabilir
- Optimize edilmemiş genel amaçlı derleme yapılmıştır
- Kütüphane versiyonları hedefinizle uyuşmayabilir

**Hazır Araç Zinciri Kaynakları:**
- ARM resmi araç zincirleri: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- Linaro: https://releases.linaro.org/components/toolchain/binaries
- Bootlin: https://toolchains.bootlin.com/

### 2. Araç Zinciri Oluşturan Yazılımlar

crosstool-NG, Buildroot ve Yocto gibi projeler özelleştirilmiş araç zincirleri oluşturabilir. Bu yöntemle hedef sistemin tam uyumlu, optimize edilmiş araç zincirleri elde edilir.

### 3. Manuel Oluşturma

Araç zincirindeki her bileşenin (GCC, binutils, glibc) kaynak kodlardan derlenmesi. Bu yöntem çok zorlu ve zaman alıcıdır, özel bilgi gerektirir.

---

## 🏗️ Buildroot

### Buildroot Nedir?

Buildroot, çapraz derleme araç zinciri, root dosya sistemi ve Linux çekirdeği içeren tam bir gömülü Linux sistemi oluşturmak için basit ve verimli bir araçtır. Öğrenme eğrisi düşüktür ve hızlı sonuç elde edilir.

**Temel Özellikler:**
- Makefile ve Kconfig tabanlı konfigürasyon (Linux çekirdeğine benzer)
- 2000+ paket desteği
- Tüm sistem sıfırdan derlenir (reproducible builds)
- Küçük ve anlaşılır kod tabanı
- Aktif topluluk ve dokümantasyon

### Buildroot Kurulum ve Kullanım

```bash
# Kaynak kodları indir
git clone https://github.com/buildroot/buildroot.git
cd buildroot

# Stabil sürüme geç (önerilen)
git checkout 2023.02.x

# Mevcut defconfig'leri listele
make list-defconfigs

# BeagleBone Black için hazır konfigürasyon
make beaglebone_defconfig

# Raspberry Pi 4 için hazır konfigürasyon
make raspberrypi4_64_defconfig

# Konfigürasyon menüsünü aç
make menuconfig

# Derleme başlat (uzun sürer: 30dk - 2 saat)
make -j$(nproc)

# Çıktıları kontrol et
ls output/images/
# rootfs.ext2  sdcard.img  zImage  am335x-boneblack.dtb
```

### menuconfig Menü Yapısı

Buildroot'un menuconfig arayüzü Linux çekirdeğine benzer ve şu kategorileri içerir:

```
┌── Target options                 (Hedef mimari ayarları)
│   ├── Target Architecture        → ARM (little endian)
│   ├── Target Architecture Variant → cortex-A8 (BBB için)
│   └── Floating Point Strategy    → VFPv3
│
├── Build options                  (Derleme ayarları)
│   ├── Download dir               → İndirme dizini
│   └── Enable compiler cache      → ccache kullanımı
│
├── Toolchain                      (Araç zinciri ayarları)
│   ├── Toolchain type             → Buildroot / External
│   ├── C library                  → glibc / uClibc-ng / musl
│   └── GCC compiler Version       → 11.x / 12.x
│
├── System configuration           (Sistem ayarları)
│   ├── System hostname            → embedded-linux
│   ├── System banner              → Boot mesajı
│   ├── Root password              → Şifre
│   └── Init system                → BusyBox / systemd
│
├── Kernel                         (Çekirdek ayarları)
│   ├── Kernel version             → 5.15.x / 6.1.x
│   └── Kernel configuration       → defconfig kullanımı
│
├── Target packages                (Hedef paketler)
│   ├── BusyBox                    → Temel komutlar
│   ├── Networking applications    → SSH, wget, curl
│   ├── Development tools          → GCC, make
│   └── Libraries                  → OpenSSL, zlib
│
└── Filesystem images              (Dosya sistemi çıktıları)
    ├── ext2/3/4 root filesystem
    ├── squashfs root filesystem
    └── initial RAM filesystem (initramfs)
```

### Buildroot Çıktıları

Derleme tamamlandığında `output/images/` dizininde şu dosyalar oluşur:

| Dosya | Açıklama |
|-------|----------|
| `zImage` veya `Image` | Sıkıştırılmış Linux çekirdeği |
| `rootfs.ext4` | Root dosya sistemi (ext4) |
| `rootfs.tar` | Root dosya sistemi (tar arşivi) |
| `sdcard.img` | SD karta yazılabilir tam imaj |
| `*.dtb` | Device Tree Blob dosyaları |
| `uboot.bin` | U-Boot bootloader (varsa) |

### Özel Paket Ekleme

Buildroot'a kendi uygulamanızı paket olarak ekleyebilirsiniz:

**1. Paket dizini oluştur:**
```bash
mkdir -p package/myapp
```

**2. myapp.mk dosyası (Makefile kuralları):**
```makefile
# package/myapp/myapp.mk

MYAPP_VERSION = 1.0
MYAPP_SITE = $(TOPDIR)/../myapp-source
MYAPP_SITE_METHOD = local

# Derleme komutları
define MYAPP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" -C $(@D)
endef

# Kurulum komutları
define MYAPP_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/myapp $(TARGET_DIR)/usr/bin/myapp
endef

$(eval $(generic-package))
```

**3. Config.in dosyası (menuconfig girişi):**
```
# package/myapp/Config.in

config BR2_PACKAGE_MYAPP
    bool "myapp"
    help
      My custom embedded application.
      
      This is an example package for learning Buildroot.
```

**4. Ana Config.in dosyasına ekle:**
```bash
# package/Config.in dosyasına aşağıdaki satırı ekle:
source "package/myapp/Config.in"
```

**5. menuconfig'den etkinleştir ve derle:**
```bash
make menuconfig   # Target packages → myapp seç
make
```

---

## 🍳 Yocto Project

### Yocto Nedir?

Yocto Project, Linux Foundation tarafından yönetilen, endüstri standardı haline gelmiş bir gömülü Linux oluşturma framework'üdür. Buildroot'tan çok daha kapsamlı ve esnektir, ancak öğrenme eğrisi daha diktir.

**Yocto'nun Avantajları:**
- Endüstri standardı (otomotiv, ağ donanımı, tüketici elektroniği)
- Mükemmel artımlı (incremental) derleme desteği
- 10.000+ hazır tarif (recipe)
- Katmanlı (layer) mimari ile modülerlik
- Geniş endüstri desteği (Intel, TI, NXP, Xilinx)

### Yocto Terminolojisi

Yocto'yu anlamak için önce terminolojisini bilmek gerekir:

| Terim | Açıklama |
|-------|----------|
| **Recipe (.bb)** | Tek bir paketi derlemek için talimatlar içeren dosya |
| **Layer (meta-*)** | İlgili recipe'ların toplandığı dizin yapısı |
| **BitBake** | Yocto'nun derleme motoru (make benzeri) |
| **Poky** | Yocto referans dağıtımı (başlangıç noktası) |
| **Machine** | Hedef donanım tanımı (beaglebone-yocto, raspberrypi4) |
| **Distro** | Dağıtım politikaları (init sistemi, özellikler) |
| **Image** | Son çıktı imaj tanımı (core-image-minimal) |

### Yocto Kurulum

```bash
# Bağımlılıkları kur (Ubuntu/Debian)
sudo apt install gawk wget git diffstat unzip texinfo \
    gcc build-essential chrpath socat cpio python3 \
    python3-pip python3-pexpect xz-utils debianutils \
    iputils-ping python3-git python3-jinja2 libegl1-mesa \
    libsdl1.2-dev python3-subunit mesa-common-dev zstd lz4

# Poky referans dağıtımını indir
git clone git://git.yoctoproject.org/poky
cd poky

# Stabil sürüme geç (LTS: kirkstone, scarthgap)
git checkout -b kirkstone origin/kirkstone

# Derleme ortamını hazırla
source oe-init-build-env

# Bu komut "build" dizinine geçer ve ortam değişkenlerini ayarlar
```

### Yocto Konfigürasyon

Ortam hazırlandıktan sonra `conf/` dizininde iki önemli dosya düzenlenir:

**conf/local.conf (Yerel ayarlar):**
```bash
# Hedef makine
MACHINE = "beaglebone-yocto"
# veya
# MACHINE = "raspberrypi4-64"

# Paralel derleme (CPU çekirdek sayısına göre)
BB_NUMBER_THREADS = "8"
PARALLEL_MAKE = "-j 8"

# İndirme dizini (paylaşılabilir)
DL_DIR = "/opt/yocto/downloads"

# Paylaşımlı state cache (artımlı derleme hızlandırır)
SSTATE_DIR = "/opt/yocto/sstate-cache"

# Ek özellikler
EXTRA_IMAGE_FEATURES += "debug-tweaks ssh-server-dropbear"

# Paket yönetim formatı
PACKAGE_CLASSES = "package_ipk"
```

**conf/bblayers.conf (Layer tanımları):**
```bash
BBLAYERS ?= " \
  /home/user/poky/meta \
  /home/user/poky/meta-poky \
  /home/user/poky/meta-yocto-bsp \
  "
```

### Yocto Derleme

```bash
# Minimal imaj derle
bitbake core-image-minimal

# Komut satırı araçları içeren imaj
bitbake core-image-full-cmdline

# X11 grafik arayüzlü imaj
bitbake core-image-sato

# Tek bir paketi derle
bitbake busybox

# Çıktıları görüntüle
ls tmp/deploy/images/beaglebone-yocto/
```

### Layer Yönetimi

Yocto'nun gücü katmanlı mimarisinden gelir:

```bash
# Mevcut layer'ları listele
bitbake-layers show-layers

# Yeni layer ekle
bitbake-layers add-layer ../meta-openembedded/meta-oe

# BSP layer ekle (Board Support Package)
git clone git://git.yoctoproject.org/meta-ti
bitbake-layers add-layer ../meta-ti

# Raspberry Pi desteği
git clone git://git.yoctoproject.org/meta-raspberrypi
bitbake-layers add-layer ../meta-raspberrypi

# Layer'ı kaldır
bitbake-layers remove-layer meta-oe
```

### Özel Recipe Oluşturma

```bash
# meta-mylayer/recipes-apps/myapp/myapp_1.0.bb

SUMMARY = "My custom application"
DESCRIPTION = "An example recipe for Yocto"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=..."

SRC_URI = "file://myapp.c \
           file://Makefile"

S = "${WORKDIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 myapp ${D}${bindir}
}
```

---

## ⚖️ Buildroot vs Yocto Karşılaştırması

| Kriter | Buildroot | Yocto |
|--------|-----------|-------|
| **Öğrenme Eğrisi** | Düşük (1-2 hafta) | Yüksek (1-2 ay) |
| **Esneklik** | Orta | Çok yüksek |
| **İlk Derleme Süresi** | Hızlı (30dk-2saat) | Yavaş (2-8 saat) |
| **Artımlı Derleme** | Sınırlı | Mükemmel |
| **Paket Sayısı** | ~2500 | ~10000+ |
| **Endüstri Kullanımı** | Hobi/KOBİ | Enterprise |
| **Dokümantasyon** | İyi | Çok kapsamlı |
| **Topluluk Desteği** | İyi | Çok iyi |
| **Ticari Destek** | Sınırlı | Yaygın |
| **Kod Tabanı** | Küçük, anlaşılır | Büyük, karmaşık |

### Hangisini Seçmeli?

| Senaryo | Öneri | Gerekçe |
|---------|-------|---------|
| Hızlı prototip | Buildroot | Hızlı başlangıç |
| Öğrenme amaçlı | Buildroot | Kolay anlaşılır |
| Küçük proje (<6 ay) | Buildroot | Yeterli esneklik |
| Karmaşık ticari ürün | Yocto | Uzun vadeli bakım |
| Çoklu platform desteği | Yocto | Layer mimarisi |
| Otomotiv/Medikal | Yocto | Endüstri standardı |
| Sınırlı kaynak (ROM<16MB) | Buildroot | Daha küçük çıktı |

---

## 🔄 Tipik İş Akışı

### Buildroot İş Akışı

```
1. git clone buildroot
       ↓
2. make <board>_defconfig
       ↓
3. make menuconfig
   (paket seç, ayarları yap)
       ↓
4. make -j$(nproc)
       ↓
5. output/images/sdcard.img
       ↓
6. dd if=sdcard.img of=/dev/sdX
       ↓
7. Boot ve test
```

### Yocto İş Akışı

```
1. git clone poky + BSP layers
       ↓
2. source oe-init-build-env
       ↓
3. conf/local.conf düzenle
   (MACHINE, features)
       ↓
4. bitbake-layers add-layer ...
       ↓
5. bitbake core-image-minimal
       ↓
6. tmp/deploy/images/*/image.wic
       ↓
7. bmaptool copy image.wic /dev/sdX
       ↓
8. Boot ve test
```

---

## 🎯 Pratik Ödevler

1. Buildroot indirin ve BeagleBone Black için derleyin
2. menuconfig ile SSH server ve GPIO araçları ekleyin
3. Oluşan sdcard.img'ı SD karta yazın ve boot edin
4. Özel bir "Hello Embedded" programını paket olarak ekleyin
5. (İleri) Yocto ile aynı sistemi oluşturun ve karşılaştırın
6. Root dosya sistemi boyutlarını karşılaştırın

---

## 📚 Kaynaklar

- [Buildroot Manual](https://buildroot.org/downloads/manual/manual.html)
- [Yocto Project Documentation](https://docs.yoctoproject.org/)
- [Bootlin Training Materials](https://bootlin.com/training/buildroot/)
- [Yocto Layer Index](https://layers.openembedded.org/)

---

*Sonraki ders: Gerçek Zamanlı Linux (Real-Time Linux)*
