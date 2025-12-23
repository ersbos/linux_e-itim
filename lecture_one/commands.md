# Ders 1 - Komut Referansı

## Sistem Bilgisi

```bash
# Kernel versiyonu
uname -r
# Örnek çıktı: 6.8.0-90-generic

# Tam sistem bilgisi
uname -a
# Örnek: Linux hostname 6.8.0-90-generic #91-Ubuntu SMP ...

# Makine mimarisi
uname -m
# x86_64 (PC), armv7l (BBB), aarch64 (RPi 64-bit)

# İşletim sistemi bilgisi
cat /etc/os-release
```

## CPU Bilgisi

```bash
# CPU detayları
cat /proc/cpuinfo

# Özet CPU bilgisi
lscpu

# CPU sayısı
nproc

# CPU sıcaklığı (RPi)
vcgencmd measure_temp
```

## Bellek Bilgisi

```bash
# RAM kullanımı (insan dostu)
free -h

# Detaylı bellek bilgisi
cat /proc/meminfo

# Swap kullanımı
swapon --show
```

## Disk / Depolama

```bash
# Disk kullanımı
df -h

# Blok cihazlar
lsblk

# SD kart / eMMC bilgisi
fdisk -l /dev/mmcblk0
```

## Ağ

```bash
# IP adresi
hostname -I

# Ağ arayüzleri
ip addr show

# Ağ bağlantı testi
ping -c 3 google.com
```

## SSH Bağlantısı

```bash
# Raspberry Pi'a bağlan
ssh pi@raspberrypi.local
ssh pi@192.168.1.X

# BeagleBone Black'e bağlan (USB)
ssh debian@192.168.7.2
# Şifre: temppwd

# Dosya kopyala (SCP)
scp file.txt user@host:/path/
```

## Derleme

```bash
# Host'ta derleme
gcc -o program program.c

# Cross-compile (BBB için)
arm-linux-gnueabihf-gcc -o program program.c

# Cross-compile (RPi 64-bit için)
aarch64-linux-gnu-gcc -o program program.c
```

---

*Detaylı açıklamalar için: notes.md*
