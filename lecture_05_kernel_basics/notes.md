# Ders 5: Linux Kernel Temelleri

> **Konu:** Kernel Yapısı, Konfigürasyon, Derleme ve Detaylı İnceleme

---

## 📋 Bu Derste İşlenenler

- [x] Kernel nedir ve ne yapar?
- [x] Kernel mimarisi ve bileşenleri
- [x] Kernel kaynak yapısı (detaylı)
- [x] Kernel konfigürasyonu
- [x] Kernel derleme (host ve cross-compile)
- [x] Kernel modülleri
- [x] Kernel parametreleri
- [x] Kernel versiyonlama ve güncelleme

---

## 🧠 Kernel Nedir?

**Kernel** = İşletim sisteminin çekirdeği. Donanım ile yazılım arasındaki **tek** bağlantı noktası.

Kernel, bilgisayar açıldığında belleğe yüklenir ve sistem kapanana kadar orada kalır. Tüm donanım erişimleri, bellek yönetimi ve süreç yönetimi kernel tarafından gerçekleştirilir.

```
┌─────────────────────────────────────────────────────────────────┐
│                    KULLANICI UYGULAMALARI                       │
│              (bash, firefox, python, gcc, ...)                   │
└───────────────────────────┬─────────────────────────────────────┘
                            │ System Calls (syscall)
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                        LINUX KERNEL                              │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ Process Management │ Memory Management │ File Systems    │   │
│  ├───────────────────────────────────────────────────────────┤   │
│  │ Device Drivers    │ Network Stack     │ Security        │   │
│  └───────────────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────────────┘
                            │ Hardware Abstraction
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                        DONANIM                                   │
│              (CPU, RAM, Disk, Network, GPIO, ...)                │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Kernel'ın Temel Görevleri

### 1. Process Management (Süreç Yönetimi)

Kernel, sistemde çalışan tüm programları (süreçleri) yönetir:

| Fonksiyon | Açıklama |
|-----------|----------|
| **fork()** | Yeni süreç oluştur |
| **exec()** | Program çalıştır |
| **wait()** | Süreç bekle |
| **exit()** | Süreç sonlandır |
| **kill()** | Sinyal gönder |

**Scheduler (Zamanlayıcı):** Hangi sürecin ne zaman CPU kullanacağını belirler.

```bash
# Çalışan süreçler
ps aux

# Süreç ağacı
pstree

# CPU kullanımı
top
htop
```

### 2. Memory Management (Bellek Yönetimi)

| Kavram | Açıklama |
|--------|----------|
| **Virtual Memory** | Her süreç kendi adres alanına sahip |
| **Paging** | Bellek 4KB sayfalar halinde yönetilir |
| **MMU** | Memory Management Unit (donanım desteği) |
| **Swap** | RAM dolduğunda disk kullanımı |
| **OOM Killer** | Bellek azaldığında süreç öldürme |

```bash
# Bellek durumu
free -h

# Detaylı bellek bilgisi
cat /proc/meminfo

# Süreç bellek haritası
cat /proc/<PID>/maps
```

### 3. Device Drivers (Aygıt Sürücüler)

Kernel, donanımla iletişimi sağlayan aygıt sürücülerini yönetir:

| Tür | Açıklama | Örnek |
|-----|----------|-------|
| **Character** | Byte stream | `/dev/ttyS0`, `/dev/random` |
| **Block** | Blok erişim | `/dev/sda`, `/dev/mmcblk0` |
| **Network** | Ağ arayüzü | `eth0`, `wlan0` |

### 4. File Systems (Dosya Sistemleri)

Linux, birçok dosya sistemini destekler:

| Dosya Sistemi | Kullanım Alanı |
|---------------|----------------|
| **ext4** | Linux root filesystem |
| **FAT32** | Boot partition, SD kart |
| **NTFS** | Windows uyumluluğu |
| **NFS** | Ağ üzerinden dosya paylaşımı |
| **tmpfs** | RAM tabanlı geçici dosyalar |
| **procfs** | `/proc` - süreç bilgileri |
| **sysfs** | `/sys` - kernel/device bilgileri |
| **devtmpfs** | `/dev` - cihaz dosyaları |

```bash
# Mount edilmiş dosya sistemleri
mount | column -t

