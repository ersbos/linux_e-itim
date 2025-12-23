# 🐧 Gömülü Linux Sistemleri Kursu

> **Embedded Linux Systems - Development & Application Course**

Bu kurs, ARM tabanlı gömülü sistemlerde Linux geliştirme konularını kapsamlı bir şekilde ele almaktadır.  
**Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📚 Kurs İçeriği

| # | Ders | Açıklama | Örnekler |
|---|------|----------|----------|
| 01 | [Giriş](lecture_one/) | Gömülü Linux'a Giriş, ARM Mimarisi | ✅ |
| 02 | [Geliştirme Ortamı](lecture_02_development_env/) | RPi/BBB Kurulumu, SSH/VNC | - |
| 03 | [Boot Süreci](lecture_03_boot_process/) | U-Boot, Device Tree, Kernel Yükleme | - |
| 04 | [Cross-Compilation](lecture_04_cross_compilation/) | Araç Zincirleri, Makefile | ✅ |
| 05 | [Kernel Temelleri](lecture_05_kernel_basics/) | Kernel Yapısı, Konfigürasyon, Derleme | ✅ |
| 06 | [Aygıt Sürücüler 1](lecture_06_kernel_modules_1/) | Kernel Modüller, printk, Parametreler | ✅ |
| 07 | [Aygıt Sürücüler 2](lecture_07_kernel_modules_2/) | Character Devices, file_operations, IOCTL | ✅ |
| 08 | [GPIO Programlama](lecture_08_gpio/) | LED, Button, Interrupt, libgpiod | ✅ |
| 09 | [İletişim Protokolleri](lecture_09_protocols/) | UART, I2C, SPI | ✅ |
| 10 | [Buildroot/Yocto](lecture_10_buildroot_yocto/) | Özel Linux Sistemleri, Custom Packages | ✅ |
| 11 | [Gerçek Zamanlı](lecture_11_realtime/) | PREEMPT_RT, Scheduling, Memory Lock | ✅ |
| 12 | [Debug/Profiling](lecture_12_debugging/) | GDB, Valgrind, strace, perf | ✅ |

---

## 🛠️ Gerekli Donanım

### Temel Gereksinimler
- **Raspberry Pi 4/5** (4GB+ RAM önerilir)
- **Micro SD Kart** (32GB+, Class 10)
- **USB-C Güç Kaynağı** (5V/3A)
- **HDMI Kablo** + Monitör
- **USB Klavye/Fare**

### Opsiyonel (İleri Konular)
- **BeagleBone Black** (GPIO ve Device Tree örnekleri için ideal)
- **USB-UART Dönüştürücü** (CP2102, FT232)
- **Breadboard** + LED + Direnç + Buton
- **I2C Sensör** (DS3231 RTC, BME280)
- **SPI Flash** veya OLED ekran
- **Ethernet Kablosu**

---

## 🚀 Hızlı Başlangıç

```bash
# Repoyu klonla
git clone https://github.com/yourusername/linux_egitim.git
cd linux_egitim

# Kurulum rehberlerine göz at
ls setup/

# İlk derse geç
cat lecture_one/notes.md

# Örnekleri derle
cd lecture_08_gpio/examples
make
```

---

## 📁 Dizin Yapısı

```
linux_eğitim/
├── README.md                    # Bu dosya
├── setup/                       # Kurulum rehberleri
│   ├── raspberry_pi_setup.md
│   ├── beaglebone_setup.md
│   └── host_setup.md
├── cheatsheets/                 # Hızlı referanslar
│   ├── linux_commands.md
│   ├── gpio_pinout.md
│   └── makefile_templates.md
├── lecture_one/                 # Ders 1: Giriş
│   ├── notes.md
│   ├── commands.md
│   └── examples/
├── lecture_02_development_env/  # Ders 2
├── ...
└── lecture_12_debugging/        # Ders 12
    ├── notes.md
    └── examples/
        ├── segfault_example.c
        ├── memory_leak.c
        ├── gdb_practice.c
        └── Makefile
```

---

## 💻 Örnek Projeler

Her derste pratik örnekler bulunmaktadır:

| Ders | Örnekler |
|------|----------|
| **Ders 7** | Character device driver, IOCTL iletişimi |
| **Ders 8** | LED blink, button interrupt, libgpiod |
| **Ders 9** | UART terminal, I2C RTC okuma, SPI loopback |
| **Ders 10** | Buildroot custom package oluşturma |
| **Ders 11** | RT scheduling, memory locking |
| **Ders 12** | Core dump analizi, Valgrind, GDB pratiği |

```bash
# Örnek derleme
cd lecture_09_protocols/examples
make
sudo ./i2c_example /dev/i2c-1 0x68
```

---

## 📋 Cheatsheets

| Dosya | İçerik |
|-------|--------|
| [linux_commands.md](cheatsheets/linux_commands.md) | Temel Linux komutları |
| [gpio_pinout.md](cheatsheets/gpio_pinout.md) | BBB ve RPi pin diyagramları |
| [makefile_templates.md](cheatsheets/makefile_templates.md) | Makefile şablonları |

---

## 📖 Kaynaklar

### Resmi Dokümantasyon
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [BeagleBoard Documentation](https://docs.beagleboard.org/)
- [Linux Kernel Documentation](https://docs.kernel.org/)

### Eğitim Materyalleri
- [Bootlin Training Materials](https://bootlin.com/training/)
- [Linux Device Drivers (LDD3)](https://lwn.net/Kernel/LDD3/)
- [The Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)

### Araçlar
- [Buildroot Manual](https://buildroot.org/downloads/manual/manual.html)
- [Yocto Project Documentation](https://docs.yoctoproject.org/)
- [GDB Manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)

---

## 📝 Lisans

Bu materyal eğitim amaçlıdır.  
Kaynak: C ve Sistem Programcıları Derneği

---

*Son güncelleme: 2025-12-23*
