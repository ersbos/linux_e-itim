# Ders 5: Linux Kernel Temelleri

> **Konu:** Kernel Yapısı, Konfigürasyon ve Derleme

---

## 📋 Bu Derste İşlenenler

- [x] Kernel nedir ve ne yapar?
- [x] Kernel kaynak yapısı
- [x] Kernel konfigürasyonu
- [x] Kernel derleme
- [x] Kernel modüller
- [x] Kernel parametreleri

---

## 🧠 Kernel Nedir?

**Kernel** = İşletim sisteminin çekirdeği. Donanım ile yazılım arasındaki köprü.

### Kernel'ın Görevleri

| Görev | Açıklama |
|-------|----------|
| **Process Management** | Süreç oluşturma, zamanlama, sonlandırma |
| **Memory Management** | RAM yönetimi, sayfalama, swap |
| **Device Drivers** | Donanım kontrolü |
| **File Systems** | Dosya sistemi desteği |
| **Networking** | TCP/IP, soketler |
| **Security** | İzinler, SELinux |

### Monolitik vs Mikrokernel

| Özellik | Monolitik | Mikrokernel |
|---------|-----------|-------------|
| Yapı | Tek parça | Modüler, ayrık |
| Performans | Yüksek | Düşük |
| Güvenlik | Daha riskli | Daha güvenli |
| Örnek | Linux | Minix, QNX |

> Linux **monolitik** ama **module** desteği var (hibrit).

---

## 📂 Kernel Kaynak Yapısı

```
linux/
├── arch/           # Mimari bağımlı kod (arm, x86, ...)
├── block/          # Blok cihaz katmanı
├── crypto/         # Şifreleme algoritmaları
├── Documentation/  # Dokümantasyon
├── drivers/        # Aygıt sürücüler
│   ├── gpio/
│   ├── i2c/
│   ├── spi/
│   └── ...
├── fs/             # Dosya sistemleri (ext4, fat, ...)
├── include/        # Header dosyaları
├── init/           # Kernel başlatma kodu
├── kernel/         # Çekirdek fonksiyonlar (scheduler, ...)
├── lib/            # Kütüphane fonksiyonları
├── mm/             # Memory management
├── net/            # Ağ protokolleri
├── scripts/        # Build scriptleri
├── security/       # Güvenlik modülleri
├── sound/          # Ses sürücüleri
├── tools/          # Yardımcı araçlar
└── Makefile        # Ana Makefile
```

---

## ⚙️ Kernel Konfigürasyonu

### Konfigürasyon Araçları

```bash
# Metin tabanlı (ncurses)
make menuconfig

# Grafiksel (Qt)
make xconfig

# Grafiksel (GTK)
make gconfig

# Varsayılan config
make defconfig

# Mimari default
make ARCH=arm multi_v7_defconfig
```

### .config Dosyası

```bash
# Konfigürasyon dosyası
cat .config | grep CONFIG_GPIO
# CONFIG_GPIOLIB=y
# CONFIG_GPIO_SYSFS=y
# CONFIG_GPIO_OMAP=y
```

### Konfigürasyon Seçenekleri

| Değer | Anlam |
|-------|-------|
| `y` | Kernel'a gömülü |
| `m` | Modül olarak |
| `n` | Dahil edilmez |

---

## 🔨 Kernel Derleme

### Host için Derleme

```bash
# Kaynak kodu indir
git clone --depth=1 https://github.com/torvalds/linux.git
cd linux

# Konfigürasyon
make menuconfig

# Derleme (paralel)
make -j$(nproc)

# Modülleri derle
make modules

# Kurulum
sudo make modules_install
sudo make install
```

### Cross-Compile (ARM için)

```bash
# Ortam değişkenleri
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

# BBB default config
make bb.org_defconfig

# Konfigürasyon (opsiyonel)
make menuconfig

# Derleme
make -j$(nproc) zImage

# Device tree derleme
make dtbs

# Modüller
make modules
```

### Derleme Çıktıları

| Dosya | Konum | Açıklama |
|-------|-------|----------|
| `vmlinux` | `./` | ELF format kernel |
| `zImage` | `arch/arm/boot/` | Sıkıştırılmış kernel |
| `Image` | `arch/arm64/boot/` | ARM64 kernel |
| `*.dtb` | `arch/arm/boot/dts/` | Device tree blob |
| `*.ko` | çeşitli | Kernel modüller |

---

## 📦 Kernel Modülleri

### Modül Listeleme

```bash
# Yüklü modüller
lsmod

# Modül bilgisi
modinfo module_name

# Modül bağımlılıkları
modprobe --show-depends module_name
```

### Modül Yönetimi

```bash
# Modül yükle
sudo insmod mymodule.ko       # Basit yükleme
sudo modprobe module_name      # Bağımlılıklarla

# Modül kaldır
sudo rmmod mymodule
sudo modprobe -r module_name

# Otomatik yükleme
echo "module_name" | sudo tee /etc/modules-load.d/mymodule.conf
```

---

## 🔧 Kernel Parametreleri

### Boot Parametreleri

```bash
# /proc/cmdline
cat /proc/cmdline
# console=ttyS0,115200 root=/dev/mmcblk0p2 ...
```

### Çalışan Sistamde Parametreler

```bash
# /proc/sys dizini
ls /proc/sys/

# Örnek: IP forwarding
cat /proc/sys/net/ipv4/ip_forward

# Değiştir (geçici)
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

# Kalıcı
echo "net.ipv4.ip_forward = 1" | sudo tee /etc/sysctl.d/99-forward.conf
sudo sysctl -p
```

### sysctl ile Yönetim

```bash
# Tüm parametreleri listele
sysctl -a

# Tek parametre
sysctl net.ipv4.ip_forward

# Ayarla
sudo sysctl -w net.ipv4.ip_forward=1
```

---

## 📊 Kernel Bilgileri

```bash
# Kernel versiyonu
uname -r

# Tam bilgi
uname -a

# Kernel config (eğer varsa)
cat /boot/config-$(uname -r)

# Kernel logları
dmesg | tail -50

# Kernel sembolleri
cat /proc/kallsyms | head -20
```

---

## 🎯 Pratik

1. Kernel kaynak kodunu indir
2. `make menuconfig` ile ayarları incele
3. GPIO desteğini kontrol et
4. `lsmod` ile yüklü modülleri listele
5. `/proc/sys` altındaki parametreleri incele

---

*Sonraki ders: Kernel Modülleri Yazma*