# Dosya sistemi türleri
cat /proc/filesystems
```

### 5. Networking (Ağ)

TCP/IP protokol yığını kernel içindedir:

```
┌─────────────────────────────────────┐
│ Application Layer  (HTTP, SSH, DNS) │ ← User space
├─────────────────────────────────────┤
│ Transport Layer    (TCP, UDP)       │ ← Kernel
│ Network Layer      (IP, ICMP)       │
│ Link Layer         (Ethernet, WiFi) │
│ Device Drivers     (e1000, r8169)   │
└─────────────────────────────────────┘
```

### 6. Security (Güvenlik)

| Mekanizma | Açıklama |
|-----------|----------|
| **DAC** | Discretionary Access Control (user/group) |
| **SELinux** | Mandatory Access Control |
| **Capabilities** | Root yetkilerini parçalama |
| **Seccomp** | System call filtering |
| **Namespaces** | Kaynak izolasyonu (container) |

---

## 🏗️ Kernel Mimarileri

### Monolitik Kernel (Linux)

```
┌──────────────────────────────────────┐
│           Monolitik Kernel            │
│  ┌────────────────────────────────┐  │
│  │ Process │ Memory │ Filesystem  │  │
│  │ Drivers │ Network│ Security    │  │
│  │    ... HEPSİ TEK PARÇA ...     │  │
│  └────────────────────────────────┘  │
└──────────────────────────────────────┘
         ↕ Donanım
