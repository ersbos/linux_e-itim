# Linux Komutları Cheatsheet

## 📁 Dosya ve Dizin İşlemleri

```bash
# Dizin listeleme
ls -la                    # Detaylı liste (gizli dosyalar dahil)
ls -lh                    # İnsan okunabilir boyutlar

# Dizin değiştirme
cd /path/to/dir           # Dizine git
cd ~                      # Home dizinine git
cd -                      # Önceki dizine geri dön

# Dosya/dizin oluşturma
mkdir -p a/b/c            # İç içe dizinler oluştur
touch file.txt            # Boş dosya oluştur

# Kopyalama/taşıma/silme
cp -r src/ dst/           # Dizin kopyala
mv old.txt new.txt        # Taşı/yeniden adlandır
rm -rf dir/               # Dizin sil (dikkatli!)

# Dosya arama
find . -name "*.c"        # İsme göre bul
find . -type f -size +1M  # 1MB'den büyük dosyalar
locate filename           # Hızlı arama (veritabanı)
```

## 📖 Dosya İçerik Görüntüleme

```bash
cat file.txt              # Tüm içerik
head -n 20 file.txt       # İlk 20 satır
tail -n 20 file.txt       # Son 20 satır
tail -f /var/log/syslog   # Canlı takip
less file.txt             # Sayfalama ile görüntüle
grep "pattern" file.txt   # Desen ara
grep -r "pattern" dir/    # Dizinde ara
```

## 🖥️ Sistem Bilgisi

```bash
# Kernel ve OS
uname -a                  # Tam sistem bilgisi
uname -r                  # Kernel versiyonu
cat /etc/os-release       # Dağıtım bilgisi

# CPU
cat /proc/cpuinfo         # CPU detayları
lscpu                     # CPU özeti
nproc                     # CPU sayısı

# Bellek
free -h                   # RAM kullanımı
cat /proc/meminfo         # Detaylı bellek

# Disk
df -h                     # Disk kullanımı
lsblk                     # Blok cihazlar
mount                     # Mount noktaları

# Süreçler
ps aux                    # Tüm süreçler
top                       # Canlı süreç izleme
htop                      # Gelişmiş izleme
kill -9 PID               # Süreç sonlandır
```

## 🔐 Kullanıcı ve İzinler

```bash
# Kullanıcı bilgisi
whoami                    # Aktif kullanıcı
id                        # UID/GID bilgisi
groups                    # Grup üyelikleri

# İzinler
chmod 755 file            # rwxr-xr-x
chmod +x script.sh        # Çalıştırma izni ekle
chown user:group file     # Sahiplik değiştir

# Süper kullanıcı
sudo command              # Root olarak çalıştır
sudo -i                   # Root shell aç
```

## 🌐 Ağ

```bash
# IP ve arayüzler
ip addr show              # IP adresleri
ip route                  # Yönlendirme tablosu
hostname -I               # Sadece IP

# Bağlantı testi
ping -c 4 google.com      # ICMP ping
curl ifconfig.me          # Harici IP
wget url                  # Dosya indir

# Portlar ve bağlantılar
ss -tulpn                 # Dinleyen portlar
netstat -tulpn            # Alternatif komut
```

## 📦 Paket Yönetimi (Debian/Ubuntu)

```bash
sudo apt update           # Paket listesi güncelle
sudo apt upgrade          # Paketleri güncelle
sudo apt install pkg      # Paket kur
sudo apt remove pkg       # Paket kaldır
sudo apt search name      # Paket ara
apt list --installed      # Kurulu paketler
```

## 🔧 Servis Yönetimi (systemd)

```bash
systemctl status sshd     # Servis durumu
systemctl start sshd      # Servisi başlat
systemctl stop sshd       # Servisi durdur
systemctl restart sshd    # Yeniden başlat
systemctl enable sshd     # Açılışta başlat
systemctl disable sshd    # Açılıştan kaldır
journalctl -u sshd        # Servis logları
```

## 💻 SSH ve Uzak Erişim

```bash
# Bağlantı
ssh user@host             # SSH bağlantısı
ssh -p 2222 user@host     # Farklı port
ssh -i key.pem user@host  # Anahtar ile

# Dosya transferi
scp file.txt user@host:/path/
scp user@host:/path/file.txt ./
scp -r dir/ user@host:/path/

# Tünel
ssh -L 8080:localhost:80 user@host
```

## 📝 Text Editörler

```bash
# Vim
vim file.txt
# i: insert mode, Esc: normal mode
# :w kaydet, :q çık, :wq kaydet-çık, :q! zorla çık

# Nano
nano file.txt
# Ctrl+O: kaydet, Ctrl+X: çık
```

## 🔨 Derleme (GCC)

```bash
# Basit derleme
gcc -o output source.c

# Debug bilgisi ile
gcc -g -o output source.c

# Optimizasyon ile
gcc -O2 -o output source.c

# Cross-compile
arm-linux-gnueabihf-gcc -o output source.c
```

## 📊 Log ve Hata Ayıklama

```bash
# Sistem logları
cat /var/log/syslog
cat /var/log/kern.log
dmesg                     # Kernel mesajları
dmesg | tail -20          # Son 20 kernel mesajı

# Gerçek zamanlı izleme
tail -f /var/log/syslog
dmesg -w
```

---

*Son güncelleme: 2025-12-23*
