# Ders 3: Boot Süreci ve Bootloader

> **Konu:** Linux Boot Süreci, U-Boot, Device Tree

---

## 📋 Bu Derste İşlenenler

- [x] Boot süreci aşamaları
- [x] Bootloader nedir ve neden gereklidir?
- [x] U-Boot detaylı inceleme
- [x] U-Boot derleme ve kurulum
- [x] U-Boot komutları ve scripting
- [x] Kernel yükleme yöntemleri
- [x] Device Tree kavramı
- [x] Init sistemleri

---

## 🔄 Boot Süreci Aşamaları

Bir gömülü Linux sistemi açıldığında, güçten kullanıcı oturumuna kadar birçok aşamadan geçer. Bu süreç, her bir aşamanın bir sonrakini hazırladığı zincirleme bir yapıdadır. Her aşama, sistemin daha karmaşık işlemleri yapabilmesi için gerekli başlatma işlemlerini gerçekleştirir.

```
┌─────────────────────────────────────────────────────────────┐
│                    POWER ON / RESET                         │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  1. ROM Bootloader (SoC içinde, değiştirilemez)             │
│     - CPU initialize                                        │
│     - Boot source seç (SD, eMMC, USB, TFTP...)             │
│     - SPL/MLO yükle                                        │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  2. SPL/MLO (Secondary Program Loader)                      │
│     - RAM initialize                                        │
│     - U-Boot'u RAM'e yükle                                 │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  3. U-Boot (Universal Bootloader)                           │
│     - Donanım init (ethernet, USB, display...)             │
│     - Kernel + DTB yükle                                   │
│     - Boot argümanları hazırla                             │
│     - Kontrolü kernel'a devret                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  4. Linux Kernel                                            │
│     - Kendini decompress et                                │
│     - Donanımı init et (DTB'den)                           │
│     - Root filesystem mount et                             │
│     - Init process başlat (PID 1)                          │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  5. Init System (systemd / sysvinit / busybox init)         │
│     - Servisleri başlat                                    │
│     - Login prompt                                         │
└─────────────────────────────────────────────────────────────┘
```

### Aşama 1: ROM Bootloader

Sistem açıldığında ilk çalışan kod, işlemcinin içindeki ROM (Read-Only Memory) bellekte bulunan bootloader kodudur. Bu kod üretici tarafından yazılmıştır ve değiştirilemez. ROM bootloader'ın görevi, hangi ortamdan (SD kart, eMMC, USB, ağ) boot edileceğini belirlemek ve o ortamdaki ikincil bootloader'ı (SPL) yüklemektir.

BeagleBone Black örneğinde, AM335x işlemcisinin ROM bootloader'ı boot düğmesine basılıp basılmadığını kontrol eder. Düğmeye basılıysa SD karttan, basılı değilse eMMC'den boot edilir. ROM bootloader çok küçüktür ve RAM'i bile başlatamaz; bu yüzden SPL'e ihtiyaç duyar.

### Aşama 2: SPL (Secondary Program Loader)

SPL, RAM'i başlatmaktan sorumlu olan küçük bir bootloader'dır. İşlemcinin dahili SRAM'inde (genellikle 64-128KB) çalışır çünkü henüz ana RAM kullanılamaz durumdadır. SPL'in ana görevi, DDR bellek kontrolcüsünü yapılandırarak ana RAM'i kullanılabilir hale getirmek ve ardından ana bootloader'ı (U-Boot) bu RAM'e yüklemektir. BeagleBone Black'te SPL dosyası "MLO" olarak adlandırılır.

### Aşama 3: U-Boot

U-Boot (Universal Bootloader), gömülü Linux sistemlerinde en yaygın kullanılan bootloader'dır. SPL tarafından RAM'e yüklendikten sonra çalışmaya başlar. U-Boot, tüm donanımı başlatır (Ethernet, USB, ekran, depolama cihazları), Linux kernel'ını ve device tree'yi belleğe yükler, boot argümanlarını hazırlar ve kontrolü kernel'a devreder.

### Aşama 4: Linux Kernel

Kernel, U-Boot'tan kontrolü aldıktan sonra önce kendini açar (sıkıştırılmış ise), ardından device tree'yi okuyarak donanım yapılandırmasını öğrenir. Sürücüleri yükler, root dosya sistemini mount eder ve son olarak init sürecini başlatır.

### Aşama 5: Init Sistemi