```

**Avantajları:**
- Yüksek performans (function call)
- Düşük latency
- Verimli kaynak kullanımı

**Dezavantajları:**
- Bir hatanın tüm sistemi çökertme riski
- Büyük kod tabanı

### Mikrokernel (Minix, QNX)

```
┌──────────────────────────────────────┐
│ User Space Services                   │
│  ┌───────┐ ┌───────┐ ┌───────┐      │
│  │ File  │ │Network│ │Device │      │
│  │ Sys   │ │Server │ │Driver │      │
│  └───┬───┘ └───┬───┘ └───┬───┘      │
│      │         │         │           │
│      └────────┬┴─────────┘           │
│               │ IPC                   │
│  ┌────────────▼───────────────────┐  │
│  │    Mikrokernel (minimal)        │  │
│  │ (scheduling, IPC, basic MM)     │  │
│  └────────────────────────────────┘  │
└──────────────────────────────────────┘
```

### Hibrit (Linux with Modules)

Linux monolitik yapıdadır ama **loadable kernel modules** ile modülerlik sağlar:

```
┌──────────────────────────────────────┐
│     Loadable Modules (.ko)           │
│  ┌───────┐ ┌───────┐ ┌───────┐      │
│  │ WiFi  │ │ USB   │ │ GPIO  │      │
│  └───┬───┘ └───┬───┘ └───┬───┘      │
│      └─────────┼─────────┘           │
│                ▼                      │
│  ┌────────────────────────────────┐  │
│  │     Core Kernel (vmlinuz)       │  │
│  │ (process, memory, VFS, net...)  │  │
│  └────────────────────────────────┘  │
└──────────────────────────────────────┘
```

---

## 📂 Kernel Kaynak Yapısı (Detaylı)

```
linux/
├── arch/                 # MİMARİ BAĞIMLI KOD
│   ├── arm/              # ARM 32-bit
│   │   ├── boot/         # Boot kodu, zImage
│   │   ├── configs/      # Defconfig dosyaları
│   │   ├── kernel/       # ARM-specific kernel code
│   │   └── mach-*/       # Machine (SoC) specific
│   ├── arm64/            # ARM 64-bit (aarch64)
│   └── x86/              # Intel/AMD x86_64
│
├── block/                # BLOK CIHAZ KATMANI
│   └── blk-core.c        # Block I/O core
│
├── crypto/               # ŞİFRELEME
│   ├── aes_generic.c     # AES algoritması
│   └── sha256_generic.c  # SHA256
│
├── Documentation/        # DOKÜMANTASYON
│   ├── admin-guide/      # Yönetici rehberi
│   ├── driver-api/       # Sürücü API'leri
│   └── devicetree/       # Device Tree
│
├── drivers/              # AYGIT SÜRÜCÜLERİ (en büyük dizin!)
│   ├── gpio/             # GPIO sürücüleri
│   ├── i2c/              # I2C bus ve cihazlar
│   ├── spi/              # SPI sürücüleri
│   ├── net/              # Ağ kartları
│   ├── usb/              # USB sürücüleri
│   ├── mmc/              # SD/MMC kart
│   ├── tty/              # Terminal/serial
│   ├── video/            # Grafik sürücüleri
│   └── ...               # Yüzlerce alt dizin
│
├── fs/                   # DOSYA SİSTEMLERİ
│   ├── ext4/             # ext4 filesystem
│   ├── fat/              # FAT12/16/32
│   ├── nfs/              # Network File System
│   ├── proc/             # /proc filesystem
│   └── sysfs/            # /sys filesystem
│
├── include/              # HEADER DOSYALARI
│   ├── linux/            # Kernel API headers
│   │   ├── module.h      # Module macros
│   │   ├── fs.h          # File operations
│   │   └── gpio.h        # GPIO API
│   └── uapi/             # User-space API headers
│
├── init/                 # KERNEL BAŞLATMA
│   └── main.c            # start_kernel() fonksiyonu
│
├── kernel/               # ÇEKIRDEK FONKSİYONLAR
│   ├── sched/            # Scheduler (zamanlayıcı)
│   ├── fork.c            # Process forking
│   ├── exit.c            # Process exit
│   └── printk.c          # printk implementasyonu
│
├── lib/                  # KÜTÜPHANE FONKSİYONLARİ
│   ├── string.c          # String functions
│   └── vsprintf.c        # sprintf implementation
│
├── mm/                   # MEMORY MANAGEMENT
│   ├── mmap.c            # Memory mapping
│   ├── page_alloc.c      # Page allocator
│   └── oom_kill.c        # OOM killer
│
├── net/                  # AĞ PROTOKOLLERİ
│   ├── ipv4/             # IPv4 stack
│   ├── ipv6/             # IPv6 stack
│   ├── socket.c          # Socket API
│   └── core/             # Network core
│
├── scripts/              # BUILD SCRIPTLERİ
│   ├── kconfig/          # menuconfig araçları
│   └── Makefile.build    # Build rules
│
├── security/             # GÜVENLİK MODÜLLERİ
│   ├── selinux/          # SELinux
│   └── apparmor/         # AppArmor
│
├── sound/                # SES SÜRÜCÜLERİ
│   └── soc/              # System-on-Chip audio
│
├── tools/                # YARDIMCI ARAÇLAR
│   ├── perf/             # Performance analysis
│   └── gpio/             # GPIO test tools
│
├── Kconfig               # Ana konfigürasyon
├── Makefile              # Ana Makefile
└── README                # Açıklama dosyası
```

### Kaynak Kodu İstatistikleri (v6.x)

| Metrik | Değer |
|--------|-------|
| Toplam satır | ~30 milyon |
| C dosyaları | ~30,000 |
| Header dosyaları | ~20,000 |
| En büyük dizin | drivers/ (~%50) |
| Desteklenen mimari | 30+ |

---

## ⚙️ Kernel Konfigürasyonu (Detaylı)

### Konfigürasyon Yöntemleri

```bash
# 1. Mevcut sistemin config'ini kopyala
cp /boot/config-$(uname -r) .config

# 2. Varsayılan config (mimari için)
make defconfig

# 3. ARM için varsayılan
make ARCH=arm multi_v7_defconfig

# 4. BBB için
make ARCH=arm bb.org_defconfig

# 5. Tüm seçenekleri varsayılan yap
make olddefconfig

# 6. Interaktif düzenleme
make menuconfig    # ncurses (terminal)
make nconfig       # ncurses (modern)
make xconfig       # Qt (grafik)
make gconfig       # GTK (grafik)
```

### menuconfig Navigasyonu

```
┌───────────── Linux Kernel Configuration ──────────────┐
│ Arrow keys navigate │ Enter selects │ ? for help      │
│ Space toggles │ Esc exits                             │
├───────────────────────────────────────────────────────┤
│   General setup  --->                                 │
│   [*] Enable loadable module support  --->            │
│   -*- Enable the block layer  --->                    │
│       Processor type and features  --->               │
│       Power management and ACPI options  --->         │
│   [*] Networking support  --->                        │
│       Device Drivers  --->                            │
│       File systems  --->                              │
│       Security options  --->                          │
│   -*- Cryptographic API  --->                         │
│       Kernel hacking  --->                            │
└───────────────────────────────────────────────────────┘
```

**Kısayollar:**
- `/` : Arama
- `?` : Yardım
- `Y` : Dahil et (built-in)
- `M` : Modül olarak
- `N` : Dahil etme
- `Space` : Toggle

### .config Dosyası Formatı

```bash
# .config dosyası örneği

