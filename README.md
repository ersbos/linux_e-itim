# 🐧 Gömülü Linux Sistemleri Kursu

> **Embedded Linux Systems - Development & Application Course**

Bu kurs, ARM tabanlı gömülü sistemlerde Linux geliştirme konularını kapsamlı bir şekilde ele almaktadır.

---

## 📚 Kurs İçeriği

| # | Ders | Açıklama |
|---|------|----------|
| 01 | [Giriş](lecture_one/) | Gömülü Linux'a Giriş, ARM Mimarisi |
| 02 | [Geliştirme Ortamı](lecture_02_development_env/) | RPi/BBB Kurulumu, SSH/VNC |
| 03 | [Boot Süreci](lecture_03_boot_process/) | U-Boot, Kernel Yükleme |
| 04 | [Cross-Compilation](lecture_04_cross_compilation/) | Araç Zincirleri, Makefile |
| 05 | [Kernel Temelleri](lecture_05_kernel_basics/) | Kernel Yapısı, Derleme |
| 06 | [Aygıt Sürücüler 1](lecture_06_kernel_modules_1/) | Kernel Modüller, printk |
| 07 | [Aygıt Sürücüler 2](lecture_07_kernel_modules_2/) | Character Devices, IOCTL |
| 08 | [GPIO Programlama](lecture_08_gpio/) | LED, Button, Kesme |
| 09 | [İletişim Protokolleri](lecture_09_protocols/) | I2C, SPI, UART |
| 10 | [Buildroot/Yocto](lecture_10_buildroot_yocto/) | Özel Linux Sistemleri |
| 11 | [Gerçek Zamanlı](lecture_11_realtime/) | PREEMPT_RT, Scheduling |
| 12 | [Debug/Profiling](lecture_12_debugging/) | GDB, strace, perf |

---

## 🛠️ Gerekli Donanım

### Temel Gereksinimler
- **Raspberry Pi 4/5** (4GB+ RAM önerilir)
- **Micro SD Kart** (64GB, Class 10)
- **USB-C Güç Kaynağı** (5V/3A)
- **HDMI Kablo** + Monitör
- **USB Klavye/Fare**

### Opsiyonel
- **BeagleBone Black** (GPIO örnekleri için ideal)
- **USB-UART Dönüştürücü** (CP2102)
- **Breadboard** + LED + Direnç + Buton
- **Ethernet Kablosu**

---

## 🚀 Hızlı Başlangıç

```bash
# Kurulum rehberlerine göz atın
ls setup/

# İlk derse geçin
cd lecture_one/
cat notes.md
```

---

## 📋 Cheatsheets

- [Linux Komutları](cheatsheets/linux_commands.md)
- [GPIO Pinout](cheatsheets/gpio_pinout.md)
- [Makefile Şablonları](cheatsheets/makefile_templates.md)

---

## 📖 Kaynaklar

- [Raspberry Pi Dokümantasyonu](https://www.raspberrypi.com/documentation/)
- [BeagleBoard Dokümantasyonu](https://docs.beagleboard.org/)
- [Linux Kernel Dokümantasyonu](https://docs.kernel.org/)
- [Bootlin Eğitimleri](https://bootlin.com/training/)

---

*Son güncelleme: 2025-12-23*