Init, çekirdek tarafından başlatılan ilk kullanıcı alanı sürecidir (PID 1). Tüm diğer süreçlerin ebeveynidir. Modern sistemlerde genellikle systemd, daha basit sistemlerde SysVinit veya BusyBox init kullanılır.

---

## 🔧 U-Boot Nedir?

U-Boot (Universal Bootloader), das U-Boot olarak da bilinen, gömülü sistemler için geliştirilmiş açık kaynaklı bir bootloader'dır. 1999 yılında Wolfgang Denk tarafından geliştirmeye başlanmış ve o zamandan beri gömülü Linux dünyasının standart bootloader'ı haline gelmiştir.

### U-Boot'un Önemi

Bir gömülü sistemde bootloader, donanım ile işletim sistemi arasında köprü görevi görür. U-Boot olmadan, Linux kernel'ı hangi adreste olduğunu, hangi RAM miktarına sahip olduğunu, hangi seri port'tan konsol çıktısı vereceğini bilemez. U-Boot tüm bu bilgileri kernel'a boot argümanları ve device tree aracılığıyla iletir.

U-Boot'un interaktif konsolu, sistem geliştiricileri için vazgeçilmezdir. Seri port üzerinden bu konsola erişerek kernel'ı farklı kaynaklardan (SD kart, eMMC, USB, ağ) yükleyebilir, boot parametrelerini değiştirebilir ve sistemi test edebilirsiniz. Geliştirme aşamasında bu esneklik hayati önem taşır.

### U-Boot Özellikleri

U-Boot, çok geniş bir donanım yelpazesini destekler. ARM, x86, MIPS, PowerPC, RISC-V gibi farklı işlemci mimarilerinde çalışabilir. SD kart, eMMC, NAND flash, NOR flash, USB, TFTP, NFS gibi farklı boot kaynaklarını destekler.

Scripting desteği sayesinde karmaşık boot senaryoları oluşturabilirsiniz. Örneğin, önce SD karttan boot etmeyi dene, başarısız olursa eMMC'den boot et gibi fallback mekanizmaları kurabilirsiniz. U-Boot'un kendi script dili basit ama güçlüdür.

GPL lisansı altında dağıtılır ve kaynak kodu tamamen açıktır. Bu, özel donanımınız için U-Boot'u değiştirebileceğiniz ve özelleştirebileceğiniz anlamına gelir.

---

## 🛠️ U-Boot Derleme

U-Boot'u kaynak koddan derlemek, özellikle özel donanımlar için gereklidir. Derleme süreci, cross-compilation araçlarını gerektirir çünkü genellikle x86 bir bilgisayarda ARM için derleme yaparsınız.

### Kaynak Kodun İndirilmesi

```bash
# Resmi U-Boot repository'sini klonla
git clone https://github.com/u-boot/u-boot.git
cd u-boot

# Kararlı bir sürümü seç
git checkout v2024.01

# Mevcut konfigürasyonları listele
ls configs/ | grep am335x
# am335x_boneblack_defconfig
# am335x_evm_defconfig
```

### BBB için Derleme

BeagleBone Black için U-Boot derlemek istiyorsanız, ARM cross-compiler'ın kurulu olması gerekir. Derleme işlemi, konfigürasyon dosyasını seçmekle başlar ve ardından make komutuyla tamamlanır.

```bash
# Ortam değişkenlerini ayarla
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm

# BBB konfigürasyonunu yükle
make am335x_boneblack_defconfig

# Konfigürasyonu özelleştir (opsiyonel)
make menuconfig

# Derle
make -j$(nproc)

# Çıktılar
ls -la MLO u-boot.img
# MLO = SPL (Secondary Program Loader)
# u-boot.img = Ana U-Boot imajı
```

### Raspberry Pi için Derleme

Raspberry Pi, kendi bootloader'ını (VideoCore GPU tarafından çalıştırılan) kullanır, ancak U-Boot da kullanılabilir. RPi için U-Boot derlemek biraz farklıdır çünkü RPi'nin firmware'i önce U-Boot'u yükler, sonra U-Boot kernel'ı yükler.

```bash
# RPi 4 için (64-bit)
export CROSS_COMPILE=aarch64-linux-gnu-
export ARCH=arm64

make rpi_4_defconfig
make -j$(nproc)
```

---

## 💻 U-Boot Konsoluna Erişim

U-Boot konsoluna erişmek için seri port bağlantısı gereklidir. BeagleBone Black'te bu bağlantı debug header üzerinden, Raspberry Pi'de GPIO pinleri üzerinden yapılır. USB-TTL dönüştürücü kablosu ile bilgisayarınıza bağlanabilirsiniz.

