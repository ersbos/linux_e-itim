# Ders 2: Geliştirme Ortamı Kurulumu

> **Konu:** Raspberry Pi ve BeagleBone Black Kurulumu, Uzak Erişim

---

## 📋 Bu Derste İşlenenler

- [x] Raspberry Pi işletim sistemi kurulumu
- [x] BeagleBone Black kurulumu (eMMC flash)
- [x] SSH ile uzak erişim
- [x] VNC/RDP ile grafiksel erişim
- [x] Temel yapılandırma (locale, klavye)

---

## 🍓 Raspberry Pi Kurulumu

### Gerekli Malzemeler

| Malzeme | Açıklama |
|---------|----------|
| Raspberry Pi 4/5 | 4GB+ RAM önerilir |
| Micro SD Kart | 64GB, Class 10 |
| USB-C Güç | RPi4: 5V/3A, RPi5: 5V/5A |
| Micro HDMI Kablo | İlk kurulum için |

### Raspberry Pi Imager ile Kurulum

```bash
# Linux'ta kurulum
sudo apt install rpi-imager

# Programı başlat
rpi-imager
```

**Adımlar:**
1. Device → Raspberry Pi modelini seç
2. OS → Raspberry Pi OS (64-bit) seç
3. Storage → SD kartı seç
4. **Edit Settings** (ÖNEMLİ!):
   - Hostname: `raspberrypi`
   - Username/Password belirle
   - WiFi ayarla
   - **SSH'ı etkinleştir**
   - Locale: `Europe/Istanbul`
5. Write → Bekle → Çıkar

### Manuel Kurulum (dd)

```bash
# İmaj indir
wget https://downloads.raspberrypi.org/raspios_arm64/images/...

# SD karta yaz (sdX yerine doğru cihaz!)
sudo dd if=raspios.img of=/dev/sdX bs=4M status=progress conv=fsync

# Sync ve çıkar
sync
sudo eject /dev/sdX
```

> [!CAUTION]
> `dd` komutunda yanlış cihaz seçimi = VERİ KAYBI! `lsblk` ile kontrol et.

---

## 🦴 BeagleBone Black Kurulumu

### BBB Özellikleri

- **SoC:** TI AM335x (ARM Cortex-A8)
- **RAM:** 512MB
- **Depolama:** 4GB eMMC (on-board)
- **GPIO:** 2x46 pin (P8, P9)

### USB Bağlantısı

```bash
# BBB'yi mini USB ile PC'ye bağla
# Otomatik IP alır: 192.168.7.2

# SSH bağlantısı
ssh debian@192.168.7.2
# Şifre: temppwd
```

### eMMC'ye Yeni İmaj Yazma

```bash
# 1. Flasher imaj indir
wget https://rcn-ee.com/rootfs/bb.org/.../flasher.img.xz

# 2. Aç
unxz flasher.img.xz

# 3. SD karta yaz
sudo dd if=flasher.img of=/dev/sdX bs=4M status=progress

# 4. SD kartı BBB'ye tak
# 5. BOOT BUTONUNA BASILI TUTARAK güç ver
# 6. 4 LED sırayla yanıp sönünce bekle (~5dk)
# 7. LED'ler sönünce SD kartı çıkar, yeniden başlat
```

---

## 🔌 SSH ile Uzak Erişim

### SSH Sunucu Kurulumu (Target)

```bash
# Genellikle zaten yüklü
sudo apt install openssh-server

# Servis durumu
sudo systemctl status ssh

# Otomatik başlat
sudo systemctl enable ssh
```

### SSH Bağlantısı (Host)

```bash
# Temel bağlantı
ssh user@hostname
ssh user@192.168.1.100

# Port belirterek
ssh -p 2222 user@host

# Anahtar ile
ssh -i ~/.ssh/id_rsa user@host
```

### SSH Anahtar Oluşturma

```bash
# Host'ta anahtar üret
ssh-keygen -t ed25519 -C "your_email@example.com"

# Target'a kopyala
ssh-copy-id user@target

# Artık şifresiz giriş
ssh user@target
```

### SCP ile Dosya Transferi

```bash
# Host → Target
scp file.txt user@target:/path/

# Target → Host
scp user@target:/path/file.txt ./

# Dizin kopyala
scp -r dir/ user@target:/path/
```

---

## 🖥️ VNC ile Grafiksel Erişim

### VNC Sunucu (Target - RPi)

```bash
# RPi'da dahili VNC
sudo raspi-config
# → Interface Options → VNC → Enable

# veya manuel
sudo apt install realvnc-vnc-server
sudo systemctl enable vncserver-x11-serviced
```

### VNC İstemci (Host)

```bash
# Linux
sudo apt install remmina

# veya RealVNC Viewer indir
# https://www.realvnc.com/en/connect/download/viewer/
```

**Bağlantı:** `hostname:5900` veya `hostname:0`

---

## 🎛️ Temel Yapılandırma

### raspi-config (RPi)

```bash
sudo raspi-config
```

**Önemli menüler:**
- System Options → Hostname
- Interface Options → SSH, VNC, I2C, SPI
- Localisation Options → Locale, Timezone, Keyboard

### Türkçe Klavye Ayarı

```bash
# Klavye düzeni
sudo nano /etc/default/keyboard
# XKBLAYOUT="tr"

# Uygula
sudo setupcon

# veya reboot
sudo reboot
```

### Locale Ayarı

```bash
# Interaktif
sudo dpkg-reconfigure locales
# → tr_TR.UTF-8 seç

# veya manuel
sudo nano /etc/default/locale
# LANG=tr_TR.UTF-8
```

### Timezone

```bash
sudo timedatectl set-timezone Europe/Istanbul
```

---

## 📦 Geliştirme Araçları Kurulumu

```bash
# Sistem güncelleme
sudo apt update && sudo apt upgrade -y

# Temel araçlar
sudo apt install -y build-essential git vim

# Kernel headers (modül derleme için)
sudo apt install -y linux-headers-$(uname -r)

# Python geliştirme
sudo apt install -y python3-pip python3-dev

# GPIO (RPi)
sudo apt install -y python3-gpiozero python3-rpi.gpio

# GPIO (BBB)
sudo apt install -y python3-adafruit-bbio
```

---

## 🔍 Faydalı Komutlar

```bash
# IP adresi
hostname -I
ip addr show

# Sistem bilgisi
uname -a
cat /etc/os-release

# Disk kullanımı
df -h

# Ağ testi
ping -c 3 google.com
```

---

## 🎯 Pratik

1. Raspberry Pi veya BBB'ye Linux kur
2. SSH ile bağlan
3. Türkçe klavye ayarla
4. Geliştirme araçlarını kur
5. `hello_embedded.c` programını target'ta derle ve çalıştır

---

*Detaylı rehberler için: `setup/` dizinine bakın*
