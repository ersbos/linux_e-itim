# Ders 2: Geliştirme Ortamı Kurulumu

> **Konu:** Raspberry Pi ve BeagleBone Black Kurulumu, Uzak Erişim, Yapılandırma
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Raspberry Pi işletim sistemi kurulumu
- [x] BeagleBone Black kurulumu ve eMMC flash
- [x] SSH ile uzak erişim (detaylı)
- [x] VNC ve RDP ile grafiksel erişim
- [x] Locale ve klavye yapılandırması
- [x] Geliştirme araçlarının kurulumu

---

## 🍓 Raspberry Pi Kurulumu

Raspberry Pi, özellikle az gelişmiş ülkelerde düşük fiyatlı bilgisayar oluşturmayı hedefleyen bir proje olarak başlamıştır. Günümüzde ise gömülü sistemler, eğitim ve hobi projeleri için en popüler platformlardan biri haline gelmiştir. Raspberry Pi'ı tamamen Linux tabanlı bir kişisel bilgisayar olarak kullanabilir, aynı zamanda gömülü sistem geliştirme platformu olarak da değerlendirebilirsiniz.

Raspberry Pi için birkaç işletim sistemi bulunmaktadır, ancak en yaygın kullanılanlar Linux'un Raspberry Pi OS (eski adıyla Raspbian) ve Ubuntu dağıtımlarıdır. Raspberry Pi OS, Raspberry Pi Foundation tarafından geliştirilen ve Raspberry Pi donanımıyla tam uyum içinde olan resmi dağıtımdır.

### Gerekli Malzemeler

| Malzeme | Açıklama |
|---------|----------|
| Raspberry Pi 4/5 | 4GB+ RAM önerilir, 8GB ideal |
| Micro SD Kart | 64GB Class 10 veya üzeri, yüksek hızlı |
| USB-C Güç Kaynağı | RPi4: 5V/3A, RPi5: 5V/5A (27W) |
| Micro HDMI Kablosu | İlk kurulum ve görsel çıkış için |
| USB Klavye/Fare | İlk yapılandırma için |
| Soğutucu (RPi5 için) | Yoğun işlemlerde zorunlu |
| Ethernet Kablosu | Opsiyonel, WiFi mevcut |

### Boot Süreci Açıklaması

Bir bilgisayar sistemi reset edildiğinde, işletim sisteminin yüklenmesini sağlayan yazılımlara **bootloader** denilmektedir. Raspberry Pi'da boot süreci şöyle gerçekleşir:

İşlemci reset edildiğinde çalışma belirli bir adresten başlar. Bu adreste, önceden flash EPROM'a yazılmış bootloader kodu bulunur. Bu kod önce çeşitli donanım birimlerini programlar ve kullanıma hazır hale getirir. Ardından bootloader'ın işletim sistemini yüklemekten sorumlu kısmı RAM'e yüklenir ve bu kısım işletim sistemini başlatır.

Bizim bu aşamada bilmemiz gereken en önemli şey, işletim sistemimizi micro SD karta nasıl yazacağımızdır. Doğru yazıldığında, geri kalan her şey otomatik olarak gerçekleşir.

### Raspberry Pi Imager ile Kurulum

Micro SD karta işletim sisteminin yüklenmesi rastgele yapılacak bir işlem değildir. Belirli programların diskin belirli sektörlerinde bulunması gerekir. Bunu sağlamanın en kolay yolu, önceden hazırlanmış bir imaj dosyasını micro SD karta aktarmaktır.

Son yıllarda Raspberry Pi ekibi bu işlemi kolaylaştırmak için **Raspberry Pi Imager** programını geliştirmiştir. Bu program, kullanıcının istediği işletim sisteminin imaj dosyasını otomatik olarak indirip micro SD karta yazmaktadır.

```bash
# Linux'ta kurulum
sudo apt install rpi-imager

# Programı başlat
rpi-imager
```

Raspberry Pi Imager çalıştırıldığında üç temel soru sorar:

1. **Device:** Hangi Raspberry Pi modelini kullandığınız
2. **Operating System:** Hangi işletim sistemini yükleyeceğiniz
3. **Storage:** Hangi micro SD karta yazma yapacağınız