### Seri Port Ayarları

U-Boot varsayılan olarak 115200 baud rate ile iletişim kurar. Linux'ta seri porta bağlanmak için çeşitli araçlar kullanabilirsiniz:

```bash
# minicom ile bağlan
sudo minicom -D /dev/ttyUSB0 -b 115200

# screen ile bağlan
screen /dev/ttyUSB0 115200

# picocom ile bağlan
picocom -b 115200 /dev/ttyUSB0
```

Sistem açıldığında, U-Boot bir geri sayım başlatır (varsayılan 3 saniye). Bu süre içinde herhangi bir tuşa basarsanız, U-Boot konsolu açılır ve interaktif mod aktif olur. Tuşa basmazsanız, U-Boot otomatik olarak boot işlemini başlatır.

### U-Boot Prompt

U-Boot konsolu açıldığında aşağıdaki gibi bir prompt göreceksiniz:

```
U-Boot 2024.01 (Jan 15 2024 - 12:34:56 +0300)

CPU  : AM335X-GP rev 2.1
Model: TI AM335x BeagleBone Black
DRAM:  512 MiB
...

Hit any key to stop autoboot:  3...2...1...0
=>
```

`=>` işareti U-Boot'un komut beklediğini gösterir.

---

## 📋 U-Boot Komutları

U-Boot zengin bir komut setine sahiptir. Bu komutları kullanarak bellek okuma/yazma, dosya yükleme, boot işlemi başlatma ve daha birçok işlem yapabilirsiniz.

### Temel Komutlar

```bash
# Yardım
=> help              # Tüm komutları listele
=> help bootm        # Belirli komut hakkında yardım

# Ortam değişkenleri
=> printenv          # Tüm değişkenleri göster
=> printenv bootargs # Tek değişken göster
=> setenv foo bar    # Değişken ayarla
=> saveenv           # Değişkenleri kalıcı kaydet

# Bellek işlemleri
=> md 0x80000000 100       # Bellek dump (256 byte)
=> mw 0x80000000 0xdeadbeef # Belleğe yaz
=> mm 0x80000000           # Interaktif bellek düzenleme
```

### Dosya Yükleme Komutları

U-Boot, farklı kaynaklardan dosya yüklemek için farklı komutlar kullanır. En yaygın kullanılanlar:

```bash
# MMC (SD kart/eMMC) işlemleri
=> mmc list                    # Mevcut MMC cihazlarını listele
=> mmc dev 0                   # MMC 0'ı seç (SD kart)
=> mmc dev 1                   # MMC 1'i seç (eMMC)
=> mmc info                    # Seçili cihaz bilgisi
=> mmc part                    # Bölüm tablosunu göster

# FAT dosya sistemi
=> fatls mmc 0:1               # FAT bölümünü listele
=> fatload mmc 0:1 0x82000000 zImage     # Kernel yükle
=> fatload mmc 0:1 0x88000000 am335x-boneblack.dtb  # DTB yükle

# ext4 dosya sistemi
=> ext4ls mmc 0:2              # ext4 bölümünü listele
=> ext4load mmc 0:2 0x82000000 /boot/zImage

# TFTP (ağ üzerinden)
=> setenv serverip 192.168.1.1   # TFTP sunucu IP
=> setenv ipaddr 192.168.1.100   # Cihaz IP
=> tftp 0x82000000 zImage        # Kernel'ı TFTP ile yükle
```

### Boot Komutları

Kernel ve device tree belleğe yüklendikten sonra boot komutu ile sistem başlatılır:

```bash
# zImage için (sıkıştırılmış kernel)
=> bootz 0x82000000 - 0x88000000
#        ^kernel     ^ramdisk(yok)  ^dtb

# uImage için (legacy format)
=> bootm 0x82000000 - 0x88000000

# Linux kernel'ı doğrudan boot et
=> booti 0x82000000 - 0x88000000  # ARM64 için
```

Tire (-) işareti, initramfs/ramdisk kullanılmadığını belirtir.

---

## ⚙️ U-Boot Ortam Değişkenleri

U-Boot, davranışını kontrol etmek için ortam değişkenleri kullanır. Bu değişkenler flash bellekte saklanır ve yeniden başlatmalarda korunur ama istenirse RAM'de de tutulabilir.

### Kritik Değişkenler

En önemli ortam değişkenleri şunlardır:

| Değişken | Açıklama | Örnek Değer |
|----------|----------|-------------|
| `bootcmd` | Otomatik boot komutu | `run mmcboot` |
| `bootargs` | Kernel'a geçirilen argümanlar | `console=ttyO0,115200 root=/dev/mmcblk0p2` |
| `bootdelay` | Boot öncesi bekleme süresi (saniye) | `3` |
| `ipaddr` | Cihaz IP adresi | `192.168.1.100` |
| `serverip` | TFTP sunucu IP adresi | `192.168.1.1` |
| `loadaddr` | Kernel yükleme adresi | `0x82000000` |
| `fdtaddr` | Device tree yükleme adresi | `0x88000000` |

### bootargs Detaylı

bootargs değişkeni, kernel'a geçirilen komut satırı parametrelerini içerir. Bu parametreler kernel'ın nasıl davranacağını belirler:

```bash
# Tipik bootargs
setenv bootargs 'console=ttyO0,115200n8 root=/dev/mmcblk0p2 rootfstype=ext4 rootwait'

# Parçaların anlamları:
# console=ttyO0,115200n8  → Konsol çıktısı seri port 0'dan, 115200 baud, 8-N-1
# root=/dev/mmcblk0p2     → Root filesystem ikinci MMC bölümünde
# rootfstype=ext4         → Filesystem tipi ext4
# rootwait                → Root cihazı hazır olana kadar bekle
```

### Özel Boot Scriptleri

Karmaşık boot senaryoları için script değişkenleri oluşturabilirsiniz:

```bash
# SD karttan boot scripti
setenv mmcboot 'mmc dev 0; fatload mmc 0:1 ${loadaddr} zImage; fatload mmc 0:1 ${fdtaddr} am335x-boneblack.dtb; bootz ${loadaddr} - ${fdtaddr}'

# Ağdan boot scripti
setenv netboot 'dhcp; tftp ${loadaddr} zImage; tftp ${fdtaddr} am335x-boneblack.dtb; bootz ${loadaddr} - ${fdtaddr}'

# Kaydet
saveenv

# Çalıştır
run mmcboot
```

---

## 📡 Ağ Üzerinden Boot (TFTP)

Geliştirme sırasında her seferinde SD kartı çıkarıp takmak yorucudur. TFTP ile ağ üzerinden boot, geliştirme sürecini büyük ölçüde hızlandırır. Kernel ve device tree dosyalarını bilgisayarınızdaki TFTP sunucusuna koyarsınız ve U-Boot bunları ağ üzerinden yükler.

### Host'ta TFTP Sunucu Kurulumu

```bash
# TFTP sunucu kur
sudo apt install tftpd-hpa

# Yapılandırma
sudo nano /etc/default/tftpd-hpa
# TFTP_DIRECTORY="/srv/tftp"
# TFTP_OPTIONS="--secure --create"

# Dizin izinleri
sudo mkdir -p /srv/tftp
sudo chmod 777 /srv/tftp

# Servisi yeniden başlat
sudo systemctl restart tftpd-hpa

# Test dosyalarını kopyala
sudo cp zImage am335x-boneblack.dtb /srv/tftp/
```

### U-Boot'ta Ağ Yapılandırması

```bash
# IP ayarları
=> setenv ipaddr 192.168.1.100      # Cihaz IP
=> setenv serverip 192.168.1.1      # TFTP sunucu IP
=> setenv gatewayip 192.168.1.1     # Gateway
=> setenv netmask 255.255.255.0     # Netmask

# DHCP kullan (opsiyonel)
=> dhcp

# Bağlantı testi
=> ping 192.168.1.1

# TFTP ile dosya yükle
=> tftp ${loadaddr} zImage
=> tftp ${fdtaddr} am335x-boneblack.dtb

# Boot et
=> bootz ${loadaddr} - ${fdtaddr}
```

### NFS Root Filesystem

TFTP ile sadece kernel ve DTB yüklenebilir. Root filesystem için NFS kullanılabilir. Bu, geliştirme sırasında dosya sisteminde yapılan değişikliklerin anında yansımasını sağlar:

```bash
# Host'ta NFS sunucu kur
sudo apt install nfs-kernel-server

# /etc/exports
echo "/srv/nfs/rootfs 192.168.1.0/24(rw,sync,no_subtree_check,no_root_squash)" | sudo tee -a /etc/exports

# Servisi yeniden başlat
sudo exportfs -a
sudo systemctl restart nfs-kernel-server

# U-Boot bootargs
setenv bootargs 'console=ttyO0,115200 root=/dev/nfs nfsroot=192.168.1.1:/srv/nfs/rootfs,nfsvers=3 ip=dhcp'
```