# Yorum satırı
# CONFIG_DEBUG_INFO is not set

# Dahil (built-in)
CONFIG_MODULES=y

# Modül olarak
CONFIG_EXT4_FS=m

# String değer
CONFIG_DEFAULT_HOSTNAME="(none)"

# Sayısal değer
CONFIG_LOG_BUF_SHIFT=17
```

### Önemli Konfigürasyon Seçenekleri

| Kategori | Seçenek | Açıklama |
|----------|---------|----------|
| **Modüller** | `CONFIG_MODULES` | Modül desteği |
| **GPIO** | `CONFIG_GPIO_SYSFS` | Sysfs GPIO arayüzü |
| **I2C** | `CONFIG_I2C_CHARDEV` | I2C user-space erişimi |
| **SPI** | `CONFIG_SPI_SPIDEV` | SPI user-space erişimi |
| **Debug** | `CONFIG_DEBUG_INFO` | Debug sembolleri |
| **Printk** | `CONFIG_PRINTK` | Kernel mesajları |

---

## 🔨 Kernel Derleme (Adım Adım)

### Bağımlılıklar

```bash
# Ubuntu/Debian
sudo apt install build-essential libncurses-dev bison flex \
    libssl-dev libelf-dev bc dwarves

# Kernel headers (mevcut kernel için)
sudo apt install linux-headers-$(uname -r)
```

### Host için Derleme

```bash
# 1. Kaynak kodu indir
git clone --depth=1 https://github.com/torvalds/linux.git
cd linux

# 2. Mevcut config'i kullan
cp /boot/config-$(uname -r) .config

# 3. Yeni seçenekleri varsayılan yap
make olddefconfig

# 4. İsteğe bağlı: düzenle
make menuconfig

# 5. Derle (çekirdek sayısı kadar paralel)
make -j$(nproc)

# 6. Modülleri derle
make modules

# 7. Kurulum
sudo make modules_install
sudo make install

# 8. Grub güncelle
sudo update-grub

# 9. Yeniden başlat ve test et
sudo reboot
```

### Cross-Compile (BBB için)

```bash
# 1. Ortam değişkenleri
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

# 2. BBB kaynak kodu
git clone https://github.com/beagleboard/linux.git -b 5.10
cd linux

# 3. BBB defconfig
make bb.org_defconfig

# 4. Düzenleme (opsiyonel)
make menuconfig

# 5. Derleme
make -j$(nproc) zImage        # Kernel imajı
make -j$(nproc) dtbs          # Device tree
make -j$(nproc) modules       # Modüller

# 6. Çıktıları incele
ls arch/arm/boot/zImage
ls arch/arm/boot/dts/am335x-boneblack.dtb
```

### Cross-Compile (RPi 64-bit için)

```bash
# 1. Ortam değişkenleri
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

# 2. RPi kaynak kodu
git clone https://github.com/raspberrypi/linux.git --depth=1
cd linux

# 3. RPi4 defconfig
make bcm2711_defconfig

# 4. Derleme
make -j$(nproc) Image
make -j$(nproc) dtbs
make -j$(nproc) modules

