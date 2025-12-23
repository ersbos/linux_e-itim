# Host Sistem Kurulum Rehberi

## 🎯 Amaç
Gömülü Linux geliştirme için host (ana) bilgisayar ortamının hazırlanması.

---

## 1. Önerilen Host Sistemler

| Dağıtım | Avantajlar |
|---------|------------|
| **Ubuntu 22.04/24.04** | En yaygın, iyi dokümantasyon |
| **Linux Mint** | Ubuntu tabanlı, kullanıcı dostu |
| **Debian** | Kararlı, BBB ile uyumlu |
| **Fedora** | Güncel paketler |

> [!TIP]
> Windows kullanıcıları WSL2 veya sanal makine (VirtualBox/VMware) kullanabilir.

---

## 2. Temel Araçlar Kurulumu

### Build Essentials

```bash
# Temel derleme araçları
sudo apt update
sudo apt install -y build-essential

# Bileşenler:
# - gcc (C derleyici)
# - g++ (C++ derleyici)
# - make (build sistemi)
# - libc-dev (C kütüphanesi)
```

### Geliştirme Araçları

```bash
# Git versiyon kontrolü
sudo apt install -y git

# Metin editörleri
sudo apt install -y vim nano

# Dosya transferi
sudo apt install -y openssh-client scp

# Seri port araçları
sudo apt install -y minicom screen picocom

# Disk araçları
sudo apt install -y parted gparted
```

---

## 3. Cross-Compiler Kurulumu

### ARM 32-bit (BeagleBone Black için)

```bash
# Debian/Ubuntu paketinden
sudo apt install -y gcc-arm-linux-gnueabihf

# Kontrol
arm-linux-gnueabihf-gcc --version
```

### ARM 64-bit (Raspberry Pi 4/5 için)

```bash
# Debian/Ubuntu paketinden
sudo apt install -y gcc-aarch64-linux-gnu

# Kontrol
aarch64-linux-gnu-gcc --version
```

### Manuel Kurulum (Linaro/ARM)

```bash
# ARM toolchain indir
wget https://developer.arm.com/-/media/Files/downloads/gnu/...

# Çıkar
tar -xf gcc-arm-*-x86_64-arm-none-linux-gnueabihf.tar.xz

# PATH'e ekle (~/.bashrc'ye ekle)
export PATH=$PATH:/path/to/gcc-arm-.../bin

# Kaynak dosyayı yeniden yükle
source ~/.bashrc
```

---

## 4. Kernel Geliştirme Araçları

```bash
# Kernel başlık dosyaları (host için)
sudo apt install -y linux-headers-$(uname -r)

# Kernel kaynak kodu (opsiyonel)
sudo apt install -y linux-source

# Kernel yapılandırma araçları
sudo apt install -y libncurses-dev flex bison libssl-dev

# Device tree derleyici
sudo apt install -y device-tree-compiler
```

---

## 5. SD Kart Yazma Araçları

### Raspberry Pi Imager

```bash
sudo apt install -y rpi-imager
```

### Balena Etcher

```bash
# Snap ile
sudo snap install balena-etcher

# veya AppImage indir
# https://etcher.balena.io/
```

### dd (Manuel)

```bash
# Zaten sistemde var
# Kullanım:
sudo dd if=image.img of=/dev/sdX bs=4M status=progress
```

---

## 6. Seri Port Yapılandırması

### Kullanıcıyı dialout grubuna ekle

```bash
# Seri porta erişim izni
sudo usermod -aG dialout $USER

# Değişikliğin etkili olması için çıkış yap
logout
# veya
newgrp dialout
```

### Minicom Yapılandırması

```bash
# Minicom başlat (yapılandırma için)
sudo minicom -s

# Serial port setup:
# - Serial Device: /dev/ttyUSB0
# - Baud Rate: 115200
# - 8N1 (8 data bits, No parity, 1 stop bit)
# - Hardware/Software Flow Control: No

# Kaydet ve çık
```

---

## 7. TFTP ve NFS Sunucu (Opsiyonel)

### TFTP Sunucu (kernel/bootloader transferi için)

```bash
# Kurulum
sudo apt install -y tftpd-hpa

# Yapılandırma
sudo nano /etc/default/tftpd-hpa
# TFTP_DIRECTORY="/srv/tftp"
# TFTP_OPTIONS="--secure"

# Servis başlat
sudo systemctl restart tftpd-hpa
sudo systemctl enable tftpd-hpa
```

### NFS Sunucu (root filesystem paylaşımı için)

```bash
# Kurulum
sudo apt install -y nfs-kernel-server

# Export dizini oluştur
sudo mkdir -p /srv/nfs/rootfs

# /etc/exports düzenle
sudo nano /etc/exports
# Ekle: /srv/nfs/rootfs *(rw,no_root_squash,no_subtree_check)

# Servis yeniden başlat
sudo exportfs -a
sudo systemctl restart nfs-kernel-server
```

---

## 8. Ortam Değişkenleri Şablonu

`~/.bashrc` veya `~/.bash_profile` dosyasına ekle:

```bash
# Embedded Linux Development Environment

# ARM 32-bit Cross-Compiler (BBB için)
export CROSS_COMPILE_ARM32=arm-linux-gnueabihf-

# ARM 64-bit Cross-Compiler (RPi için)
export CROSS_COMPILE_ARM64=aarch64-linux-gnu-

# Manuel toolchain (gerekirse)
# export PATH=$PATH:/opt/gcc-arm/bin

# Kernel derleme için kısayollar
alias make-arm32='make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE_ARM32'
alias make-arm64='make ARCH=arm64 CROSS_COMPILE=$CROSS_COMPILE_ARM64'
```

---

## 9. Kurulum Doğrulama

```bash
# Temel araçlar
gcc --version
make --version
git --version

# Cross-compiler
arm-linux-gnueabihf-gcc --version 2>/dev/null || echo "ARM32 yok"
aarch64-linux-gnu-gcc --version 2>/dev/null || echo "ARM64 yok"

# Seri port
ls /dev/ttyUSB* 2>/dev/null || echo "USB-Serial yok"

# Kullanıcı grupları
groups | grep dialout || echo "dialout grubunda değil"
```

---

*Sonraki adım: Raspberry Pi veya BeagleBone Black kurulumuna geç*