---

## 🌳 Device Tree (DT)

Device Tree, donanım yapılandırmasını tanımlayan hiyerarşik bir veri yapısıdır. Eskiden kernel kaynak koduna gömülü olan donanım bilgileri, artık device tree ile ayrı bir dosyada tutulur. Bu sayede aynı kernel farklı board'larda çalışabilir.

### Device Tree Neden Önemlidir?

ARM dünyasında yüzlerce farklı geliştirme kartı vardır. Her birinin GPIO pin düzeni, peripheral adresleri, clock yapılandırması farklıdır. Kernel'ı her board için yeniden derlemek yerine, sadece device tree dosyasını değiştirmek yeterlidir.

Device tree, kernel'a şunları söyler:
- Hangi CPU'lar var ve özellikleri neler
- RAM'in adresi ve boyutu
- Hangi peripheral'lar var (UART, I2C, SPI, GPIO)
- Bu peripheral'ların bellek adresleri
- Interrupt numaraları ve bağlantıları
- Clock frekansları ve hiyerarşisi

### DT Dosya Türleri

| Uzantı | Tam Adı | Açıklama |
|--------|---------|----------|
| `.dts` | Device Tree Source | İnsan okunabilir kaynak dosyası |
| `.dtsi` | Device Tree Source Include | Diğer DTS dosyaları tarafından include edilen dosya |
| `.dtb` | Device Tree Blob | Derlenmiş binary format |
| `.dtbo` | Device Tree Blob Overlay | Runtime'da uygulanan değişiklikler |

### DT Syntax

Device tree syntax'ı JSON benzeri bir yapıya sahiptir. Her düğüm (node) bir cihazı veya bir alt sistemi temsil eder:

```dts
/dts-v1/;

/ {
    /* Root düğüm özellikleri */
    model = "BeagleBone Black";
    compatible = "ti,am335x-bone-black", "ti,am335x-bone", "ti,am33xx";
    
    /* CPU düğümü */
    cpus {
        cpu@0 {
            compatible = "arm,cortex-a8";
            device_type = "cpu";
            reg = <0>;
            operating-points-v2 = <&cpu0_opp_table>;
        };
    };
    
    /* Bellek düğümü */
    memory@80000000 {
        device_type = "memory";
        reg = <0x80000000 0x20000000>; /* 512MB @ 0x80000000 */
    };
    
    /* LED örneği */
    leds {
        compatible = "gpio-leds";
        
        led0 {
            label = "beaglebone:green:usr0";
            gpios = <&gpio1 21 GPIO_ACTIVE_HIGH>;
            linux,default-trigger = "heartbeat";
        };
        
        led1 {
            label = "beaglebone:green:usr1";
            gpios = <&gpio1 22 GPIO_ACTIVE_HIGH>;
            linux,default-trigger = "mmc0";
        };
    };
    
    /* I2C controller */
    i2c0: i2c@44e0b000 {
        compatible = "ti,omap4-i2c";
        reg = <0x44e0b000 0x1000>;
        interrupts = <70>;
        clock-frequency = <400000>;
        status = "okay";
        
        /* I2C üzerindeki cihaz */
        eeprom@50 {
            compatible = "atmel,24c256";
            reg = <0x50>;
        };
    };
};
```

### DT Derleme

```bash
# DTS → DTB derleme
dtc -I dts -O dtb -o output.dtb input.dts

# DTB → DTS (decompile)
dtc -I dtb -O dts -o output.dts input.dtb

# Kernel kaynak ağacında
make dtbs

# Belirli bir DTB derle
make am335x-boneblack.dtb
```

### Device Tree Overlays

Overlays, mevcut device tree'ye runtime'da değişiklik yapmanızı sağlar. GPIO pinlerini farklı amaçlarla kullanmak istediğinizde overlay'ler kullanılır:

```dts
/* Örnek overlay: SPI'ı etkinleştir */
/dts-v1/;
/plugin/;

&spi0 {
    status = "okay";
    
    spidev@0 {
        compatible = "spidev";
        reg = <0>;
        spi-max-frequency = <24000000>;
    };
};
```

---

## ⚙️ Boot Argümanları

Kernel'a geçirilen komut satırı argümanları, kernel'ın davranışını kontrol eder. Bu argümanlar U-Boot'un bootargs değişkeninde saklanır.

### Yaygın Boot Argümanları

