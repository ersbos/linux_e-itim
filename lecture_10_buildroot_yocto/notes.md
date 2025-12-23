# Ders 10: Buildroot ve Yocto

> **Konu:** Özel Gömülü Linux Sistemleri Oluşturma

---

## 📋 Bu Derste İşlenenler

- [x] Neden özel Linux?
- [x] Buildroot temelleri
- [x] Yocto Project temelleri
- [x] Karşılaştırma
- [x] Basit sistem oluşturma

---

## ❓ Neden Özel Linux Sistemi?

| Sorun | Çözüm |
|-------|-------|
| Debian/Ubuntu çok büyük | Minimal sistem |
| Gereksiz paketler | Sadece gerekli olanlar |
| Boot süresi uzun | Hızlı boot |
| Güvenlik riskleri | Saldırı yüzeyi azaltma |
| Lisans sorunları | Kontrollü paketler |

### Araç Seçenekleri

| Araç | Karmaşıklık | Esneklik | Öğrenme |
|------|-------------|----------|---------|
| **Buildroot** | Düşük | Orta | Kolay |
| **Yocto** | Yüksek | Çok yüksek | Zor |
| **OpenWRT** | Orta | Router odaklı | Kolay |

---

## 🔧 Buildroot

### Buildroot Nedir?

**Buildroot** = Cross-compilation araç zinciri + root filesystem + kernel oluşturmak için basit ve verimli araç.

### Temel Özellikler

- Makefile ve Kconfig tabanlı
- 2000+ paket desteği
- Küçük öğrenme eğrisi
- Tüm sistem sıfırdan derlenir

### Kurulum ve Başlangıç

```bash
# İndir
git clone https://github.com/buildroot/buildroot.git
cd buildroot

# Defconfig listele
make list-defconfigs

# BBB için config
make beaglebone_defconfig

# Konfigürasyon menüsü
make menuconfig

# Derle (uzun sürer!)
make -j$(nproc)

# Çıktılar
ls output/images/
# rootfs.ext2  sdcard.img  zImage  am335x-boneblack.dtb
```

### menuconfig Temel Menüler

```
┌── Target options
│   ├── Target Architecture (ARM little endian)
│   └── Target Architecture Variant (cortex-A8)
├── Build options
├── Toolchain
│   └── Toolchain type (Buildroot toolchain)
├── System configuration
│   ├── Root password
│   └── System hostname
├── Kernel
│   └── Kernel version
├── Target packages
│   ├── BusyBox
│   ├── Networking
│   └── ...
└── Filesystem images
    └── ext2/3/4 root filesystem
```

### Özel Paket Ekleme

```bash
# Paket dizini oluştur
mkdir -p package/myapp

# myapp.mk dosyası
cat > package/myapp/myapp.mk << 'EOF'
MYAPP_VERSION = 1.0
MYAPP_SITE = $(TOPDIR)/../myapp
MYAPP_SITE_METHOD = local

define MYAPP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" -C $(@D)
endef

define MYAPP_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/myapp $(TARGET_DIR)/usr/bin/myapp
endef

$(eval $(generic-package))
EOF

# Config.in dosyası
cat > package/myapp/Config.in << 'EOF'
config BR2_PACKAGE_MYAPP
    bool "myapp"
    help
      My custom application
EOF
```

---

## 🍳 Yocto Project

### Yocto Nedir?

**Yocto Project** = Endüstri standardı gömülü Linux oluşturma framework'ü.

### Temel Kavramlar

| Terim | Açıklama |
|-------|----------|
| **Recipe** (.bb) | Paket build talimatları |
| **Layer** | İlgili recipe'lerin koleksiyonu |
| **BitBake** | Build engine |
| **Poky** | Referans dağıtım |
| **meta-\*** | Layer isimleri |

### Kurulum

```bash
# Bağımlılıklar
sudo apt install gawk wget git diffstat unzip texinfo \
    gcc build-essential chrpath socat cpio python3 \
    python3-pip python3-pexpect xz-utils debianutils \
    iputils-ping python3-git python3-jinja2 libegl1-mesa \
    libsdl1.2-dev pylint3 xterm python3-subunit mesa-common-dev

# Poky indir
git clone git://git.yoctoproject.org/poky
cd poky

# Branch seç
git checkout -b kirkstone origin/kirkstone

# Ortamı hazırla
source oe-init-build-env

# Konfigürasyon
# conf/local.conf düzenle
```

### local.conf Ayarları

```bash
# conf/local.conf

# Makine seçimi
MACHINE = "beaglebone-yocto"

# Paralel derleme
BB_NUMBER_THREADS = "8"
PARALLEL_MAKE = "-j 8"

# İndirme dizini (paylaşılabilir)
DL_DIR = "/opt/yocto/downloads"

# Ek özellikler
EXTRA_IMAGE_FEATURES += "debug-tweaks ssh-server-dropbear"
```

### Build

```bash
# Minimal image
bitbake core-image-minimal

# Daha gelişmiş
bitbake core-image-full-cmdline

# Çıktılar
ls tmp/deploy/images/beaglebone-yocto/
```

### Layer Ekleme

```bash
# Mevcut layer'ları listele
bitbake-layers show-layers

# Layer ekle
bitbake-layers add-layer ../meta-openembedded/meta-oe

# BSP layer (board support package)
git clone git://git.yoctoproject.org/meta-ti
bitbake-layers add-layer ../meta-ti
```

---

## ⚖️ Buildroot vs Yocto

| Kriter | Buildroot | Yocto |
|--------|-----------|-------|
| **Öğrenme** | Kolay | Zor |
| **Esneklik** | Orta | Çok yüksek |
| **Build süresi** | Hızlı | Yavaş |
| **Paket desteği** | 2000+ | 10000+ |
| **Endüstri** | Hobi/SME | Enterprise |
| **Artımlı build** | Sınırlı | Mükemmel |
| **Dokümantasyon** | İyi | Çok iyi |

### Hangisini Seçmeli?

| Durum | Öneri |
|-------|-------|
| Hızlı prototip | Buildroot |
| Küçük proje | Buildroot |
| Karmaşık ürün | Yocto |
| Çoklu platform | Yocto |
| Öğrenme | Buildroot |

---

## 🎯 Pratik

1. Buildroot indir ve BBB için derle
2. menuconfig ile paket ekle/çıkar
3. Oluşan image'ı SD karta yaz
4. Boot et ve test et
5. (İleri) Yocto ile aynı işlemi yap

---

*Sonraki ders: Real-Time Linux*