**Edit Settings seçeneği** son derece önemlidir. Bu menüden yükleme öncesi yapılandırmalar yapılabilir:
- Hostname ayarı
- Kullanıcı adı ve parola belirleme
- WiFi bağlantı bilgileri
- **SSH'ı etkinleştirme** (uzaktan erişim için kritik)
- Locale ve timezone ayarları

### Manuel Kurulum (dd komutu ile)

Raspberry Pi Imager kullanmak zorunda değilsiniz. İmaj dosyasını manuel olarak indirip `dd` komutuyla da yazabilirsiniz. İmajlar şu adresten indirilebilir:

https://www.raspberrypi.com/software/operating-systems/

```bash
# İmaj dosyasını indir
wget https://downloads.raspberrypi.org/raspios_arm64/images/...

# Sıkıştırılmış ise aç
unxz raspios.img.xz

# SD kartı tanımla
lsblk
# /dev/sdX veya /dev/mmcblk0 olarak görünür

# DD ile yaz (DİKKAT! Doğru cihazı seçin)
sudo dd if=raspios.img of=/dev/sdX bs=4M status=progress conv=fsync

# Sync ve çıkar
sync
sudo eject /dev/sdX
```

> [!CAUTION]
> `dd` komutu son derece güçlüdür ve yanlış cihaz seçilirse tüm verileri silecektir! `lsblk` komutu ile SD kartın doğru cihaz adını mutlaka doğrulayın.

### İlk Açılış

Kurulum tamamlandıktan sonra aşağıdaki bağlantıları yapın:

1. SD kartı Raspberry Pi'a takın
2. Monitörü micro HDMI soketine bağlayın
3. Klavye ve fareyi USB portlara bağlayın
4. USB-C ile güç verin

Sistem birkaç dakika içinde açılacaktır. İlk açılışta otomatik olarak dosya sistemi genişletilir ve temel ayarlar yapılır.

---

## 🦴 BeagleBone Black Kurulumu

BeagleBone Black (BBB), özellikle GPIO yoğun projeler ve endüstriyel uygulamalar için tercih edilen güçlü bir SBC'dir. Raspberry Pi'dan farklı olarak, kendi içerisinde **eMMC** (Embedded Multi-Media Card) denilen on-board flash belleğe sahiptir. Ürün satın alındığında bu flash belleğin içerisinde zaten bir Debian Linux dağıtımı yüklü durumdadır.

### BBB Özellikleri ve Donanım

| Özellik | Değer |
|---------|-------|
| **SoC** | Texas Instruments Sitara AM335x |
| **CPU** | ARM Cortex-A8 @ 1 GHz |
| **RAM** | 512 MB DDR3 |
| **Depolama** | 4GB eMMC (on-board) |
| **GPIO** | 2x46 pin header (P8, P9) |
| **Analog Giriş** | 7 adet (1.8V max) |

BBB'nin donanım bileşenleri:
- **Güç girişi:** Mini USB veya DC barrel jack (5V)
- **USB Host:** Standart USB-A soketi (klavye/fare/hub için)
- **USB Client:** Mini USB (PC bağlantısı ve güç için)
- **Ethernet:** RJ45 soketi
- **Micro HDMI:** Video çıkışı
- **Boot düğmesi:** SD karttan boot için (USB soketinin yanında)
- **Reset düğmesi:** Sistemi reset için (ethernet soketinin yanında)

### İlk Bağlantı (USB over IP)

BBB'yi ilk kez kullanırken en pratik yöntem mini USB kablosuyla PC'ye bağlamaktır. Bu bağlantı hem güç sağlar hem de IP üzerinden iletişim kurulmasına olanak tanır.

