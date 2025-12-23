# Ders 12: Debugging ve Profiling

> **Konu:** Gömülü Linux Sistemlerinde Hata Ayıklama ve Performans Analizi

---

## 📋 Bu Derste İşlenenler

- [x] printf/printk debugging
- [x] GDB ile debug
- [x] Remote debugging (gdbserver)
- [x] strace ve ltrace
- [x] Profiling araçları
- [x] Kernel debugging

---

## 🔍 Debugging Seviyeleri

| Seviye | Araçlar | Kullanım |
|--------|---------|----------|
| **User-space** | GDB, strace, ltrace | Uygulama debug |
| **Kernel-space** | printk, KGDB, ftrace | Kernel/driver debug |
| **Hardware** | JTAG, Serial | Low-level debug |

---

## 📝 printk Debugging

### Kernel Logging

```c
/* Kernel modülünde */
pr_info("Value = %d\n", value);
pr_err("Error occurred!\n");
pr_debug("Debug info\n");

/* Log seviyeleri */
printk(KERN_EMERG "Emergency!\n");   /* 0 */
printk(KERN_ALERT "Alert!\n");       /* 1 */
printk(KERN_CRIT "Critical!\n");     /* 2 */
printk(KERN_ERR "Error!\n");         /* 3 */
printk(KERN_WARNING "Warning!\n");   /* 4 */
printk(KERN_NOTICE "Notice!\n");     /* 5 */
printk(KERN_INFO "Info!\n");         /* 6 */
printk(KERN_DEBUG "Debug!\n");       /* 7 */
```

### Log Görüntüleme

```bash
# Kernel mesajları
dmesg | tail -20

# Canlı takip
dmesg -w

# Syslog
tail -f /var/log/syslog

# Journal (systemd)
journalctl -f
journalctl -k  # Sadece kernel
```

---

## 🐛 GDB ile Debugging

### Temel Kullanım

```bash
# Debug sembolü ile derle
gcc -g -o program program.c

# GDB başlat
gdb ./program

# Temel komutlar:
(gdb) run                    # Çalıştır
(gdb) break main             # Breakpoint
(gdb) break file.c:42        # Satır breakpoint
(gdb) continue               # Devam et
(gdb) next                   # Sonraki satır (step over)
(gdb) step                   # İçine gir (step into)
(gdb) print variable         # Değişken yazdır
(gdb) backtrace             # Stack trace
(gdb) quit                   # Çık
```

### Remote Debugging (gdbserver)

```bash
# Target'ta (BBB/RPi):
gdbserver :2345 ./program

# Host'ta:
arm-linux-gnueabihf-gdb ./program
(gdb) target remote 192.168.7.2:2345
(gdb) break main
(gdb) continue
```

### Core Dump Analizi

```bash
# Core dump etkinleştir
ulimit -c unlimited

# Program çöktüğünde
gdb ./program core

# Analiz
(gdb) backtrace
(gdb) info registers
```

---

## 📊 strace ve ltrace

### strace - Sistem Çağrıları

```bash
# Tüm sistem çağrıları
strace ./program

# Sadece dosya işlemleri
strace -e trace=open,read,write ./program

# Network işlemleri
strace -e trace=network ./program

# Çalışan process
strace -p <PID>

# İstatistik
strace -c ./program
```

### ltrace - Kütüphane Çağrıları

```bash
# Library çağrıları
ltrace ./program

# Belirli kütüphane
ltrace -e malloc+free ./program
```

### Örnek strace Çıktısı

```bash
$ strace ./hello
execve("./hello", ["./hello"], ...) = 0
...
write(1, "Hello World!\n", 13)         = 13
exit_group(0)                          = ?
```

---

## ⚡ Profiling Araçları

### time Komutu

```bash
# Basit zamanlama
time ./program

# Çıktı:
# real    0m1.234s   (duvar saati)
# user    0m0.123s   (kullanıcı modu)
# sys     0m0.012s   (kernel modu)
```

### perf (Linux Performance Counter)

```bash
# Kurulum
sudo apt install linux-tools-common linux-tools-$(uname -r)

# Temel profiling
sudo perf stat ./program

# CPU sampling
sudo perf record -g ./program
sudo perf report

# Live analiz
sudo perf top

# Flame graph (görselleştirme)
sudo perf record -F 99 -g -- ./program
sudo perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > out.svg
```

### Valgrind - Bellek Analizi

```bash
# Bellek sızıntısı tespiti
valgrind --leak-check=full ./program

# Detaylı analiz
valgrind --leak-check=full --show-leak-kinds=all ./program

# Cache profiling
valgrind --tool=cachegrind ./program
```

---

## 🧠 Kernel Debugging

### Dynamic Debug

```bash
# Dinamik debug mesajları aç
echo 'module mymodule +p' > /sys/kernel/debug/dynamic_debug/control

# Fonksiyon bazlı
echo 'func my_function +p' > /sys/kernel/debug/dynamic_debug/control
```

### ftrace - Kernel Tracer

```bash
# Mevcut tracer'lar
cat /sys/kernel/debug/tracing/available_tracers

# Function tracer aç
echo function > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on

# Trace oku
cat /sys/kernel/debug/tracing/trace

# Kapat
echo 0 > /sys/kernel/debug/tracing/tracing_on
```

### KGDB (Kernel GDB)

```bash
# Boot parametreleri
# kgdboc=ttyS0,115200 kgdbwait

# Host'ta
gdb vmlinux
(gdb) target remote /dev/ttyUSB0
```

---

## 🛠️ Debugging Cheatsheet

### Sık Kullanılan Komutlar

```bash
# Process bilgisi
ps aux | grep myprogram
top -p <PID>

# Açık dosyalar
lsof -p <PID>

# Bellek haritası
cat /proc/<PID>/maps

# Sinyal gönder
kill -SIGSEGV <PID>   # Crash test

# Object dosyayı incele
objdump -d program | head -100
readelf -a program
strings program | grep -i pass
```

### Cross-Debug Setup

```bash
# Host'ta (x86_64)
arm-linux-gnueabihf-gdb program

# Target bağlantısı
(gdb) set sysroot /path/to/target/rootfs
(gdb) target remote 192.168.7.2:2345
```

---

## 🎯 Pratik

1. Bir programı `-g` ile derle ve GDB ile debug et
2. `strace` ile sistem çağrılarını izle
3. Kernel modülünde `pr_debug` kullan
4. `valgrind` ile bellek sızıntısı ara
5. `perf` ile profiling yap

---

*Kurs sonu! Tebrikler!* 🎉
