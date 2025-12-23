# Raspberry Pi Kurulum Rehberi

## 🎯 Amaç
Raspberry Pi üzerine Linux işletim sistemi kurulumu ve temel yapılandırma.

---

## 1. Gerekli Malzemeler

| Malzeme | Açıklama |
|---------|----------|
| Raspberry Pi 4/5 | 4GB+ RAM önerilir |
| Micro SD Kart | 64GB, Class 10, yüksek hız |
| USB-C Güç Kaynağı | RPi4: 5V/3A, RPi5: 5V/5A (27W) |
| Micro HDMI Kablosu | RPi4/5 için |
| USB Klavye/Fare | İlk kurulum için |
| Ethernet Kablosu | Opsiyonel (WiFi varsa) |

---

## 2. İşletim Sistemi Kurulumu

### Raspberry Pi Imager ile Kurulum (Önerilen)

```bash
# Linux'ta Raspberry Pi Imager kurulumu
sudo apt install rpi-imager

# veya indirme
# https://www.raspberrypi.com/software/
```

**Adımlar:**
1. Raspberry Pi Imager'ı çalıştır
2. **Device**: Raspberry Pi modelini seç
3. **OS**: Raspberry Pi OS (64-bit) seç
4. **Storage**: SD kartı seç
5. **Edit Settings** ile ön yapılandırma:
   - Hostname ayarla
   - Kullanıcı adı/şifre belirle
   - WiFi yapılandır
   - SSH'ı etkinleştir
6. **Write** tuşuna bas

### Manuel Kurulum (dd ile)

```bash
# İmaj dosyasını indir
wget https://downloads.raspberrypi.org/raspios_arm64/images/...

# İmajı SD karta yaz (sdX yerine doğru cihazı yaz!)
sudo dd if=raspios.img of=/dev/sdX bs=4M status=progress conv=fsync

# Sync ve çıkar
sync
sudo eject /dev/sdX
```

> [!CAUTION]
> `dd` komutunda yanlış cihaz seçimi veri kaybına neden olabilir! `lsblk` ile cihazı doğrulayın.

---

## 3. İlk Boot ve Bağlantı

### Fiziksel Bağlantı
1. SD kartı takın
2. HDMI kabloyu bağlayın
3. Klavye/fareyi bağlayın
4. Güç kablosunu bağlayın
5. Sistem otomatik boot eder

### SSH ile Uzaktan Bağlantı

```bash
# IP adresini bul (router'dan veya nmap ile)
nmap -sn 192.168.1.0/24 | grep -i raspberry

# SSH bağlantısı
ssh kullanici@192.168.1.X

# İlk bağlantıda parmak izi onayı
# Are you sure you want to continue connecting? yes
```

---

## 4. Temel Yapılandırma

### raspi-config ile Yapılandırma

```bash
sudo raspi-config
```

**Önemli Ayarlar:**
- **System Options > Hostname**: Cihaz adını değiştir
- **Interface Options > SSH**: SSH'ı etkinleştir
- **Interface Options > VNC**: VNC'yi etkinleştir
- **Localisation Options > Locale**: tr_TR.UTF-8 ekle
- **Localisation Options > Timezone**: Europe/Istanbul
- **Localisation Options > Keyboard**: Turkish

### Manuel Yapılandırma

```bash
# Sistem güncelleme
sudo apt update && sudo apt upgrade -y

# Türkçe klavye ayarı
sudo nano /etc/default/keyboard
# XKBLAYOUT="tr" olarak değiştir

# Locale ayarı
sudo dpkg-reconfigure locales
# tr_TR.UTF-8 seç

# Değişiklikleri uygula
sudo reboot
```

---

## 5. Geliştirme Araçları Kurulumu

```bash
# Temel geliştirme araçları
sudo apt install -y build-essential git vim

# Kernel modül geliştirme için
sudo apt install -y linux-headers-$(uname -r)

# Python geliştirme
sudo apt install -y python3-pip python3-dev

# GPIO kütüphaneleri
sudo apt install -y python3-gpiozero python3-rpi.gpio
```

---

## 6. Faydalı Komutlar

```bash
# Sistem bilgisi
uname -a                    # Kernel bilgisi
cat /proc/cpuinfo          # CPU bilgisi
free -h                    # RAM kullanımı
df -h                      # Disk kullanımı
vcgencmd measure_temp      # CPU sıcaklığı (RPi özel)

# Ağ bilgisi
hostname -I                # IP adresi
ifconfig                   # Ağ arayüzleri

# GPIO durumu (RPi özel)
pinout                     # GPIO pinout şeması
gpio readall               # Tüm GPIO durumları
```

---

## 7. Sorun Giderme

| Problem | Çözüm |
|---------|-------|
| Boot etmiyor | SD kartı kontrol et, imajı yeniden yaz |
| SSH bağlanmıyor | SSH servisini kontrol et: `sudo systemctl status ssh` |
| WiFi bağlanmıyor | `sudo raspi-config` > Network Options |
| Ekran siyah | HDMI kabloyu kontrol et, config.txt düzenle |

---

*Kaynaklar: [raspberrypi.com/documentation](https://www.raspberrypi.com/documentation/)*