BBB'yi mini USB ile bağladığınızda:
1. Sistem otomatik olarak boot edilir (eMMC'deki OS ile)
2. USB üzerinden IP bağlantısı oluşur
3. BBB'nin IP adresi: **192.168.7.2**
4. Host bilgisayarın IP adresi: **192.168.7.1**

```bash
# SSH ile bağlan
ssh debian@192.168.7.2

# Varsayılan parola
# temppwd
```

> [!NOTE]
> Windows'ta USB IP için aygıt sürücüsü kurulması gerekebilir. BBB bağlandığında görünen sürücü dizininden gerekli dosyalar yüklenebilir. UEFI BIOS ayarlarından imzasız sürücülere izin verilmesi gerekebilir.

### eMMC'ye Yeni İşletim Sistemi Yazma

BBB'nin on-board flash belleğine yeni işletim sistemi yüklemek için "flasher" imajları kullanılır. Bu imajlar, SD karttan boot ettikten sonra otomatik olarak eMMC'ye yazma yapar.

**Adım 1: İmaj İndir**

https://www.beagleboard.org/distros adresinden uygun imajı indirin. "Flasher" ibareli imajlardan birini seçin:
- **AM335x IoT Flasher:** Minimal, pencere yöneticisi yok
- **AM335x Xfce Flasher:** Xfce masaüstü dahil (512MB RAM için ağır olabilir)

```bash
# Örnek indirme
wget https://rcn-ee.com/rootfs/bb.org/...flasher.img.xz

# Sıkıştırmayı aç
unxz *.img.xz
```

**Adım 2: SD Karta Yaz**

```bash
# Linux'ta dd ile yaz
sudo dd if=flasher.img of=/dev/sdX bs=4M status=progress

# Windows'ta Rufus veya Etcher kullanabilirsiniz
```

**Adım 3: Flash İşlemi**

1. SD kartı BBB'nin yuvasına takın
2. **Boot düğmesine basılı tutarak** güç verin
3. 7-8 saniye boot düğmesine basılı tutmaya devam edin
4. Elinizi çekin
5. 4 LED önce tamamen yanar, sonra sırayla yanıp sönmeye başlar
6. Bu durum yazma işleminin başladığını gösterir
7. Yaklaşık 5 dakika bekleyin
8. LED'ler tamamen söndüğünde yazma tamamdır

**Adım 4: Yeniden Başlat**

SD kartı çıkarın ve sistemi yeniden başlatın. Artık yeni işletim sistemi eMMC'den boot edilecektir.

---

## 🔌 SSH ile Uzak Erişim

Gömülü sistemimize Linux kurduktan sonra, ona başka bir makineden erişmek isteyeceğiz. Bunun en yaygın yöntemi SSH (Secure Shell) protokolüdür. SSH, konsol tabanlı bir ortamda güvenli uzaktan bağlantı sağlar.

SSH bağlantısı için uzak makinede bir **SSH server**, bağlantı yapacağımız makinede bir **SSH client** programı olmalıdır. Modern Linux dağıtımlarında her iki program da genellikle kurulu gelir.

### SSH Server Kurulumu (Target Tarafı)

Raspberry Pi OS ve BBB'deki Debian'da SSH server zaten yüklüdür. Yüklü değilse:

```bash
# OpenSSH server kurulumu
sudo apt install openssh-server

# Servis durumunu kontrol et
sudo systemctl status ssh

# Servisi başlat
sudo systemctl start ssh

# Boot'ta otomatik başlat
sudo systemctl enable ssh

# Yapılandırma dosyası
sudo nano /etc/ssh/sshd_config
```

### SSH Client Bağlantısı (Host Tarafı)

SSH client programı Windows, macOS ve Linux'ta hazır bulunmaktadır.

```bash
# Temel bağlantı
ssh kullanici@hostname
ssh kullanici@192.168.1.100

# Alternatif kullanıcı belirtme
ssh -l kullanici hostname

# Farklı port kullanma
ssh -p 2222 kullanici@hostname

# Anahtar dosyası ile bağlantı
ssh -i ~/.ssh/id_rsa kullanici@hostname

# İlk bağlantıda host key doğrulaması
# "Are you sure you want to continue connecting?" → yes
```

### SSH Anahtarı Oluşturma (Şifresiz Giriş)

Her seferinde şifre girmemek için SSH anahtar çifti oluşturabilirsiniz. Bu yöntem hem daha güvenli hem de daha pratiktir.

```bash
# Host makinede anahtar üret
ssh-keygen -t ed25519 -C "your_email@example.com"
# veya RSA:
ssh-keygen -t rsa -b 4096

# Sorulara cevap ver (varsayılan konum için Enter)
# Passphrase: opsiyonel ama önerilir

# Anahtarı heap sistemine kopyala
ssh-copy-id kullanici@hedef_makine

# Artık şifresiz giriş yapabilirsiniz
ssh kullanici@hedef_makine
```

> [!TIP]
> `ssh-copy-id` komutu, public anahtarınızı hedef sistemin `~/.ssh/authorized_keys` dosyasına ekler.

### SCP ile Dosya Transferi

SCP (Secure Copy Protocol), SSH protokolü üzerinden güvenli dosya transferi sağlar. Cross-compile edilen programları hedef sisteme göndermek için sıkça kullanılır.

```bash
# Host'tan target'a dosya gönder
scp program.exe kullanici@192.168.7.2:~/

# Belirli dizine gönder
scp program.exe kullanici@192.168.7.2:/home/debian/bin/

# Dizin gönder (recursive)
scp -r proje_dizini/ kullanici@192.168.7.2:~/

# Target'tan host'a dosya al
scp kullanici@192.168.7.2:~/output.log ./

# Port belirterek
scp -P 2222 dosya kullanici@host:~/
```

---

## 🖥️ VNC ile Grafiksel Erişim

SSH yalnızca konsol erişimi sağlar. Pencere yöneticisi (masaüstü) olan sistemlere grafiksel olarak erişmek için VNC (Virtual Network Computing) kullanılır.

VNC kullanabilmek için:
1. Hedef sistemde grafik arayüz (Desktop Environment) yüklü olmalı
2. Hedef sistemde VNC server çalışıyor olmalı
3. Host sistemde VNC client yüklü olmalı

### VNC Server Kurulumu (Raspberry Pi)

Raspberry Pi OS'ta VNC server dahili olarak gelir, sadece etkinleştirilmesi gerekir:

**Grafik arayüzden:**
```
Preferences → Raspberry Pi Configuration → Interfaces → VNC → Enable
```

**Komut satırından:**
```bash
sudo raspi-config
# → Interface Options → VNC → Enable

# Manuel kurulum (gerekirse)
sudo apt install realvnc-vnc-server
sudo systemctl enable vncserver-x11-serviced
```

### VNC Client (Host)

```bash
# Linux'ta Remmina kurulumu
sudo apt install remmina remmina-plugin-vnc

# Windows/macOS için RealVNC Viewer indirin:
# https://www.realvnc.com/en/connect/download/viewer/
```

**Bağlantı:** `hostname:5900` veya `hostname:0`

### TightVNC (Alternatif)

TightVNC, Linux'ta yaygın kullanılan hafif bir VNC server'dır:

```bash
# Kurulum
sudo apt install tightvncserver

# İlk çalıştırma (şifre belirle)
vncserver :1

# Durdurmak için
vncserver -kill :1
```

---

## 📡 Diğer Erişim Yöntemleri

### Microsoft RDP (Remote Desktop Protocol)

Windows'un Remote Desktop bağlantısını Linux'a yöneltmek için xrdp kullanılabilir:

```bash
# Hedef sistemde kurulum
sudo apt install xrdp

# Servis başlat
sudo systemctl enable xrdp

# Windows'tan bağlantı:
# Uzak Masaüstü Bağlantısı (mstsc) → IP adresi gir
```

> [!NOTE]
> Linux dünyasında VNC, RDP'den daha hızlı çalışma eğilimindeyken, Windows dünyasında durum tam tersidir.

### SSHFS (SSH File System)

SSHFS, uzak sistemdeki bir dizini yerel sistemde bir dizin olarak bağlar. Dosya kopyaladığınızda otomatik olarak hedefe transfer edilir.

```bash
# Kurulum
sudo apt install sshfs

# Mount dizini oluştur
mkdir ~/bbb_home

# Bağla
sshfs debian@192.168.7.2:/home/debian ~/bbb_home

# Artık ~/bbb_home dizini BBB'deki /home/debian gibi çalışır
cp program ~/bbb_home/  # Otomatik transfer

# Bağlantıyı kopar
umount ~/bbb_home
```

---

## 🎛️ Sistem Yapılandırması

### raspi-config (Raspberry Pi)

Raspberry Pi'ın yapılandırma aracı:

```bash
sudo raspi-config
```

**Önemli menüler:**
- **System Options:** Hostname, kullanıcı, şifre
- **Interface Options:** SSH, VNC, I2C, SPI etkinleştirme
- **Localisation Options:** Locale, Timezone, Keyboard
- **Advanced Options:** RAM split, expand filesystem

### Locale Ayarları

Locale, sistem dilini, para birimi formatını ve karakter kodlamasını belirler. BBB'deki hazır Debian dağıtımı American English olarak ayarlıdır.

```bash
# İnteraktif locale ayarlama
sudo dpkg-reconfigure locales

# Listeden tr_TR.UTF-8 seçin
# Default locale olarak da tr_TR.UTF-8 belirleyin

# Manuel düzenleme
sudo nano /etc/default/locale
# LANG=tr_TR.UTF-8
```

Locale'in ayarlanması bazı mesajların Türkçe çıkmasını ve terminal giriş/çıkışlarının encoding'ini belirler. Ancak **klavye düzenini değiştirmez**.

### Klavye Ayarları

Klavye düzeni `/etc/default/keyboard` dosyasından ayarlanır:

```bash
# Dosyayı düzenle
sudo nano /etc/default/keyboard

# İçeriği şu şekilde değiştir:
XKBMODEL="pc105"
XKBLAYOUT="tr"    # "us" yerine "tr" yaz
XKBVARIANT=""
XKBOPTIONS=""
BACKSPACE="guess"

# Değişiklikleri uygula
sudo setupcon

# Veya yeniden başlat
sudo reboot
```

### Console Font Ayarı (BBB için)

BBB'deki Debian'ın varsayılan console font Türkçe karakterleri desteklemez. Bunu düzeltmek için:

```bash
# Console setup dosyasını düzenle
sudo nano /etc/default/console-setup

# CHARMAP satırını şu şekilde değiştir:
CHARMAP="UTF-8"

# Kaydet ve setupcon çalıştır
sudo setupcon
```

### Timezone Ayarı

```bash
# Timezone listele
timedatectl list-timezones | grep Istanbul

# İstanbul timezone'u ayarla
sudo timedatectl set-timezone Europe/Istanbul

# Doğrula
date
timedatectl
```

---

## 📦 Geliştirme Araçları Kurulumu

Gömülü Linux geliştirmesi için temel araçları hedef sisteme kurmanız gerekir:

```bash
# Sistem güncelleme
sudo apt update && sudo apt upgrade -y

# Temel yapı araçları
sudo apt install -y build-essential git vim nano

# Kernel headers (modül derleme için)
sudo apt install -y linux-headers-$(uname -r)

# GDB debugger
sudo apt install -y gdb

# Python geliştirme
sudo apt install -y python3-pip python3-dev

# GPIO kütüphaneleri (Raspberry Pi)
sudo apt install -y python3-gpiozero python3-rpi.gpio

# GPIO kütüphaneleri (BeagleBone)
sudo apt install -y python3-adafruit-bbio

# I2C araçları
sudo apt install -y i2c-tools

# Ağ araçları
sudo apt install -y net-tools curl wget
```

---

## 🔍 Faydalı Komutlar

### Sistem Bilgisi

```bash
# IP adresi
hostname -I
ip addr show

# Sistem bilgisi
uname -a
cat /etc/os-release
hostnamectl

# Uptime
uptime
```

### Donanım Bilgisi

```bash
# CPU
lscpu
cat /proc/cpuinfo | head -20

# Bellek
free -h
cat /proc/meminfo | head -10

# Disk
df -h
lsblk

# GPIO (BBB ve RPi)
cat /sys/kernel/debug/gpio
```

### Ağ Bilgisi

```bash
# Ağ arayüzleri
ip link show
ifconfig

# Bağlantı testi
ping -c 3 google.com

# Açık portlar
ss -tuln
```

---

## 🎯 Pratik Ödevler

1. Raspberry Pi veya BBB'ye Linux işletim sistemi kurun
2. SSH ile uzaktan bağlantı yapın
3. Türkçe klavye ve locale ayarlarını yapın
4. Geliştirme araçlarını kurun
5. `hello_embedded.c` programını hedef sistemde derleyin ve çalıştırın
6. SCP ile host'tan hedef sisteme dosya transfer edin
7. SSHFS ile dizin bağlayın

---

## 📚 Kaynaklar

- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [BeagleBoard Documentation](https://docs.beagleboard.org/)
- [OpenSSH Documentation](https://www.openssh.com/manual.html)
- [Debian Administrator's Handbook](https://debian-handbook.info/)

---

*Detaylı kurulum rehberleri için: `setup/` dizinine bakın*

*Sonraki ders: Boot Süreci ve Bootloader*
