# Ders 3: Boot Süreci ve Bootloader

> **Konu:** Linux Boot Süreci, U-Boot, Device Tree

---

## 📋 Bu Derste İşlenenler

- [x] Boot süreci aşamaları
- [x] Bootloader nedir?
- [x] U-Boot temelleri
- [x] Kernel yükleme
- [x] Device Tree kavramı
- [x] Init sistemleri

---

## 🔄 Boot Süreci Aşamaları

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

---

## 🔧 U-Boot Nedir?

**U-Boot (Universal Bootloader)** = Gömülü sistemlerde en yaygın bootloader.

### Özellikler
- Açık kaynak (GPL)
- ARM, x86, MIPS, PowerPC desteği
- SD, eMMC, NAND, NOR, USB, TFTP boot
- Scripting desteği
- Interaktif konsol

### U-Boot Ortam Değişkenleri

```bash
# U-Boot konsolunda (seri port ile)

# Değişkenleri listele
printenv

# Değişken ayarla
setenv bootdelay 5
setenv ipaddr 192.168.1.100

# Kaydet (kalıcı)
saveenv
```

### Önemli U-Boot Değişkenleri

| Değişken | Açıklama |
|----------|----------|
| `bootcmd` | Otomatik boot komutu |
| `bootargs` | Kernel'a geçirilen argümanlar |
| `bootdelay` | Boot öncesi bekleme (saniye) |
| `ipaddr` | Cihaz IP adresi |
| `serverip` | TFTP sunucu IP |
| `loadaddr` | Kernel yükleme adresi |

---

## 💾 Kernel Yükleme

### SD Karttan Boot

```bash
# U-Boot konsolunda

# Fat partition'dan kernel yükle
fatload mmc 0:1 ${loadaddr} zImage

# Device tree yükle
fatload mmc 0:1 ${fdtaddr} am335x-boneblack.dtb

# Boot et
bootz ${loadaddr} - ${fdtaddr}
```

### TFTP ile Boot (Ağ üzerinden)

```bash
# Host'ta TFTP sunucu kur
sudo apt install tftpd-hpa
sudo cp zImage /srv/tftp/

# U-Boot'ta
setenv serverip 192.168.1.1
setenv ipaddr 192.168.1.100
tftp ${loadaddr} zImage
tftp ${fdtaddr} am335x-boneblack.dtb
bootz ${loadaddr} - ${fdtaddr}
```

---

## 🌳 Device Tree (DT)

**Device Tree** = Donanım konfigürasyonunu tanımlayan veri yapısı.

### Neden Device Tree?

- Kernel'ı donanımdan bağımsız hale getirir
- Aynı kernel, farklı board'larda çalışır
- Donanım değişikliği = DT değişikliği (recompile gerekmez)

### DT Dosya Tipleri

| Uzantı | Açıklama |
|--------|----------|
| `.dts` | Device Tree Source (kaynak) |
| `.dtsi` | Include dosyası |
| `.dtb` | Device Tree Blob (derlenmiş) |

### DT Yapısı Örneği

```dts
/dts-v1/;

/ {
    model = "BeagleBone Black";
    compatible = "ti,am335x-bone-black";

    cpus {
        cpu@0 {
            compatible = "arm,cortex-a8";
            clock-frequency = <1000000000>;
        };
    };

    memory {
        device_type = "memory";
        reg = <0x80000000 0x20000000>; /* 512MB */
    };

    leds {
        compatible = "gpio-leds";
        led0 {
            label = "beaglebone:green:usr0";
            gpios = <&gpio1 21 0>;
        };
    };
};
```

### DT Derleme

```bash
# DTS → DTB
dtc -I dts -O dtb -o output.dtb input.dts

# DTB → DTS (tersine)
dtc -I dtb -O dts -o output.dts input.dtb

# Kernel kaynak ağacında
make dtbs
```

---

## ⚙️ Boot Argümanları

Kernel'a geçirilen komut satırı argümanları:

```bash
# Örnek bootargs
console=ttyO0,115200n8 root=/dev/mmcblk0p2 rootwait ro

# Anlamları:
# console=ttyO0,115200n8  → Seri konsol ayarı
# root=/dev/mmcblk0p2     → Root filesystem
# rootwait                → Root cihazı bekle
# ro                      → Read-only mount
```

### Yaygın Boot Argümanları

| Argüman | Açıklama |
|---------|----------|
| `console=` | Seri port konsol |
| `root=` | Root filesystem |
| `rootfstype=` | Filesystem tipi |
| `rootwait` | Root cihazı bekle |
| `init=` | Init programı |
| `quiet` | Sessiz boot |
| `debug` | Debug mesajları |

---

## 🏃 Init Sistemleri

### systemd (Modern)

```bash
# Servis yönetimi
systemctl status sshd
systemctl start sshd
systemctl enable sshd

# Log görüntüleme
journalctl -u sshd
```

### SysVinit (Klasik)

```bash
# Runlevel'lar
# 0: Halt
# 1: Single user
# 3: Multi-user (CLI)
# 5: Multi-user (GUI)
# 6: Reboot

# Servis kontrolü
/etc/init.d/ssh start
/etc/init.d/ssh stop
```

### BusyBox init (Minimal)

```bash
# /etc/inittab
::sysinit:/etc/init.d/rcS
::respawn:/sbin/getty 115200 ttyO0
```

---

## 🔍 Boot Sürecini İzleme

```bash
# Kernel mesajları
dmesg | head -50

# Systemd boot analizi
systemd-analyze
systemd-analyze blame
systemd-analyze critical-chain

# Boot log
journalctl -b
```

---

## 🎯 Pratik

1. U-Boot konsoluna eriş (seri port ile)
2. `printenv` ile değişkenleri listele
3. `dmesg` ile boot mesajlarını incele
4. Device tree dosyasını bul ve incele
5. Boot süresini analiz et

---

*Sonraki ders: Cross-Compilation*
