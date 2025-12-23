# BeagleBone Black Kurulum Rehberi

## 🎯 Amaç
BeagleBone Black (BBB) üzerine Linux kurulumu ve yapılandırma.

---

## 1. Gerekli Malzemeler

| Malzeme | Açıklama |
|---------|----------|
| BeagleBone Black | 4G, Wireless veya Industrial |
| Micro SD Kart | 8-16GB yeterli |
| Mini USB Kablosu | Güç ve seri bağlantı için |
| Micro HDMI Kablosu | Opsiyonel (monitör için) |
| Ethernet Kablosu | İnternet bağlantısı için |
| 5V DC Güç Kaynağı | Opsiyonel (USB ile de çalışır) |

---

## 2. BBB Özellikleri

- **SoC**: TI Sitara AM335x (ARM Cortex-A8)
- **RAM**: 512MB DDR3
- **Depolama**: 4GB eMMC (on-board flash)
- **GPIO**: 2x46 pin header (P8, P9)
- **USB**: 1x USB Host, 1x Mini USB (client)

> [!NOTE]
> BBB fabrikadan Debian Linux yüklü olarak gelir. eMMC'den boot eder.

---

## 3. İlk Bağlantı (USB over IP)

### Windows'ta

1. Mini USB kabloyu BBB ve PC'ye bağla
2. BBB güç aldığında 4 LED yanar
3. Windows yeni bir sürücü tanır
4. Tarayıcıda `http://192.168.7.2` aç
5. Gerekli driver'ları kur

### Linux'ta

```bash
# BBB'yi USB ile bağla
# Yeni ağ arayüzü oluşur (usb0 veya enx...)

# IP kontrolü
ip addr show | grep 192.168.7

# SSH bağlantısı
ssh debian@192.168.7.2
# Şifre: temppwd
```

---

## 4. eMMC'ye Yeni İmaj Yazma

### İmaj İndirme

```bash
# Resmi imajlar:
# https://www.beagleboard.org/distros

# Flasher imaj indir (eMMC yazma için)
wget https://rcn-ee.com/rootfs/bb.org/testing/...flasher.img.xz

# Sıkıştırmayı aç
unxz image.img.xz
```

### SD Karta Yazma

```bash
# SD karta yaz
sudo dd if=image.img of=/dev/sdX bs=4M status=progress

# Sync
sync
```

### eMMC'ye Flash

1. SD kartı BBB'ye tak
2. **Boot butonuna basılı tutarak** güç ver
3. 7-8 saniye boot butonunu basılı tut
4. 4 LED sırayla yanıp sönmeye başlar
5. ~5 dakika bekle (LED'ler söndüğünde bitti)
6. SD kartı çıkar, sistemi yeniden başlat

> [!IMPORTANT]
> Boot butonu: SD kart yuvasının yanındaki küçük buton

---

## 5. Temel Yapılandırma

### Türkçe Klavye Ayarı

```bash
# Locale ayarı
sudo dpkg-reconfigure locales
# tr_TR.UTF-8 seç

# Klavye düzeni
sudo nano /etc/default/keyboard
# XKBLAYOUT="tr"

# Console font (UTF-8 desteği)
sudo nano /etc/default/console-setup
# CHARMAP="UTF-8"

# Uygula
sudo setupcon
```

### Ağ Yapılandırması

```bash
# Ethernet ile internet (en basit yol)
# RJ45 kabloyu router'a bağla

# IP kontrolü
ip addr show eth0

# veya DHCP ile
sudo dhclient eth0
```

### SSH Sunucu

```bash
# SSH durumu
sudo systemctl status ssh

# Yoksa kur
sudo apt install openssh-server

# Otomatik başlat
sudo systemctl enable ssh
```

---

## 6. GPIO Header Pinout

```
          P9 (sol)                         P8 (sağ)
    ┌─────────┬─────────┐           ┌─────────┬─────────┐
  1 │   GND   │   GND   │ 2       1 │   GND   │   GND   │ 2
  3 │  3.3V   │  3.3V   │ 4       3 │ GPIO1_6 │ GPIO1_7 │ 4
  5 │   5V    │   5V    │ 6       5 │ GPIO1_2 │ GPIO1_3 │ 6
    │   ...   │   ...   │           │   ...   │   ...   │
    └─────────┴─────────┘           └─────────┴─────────┘
```

**Detaylı pinout için:** `lecture_08_gpio/` ve `cheatsheets/gpio_pinout.md`

---

## 7. Faydalı Komutlar

```bash
# Sistem bilgisi
uname -a                              # Kernel
cat /proc/cpuinfo | head -20          # CPU
free -h                               # RAM

# Board bilgisi
cat /etc/dogtag                       # BBB versiyon

# GPIO durumu
ls /sys/class/gpio/

# eMMC/SD bilgisi
lsblk
df -h
```

---

## 8. Sorun Giderme

| Problem | Çözüm |
|---------|-------|
| Boot etmiyor | Boot butonuna basarak SD'den boot et |
| SSH bağlanmıyor | `ssh debian@192.168.7.2` dene |
| IP yok | Ethernet kabloyu kontrol et |
| 4G görünmüyor | eMMC'yi flasher imajıyla güncelle |

---

*Kaynaklar: [docs.beagleboard.org](https://docs.beagleboard.org/)*