| Argüman | Açıklama | Örnek |
|---------|----------|-------|
| `console=` | Konsol cihazı | `console=ttyS0,115200n8` |
| `root=` | Root filesystem | `root=/dev/mmcblk0p2` |
| `rootfstype=` | Filesystem türü | `rootfstype=ext4` |
| `rootwait` | Root cihazı bekle | (parametre yok) |
| `ro` | Read-only mount | (parametre yok) |
| `rw` | Read-write mount | (parametre yok) |
| `init=` | Init programı | `init=/bin/bash` |
| `quiet` | Sessiz boot | (parametre yok) |
| `debug` | Debug mesajları | (parametre yok) |
| `loglevel=` | Kernel log seviyesi | `loglevel=7` |

### Örnek Boot Argümanları

```bash
# Normal boot
console=ttyO0,115200n8 root=/dev/mmcblk0p2 rootfstype=ext4 rootwait

# Debug boot
console=ttyO0,115200n8 root=/dev/mmcblk0p2 rootwait debug loglevel=7

# Initramfs ile boot
console=ttyO0,115200n8 root=/dev/ram0 initrd=0x88080000

# NFS root için
console=ttyO0,115200n8 root=/dev/nfs nfsroot=192.168.1.1:/srv/nfs/rootfs ip=dhcp
```

---

## 🏃 Init Sistemleri

Kernel, root filesystem'i mount ettikten sonra ilk kullanıcı alanı sürecini (PID 1) başlatır. Bu süreç "init" olarak adlandırılır ve tüm diğer süreçlerin ebeveynidir.

### systemd (Modern)

systemd, Linux dünyasının modern init sistemidir. Paralel servis başlatma, bağımlılık yönetimi, socket aktivasyonu gibi gelişmiş özellikler sunar.

```bash
# Servis durumu
systemctl status sshd

# Servisi başlat/durdur
sudo systemctl start sshd
sudo systemctl stop sshd

# Boot'ta otomatik başlat
sudo systemctl enable sshd

# Log görüntüleme
journalctl -u sshd
journalctl -f        # Canlı takip
```

### SysVinit (Klasik)

SysVinit, geleneksel init sistemidir. Runlevel kavramını kullanır:

| Runlevel | Açıklama |
|----------|----------|
| 0 | Halt (sistem kapat) |
| 1 | Single user (bakım modu) |
| 2 | Multi-user (ağ yok) |
| 3 | Multi-user (CLI) |
| 5 | Multi-user (GUI) |
| 6 | Reboot |

```bash
# Servis kontrolü
/etc/init.d/ssh start
/etc/init.d/ssh stop
/etc/init.d/ssh restart

# Runlevel değiştir
init 3    # CLI moduna geç
init 0    # Sistemi kapat
```

### BusyBox init (Minimal)

BusyBox, gömülü sistemler için minimal bir init sağlar. Yapılandırma `/etc/inittab` dosyasında yapılır:

```bash
# /etc/inittab örneği
::sysinit:/etc/init.d/rcS
::respawn:/sbin/getty 115200 ttyS0
::shutdown:/bin/umount -a -r
```

---

## 🔍 Boot Sürecini İzleme

```bash
# Kernel mesajları (ring buffer)
dmesg | head -50
dmesg | less

# systemd boot analizi
systemd-analyze                    # Toplam boot süresi
systemd-analyze blame              # Servis başına süre
systemd-analyze critical-chain     # Kritik yol
systemd-analyze plot > boot.svg    # Grafik

# Boot logları
journalctl -b          # Mevcut boot
journalctl -b -1       # Önceki boot
```

---

## 🎯 Pratik Ödevler

1. BBB veya RPi'de seri port üzerinden U-Boot konsoluna eriş
2. `printenv` ile ortam değişkenlerini listele
3. `bootdelay` değişkenini değiştir ve kaydet
4. TFTP sunucu kur ve ağ üzerinden boot dene
5. Device tree dosyasını bul ve `dtc` ile decompile et
6. `dmesg` ile boot mesajlarını incele
7. `systemd-analyze` ile boot süresini ölç

---

## 📚 Kaynaklar

- [U-Boot Documentation](https://docs.u-boot.org/)
- [U-Boot Source Code](https://github.com/u-boot/u-boot)
- [Device Tree Specification](https://www.devicetree.org/)
- [Bootlin U-Boot Training](https://bootlin.com/doc/training/embedded-linux/)

---

*Sonraki ders: Cross-Compilation*