# 5. Çıktılar
ls arch/arm64/boot/Image
ls arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb
```

### Derleme Çıktıları (Detaylı)

| Dosya | Konum | Açıklama |
|-------|-------|----------|
| `vmlinux` | `./` | Sıkıştırılmamış ELF, debug için |
| `System.map` | `./` | Kernel sembol haritası |
| `zImage` | `arch/arm/boot/` | Sıkıştırılmış 32-bit kernel |
| `Image` | `arch/arm64/boot/` | 64-bit kernel |
| `bzImage` | `arch/x86/boot/` | x86 sıkıştırılmış kernel |
| `*.dtb` | `arch/*/boot/dts/` | Derlenmiş device tree |
| `*.ko` | çeşitli dizinler | Kernel modüller |
| `.config` | `./` | Kullanılan konfigürasyon |

---

## 📦 Kernel Modülleri (Detaylı)

### Modül Nedir?

Modül = Çalışan kernel'a **dinamik olarak** yüklenebilen/kaldırılabilen kod parçası.

**Avantajları:**
- Kernel boyutunu küçük tutar
- Gerektiğinde yüklenir
- Reboot gerektirmez
- Geliştirme kolaylığı

### Modül Komutları

```bash
# Yüklü modülleri listele
lsmod

# Modül bilgisi
modinfo <modül_adı>
modinfo /path/to/module.ko

# Modül yükle (basit)
sudo insmod mymodule.ko

# Modül yükle (bağımlılıklarla)
sudo modprobe module_name

# Modül kaldır
sudo rmmod mymodule
sudo modprobe -r module_name

# Bağımlılıkları göster
modprobe --show-depends module_name

# Modül veritabanını güncelle
sudo depmod -a
```

### Modül Konumları

```bash
# Kurulu modüller
/lib/modules/$(uname -r)/
├── kernel/           # Kernel ile gelen modüller
│   ├── drivers/
│   ├── fs/
│   └── net/
├── extra/            # Ekstra modüller
├── updates/          # Güncellenmiş modüller
├── modules.dep       # Bağımlılık dosyası
├── modules.alias     # Alias eşlemeleri
└── modules.symbols   # Export edilen semboller
```

### Otomatik Modül Yükleme

```bash
# Açılışta yükle
echo "mymodule" | sudo tee /etc/modules-load.d/mymodule.conf

# Kara listeye al (yükleme)
echo "blacklist module_name" | sudo tee /etc/modprobe.d/blacklist.conf

# Modül parametresi
echo "options mymodule param=value" | sudo tee /etc/modprobe.d/mymodule.conf
```

---

## 🔧 Kernel Parametreleri (Detaylı)

### 1. Boot Parametreleri (Kernel Command Line)

GRUB veya bootloader tarafından kernel'a geçirilir:

```bash
# Mevcut parametreleri gör
cat /proc/cmdline
# BOOT_IMAGE=/boot/vmlinuz-6.1.0 root=/dev/sda1 ro quiet

# GRUB'da düzenleme
sudo nano /etc/default/grub
# GRUB_CMDLINE_LINUX="console=ttyS0,115200"
sudo update-grub
```

**Yaygın Boot Parametreleri:**

| Parametre | Açıklama |
|-----------|----------|
| `root=/dev/sda1` | Root filesystem |
| `ro` / `rw` | Read-only / Read-write mount |
| `quiet` | Sessiz boot |
| `debug` | Debug mesajları |
| `console=ttyS0,115200` | Seri konsol |
| `init=/bin/bash` | Init yerine bash başlat |
| `single` | Single user mode |
| `mem=512M` | Kullanılabilir RAM sınırla |
| `nosmp` | Tek CPU kullan |

### 2. Runtime Parametreleri (/proc/sys)

```bash
# Kategori yapısı
/proc/sys/
├── kernel/     # Kernel parametreleri
├── net/        # Ağ parametreleri
├── vm/         # Bellek parametreleri
├── fs/         # Dosya sistemi
└── dev/        # Device parametreleri

# Örnek: IP forwarding
cat /proc/sys/net/ipv4/ip_forward
echo 1 > /proc/sys/net/ipv4/ip_forward  # Geçici

# Kalıcı ayar
echo "net.ipv4.ip_forward = 1" >> /etc/sysctl.conf
sysctl -p
```

### 3. sysctl Kullanımı

```bash
# Tüm parametreleri listele
sysctl -a

# Filtreleme
sysctl -a | grep tcp

# Tek parametre oku
sysctl net.ipv4.ip_forward

# Parametre ayarla
sudo sysctl -w net.ipv4.ip_forward=1

# Dosyadan yükle
sudo sysctl -p /etc/sysctl.d/99-custom.conf
```

### Önemli sysctl Parametreleri

| Parametre | Varsayılan | Açıklama |
|-----------|------------|----------|
| `kernel.hostname` | - | Sistem adı |
| `kernel.panic` | 0 | Panic sonrası reboot süresi |
| `vm.swappiness` | 60 | Swap kullanım eğilimi |
| `net.ipv4.ip_forward` | 0 | IP yönlendirme |
| `fs.file-max` | - | Maksimum açık dosya |

---

## 📊 Kernel Bilgileri ve Monitoring

### Kernel Versiyonu

```bash
# Versiyon
uname -r
# 6.1.0-123-generic

# Detaylı bilgi
uname -a
# Linux hostname 6.1.0-123-generic #1 SMP ... x86_64 GNU/Linux

# Versiyon numaraları anlamı
# 6.1.0-123-generic
# │ │ │  │    └── Dağıtım eki
# │ │ │  └── Minor/patch
# │ │ └── Patch level
# │ └── Minor version
# └── Major version
```

### Kernel Logları

```bash
# dmesg - kernel ring buffer
dmesg
dmesg | tail -50
dmesg -w                # Canlı takip
dmesg -T                # Okunabilir zaman
dmesg -l err            # Sadece hatalar
dmesg -c                # Oku ve temizle

# journalctl (systemd)
journalctl -k           # Kernel logları
journalctl -k -f        # Canlı takip
journalctl -k -p err    # Sadece hatalar
journalctl -k --since "1 hour ago"

# Geleneksel log dosyaları
cat /var/log/kern.log
cat /var/log/dmesg
```

### /proc Filesystem

```bash
# Kernel bilgileri
cat /proc/version       # Kernel versiyon detayı
cat /proc/cmdline       # Boot parametreleri
cat /proc/cpuinfo       # CPU bilgisi
cat /proc/meminfo       # Bellek bilgisi
cat /proc/interrupts    # Interrupt istatistikleri
cat /proc/ioports       # I/O port tahsisleri
cat /proc/iomem         # Memory-mapped I/O

# Süreç bilgileri
cat /proc/<PID>/status  # Süreç durumu
cat /proc/<PID>/maps    # Bellek haritası
cat /proc/<PID>/fd/     # Açık dosyalar
```

### /sys Filesystem

```bash
# Cihaz bilgileri
ls /sys/class/          # Cihaz sınıfları
ls /sys/devices/        # Cihaz ağacı
ls /sys/module/         # Yüklü modüller
ls /sys/bus/            # Bus türleri

# GPIO örneği
ls /sys/class/gpio/
echo 60 > /sys/class/gpio/export
cat /sys/class/gpio/gpio60/value
```

---

## 🎯 Pratik Ödevler

### Temel
1. `uname -a` ile kernel bilgilerini görüntüle
2. `lsmod` ile yüklü modülleri listele
3. `dmesg | tail -30` ile son kernel mesajlarını incele
4. `/proc/cpuinfo` içeriğini incele

### Orta
1. Kernel kaynak kodunu indir (`git clone`)
2. `make menuconfig` ile konfigürasyonu incele
3. CONFIG_GPIO_SYSFS seçeneğini bul
4. Konfigürasyonu `.config` dosyasında incele

### İleri
1. Kernel'ı cross-compile et (BBB veya RPi için)
2. zImage ve dtb dosyalarını target'a kopyala
3. Yeni kernel ile boot et
4. `uname -r` ile doğrula

---

## 📚 Kaynaklar

- [Linux Kernel Documentation](https://docs.kernel.org/)
- [The Linux Kernel Archives](https://kernel.org/)
- [Linux Device Drivers (LDD3)](https://lwn.net/Kernel/LDD3/) - Ücretsiz online
- [Bootlin Kernel Training](https://bootlin.com/doc/training/)
- [kernel.org Coding Style](https://docs.kernel.org/process/coding-style.html)

---

## ❓ Sık Sorulan Sorular

**S: Kernel güncellemesi tehlikeli mi?**
A: Dikkatli yapılmalı. Önce mevcut kernel'ı yedekle, boot menu'de eski kernel seçeneği kal.

**S: Hangi kernel versiyonunu seçmeliyim?**
A: LTS (Long Term Support) versiyonları önerilir: 5.4, 5.10, 5.15, 6.1

**S: Kernel panic ne demek?**
A: Kernel'ın kurtarılamaz bir hata ile karşılaşması. Sistem durur.

**S: Modül mü yoksa built-in mı seçmeliyim?**
A: Sürekli kullanılacaksa built-in (y), nadiren kullanılacaksa modül (m).

---

*Sonraki ders: Kernel Modülleri Yazma (Ders 6)*
