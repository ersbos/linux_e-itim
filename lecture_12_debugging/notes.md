# Ders 12: Debugging ve Profiling

> **Konu:** Gömülü Linux Sistemlerinde Hata Ayıklama ve Performans Analizi
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Debugging seviyeleri ve yöntemleri
- [x] printf/printk debugging
- [x] GDB ile debugging
- [x] Remote debugging (gdbserver)
- [x] Core dump analizi
- [x] strace ve ltrace
- [x] Profiling araçları (time, perf, valgrind)
- [x] Kernel debugging (ftrace, dynamic debug)
- [x] Binary analiz araçları

---

## 🔍 Debugging Seviyeleri

Gömülü Linux sistemlerinde debugging farklı seviyelerde gerçekleştirilebilir. Her seviyenin kendi araçları ve yaklaşımları vardır:

| Seviye | Araçlar | Kullanım Alanı |
|--------|---------|----------------|
| **User-space** | GDB, strace, ltrace, valgrind | Uygulama debugging |
| **Kernel-space** | printk, KGDB, ftrace, perf | Kernel/driver debugging |
| **Hardware** | JTAG, Serial console, LED | Low-level debugging |

### Debugging Yaklaşımları

Debugging temel olarak iki kategoriye ayrılır:

**Post-mortem Debugging:** Program çöktükten sonra, core dump dosyası veya log dosyaları incelenerek hata analizi yapılır. Bu yöntem production ortamlarında sık kullanılır.

**Interactive Debugging:** Program çalışırken breakpoint'ler koyarak, değişkenleri izleyerek adım adım debugging yapılır. GDB bu kategorinin en önemli aracıdır.

---

## 📝 printf/printk Debugging

En basit ve yaygın debugging yöntemi, programın kritik noktalarına print ifadeleri eklemektir. Basit görünmesine rağmen, birçok karmaşık hatanın tespitinde etkilidir.

### User-Space printf Debugging

```c
/* debug_example.c */
#include <stdio.h>

#define DEBUG 1

#if DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

int main(void)
{
    int value = 42;
    
    DEBUG_PRINT("Program başlıyor");
    DEBUG_PRINT("value = %d", value);
    
    /* ... */
    
    DEBUG_PRINT("Fonksiyon çağrılıyor");
    
    return 0;
}
```

**Avantajları:**
- Basit ve hızlı
- Herhangi bir araç gerektirmez
- Production'da koşullu olarak devre dışı bırakılabilir

**Dezavantajları:**
- Kod değişikliği ve yeniden derleme gerektirir
- Çok fazla log, performansı etkiler
- Timing-sensitive hataları gizleyebilir

### Kernel-Space printk Debugging

Kernel modüllerinde `printf` yerine `printk` ailesi fonksiyonlar kullanılır:

```c
/* Kernel modülünde */
#include <linux/kernel.h>
#include <linux/module.h>

/* Modern pr_* makroları (önerilen) */
pr_emerg("Emergency: System is unusable\n");      /* 0 */
pr_alert("Alert: Action must be taken\n");        /* 1 */
pr_crit("Critical: Critical conditions\n");       /* 2 */
pr_err("Error: Error conditions\n");              /* 3 */
pr_warn("Warning: Warning conditions\n");         /* 4 */
pr_notice("Notice: Normal but significant\n");    /* 5 */
pr_info("Info: Informational\n");                 /* 6 */
pr_debug("Debug: Debug-level messages\n");        /* 7 */

/* Eski printk stili */
printk(KERN_INFO "Info message\n");
printk(KERN_ERR "Error message\n");
```

**Log Seviyelerinin Anlamları:**

| Seviye | Makro | Açıklama |
|--------|-------|----------|
| 0 | `KERN_EMERG` | Sistem kullanılamaz |
| 1 | `KERN_ALERT` | Acil müdahale gerekli |
| 2 | `KERN_CRIT` | Kritik koşullar |
| 3 | `KERN_ERR` | Hata koşulları |
| 4 | `KERN_WARNING` | Uyarı koşulları |
| 5 | `KERN_NOTICE` | Normal ama önemli |
| 6 | `KERN_INFO` | Bilgilendirme |
| 7 | `KERN_DEBUG` | Debug mesajları |

### Log Görüntüleme

```bash
# Kernel mesajlarını görüntüle
dmesg
dmesg | tail -20      # Son 20 satır
dmesg -T              # Tarih/saat ile
dmesg -w              # Canlı takip (follow)
dmesg -l err,warn     # Sadece error ve warning

# Syslog (geleneksel)
tail -f /var/log/syslog
tail -f /var/log/messages

# Systemd journal
journalctl -f         # Canlı takip
journalctl -k         # Sadece kernel mesajları
journalctl -b         # Bu boot'tan itibaren
journalctl --since "5 minutes ago"

# Log seviyesi ayarlama
echo 7 > /proc/sys/kernel/printk   # Tüm mesajları göster
```

---

## 🐛 GDB ile Debugging

GDB (GNU Debugger), UNIX/Linux dünyasında en yaygın kullanılan debugger'dır. Program çalışırken veya core dump üzerinde analiz yapabilir.

### Derleme Ayarları

Debugging için programın debug sembolleri ile derlenmesi gerekir:

```bash
# Debug sembolleri ile derle
gcc -g -O0 -o program program.c

# Optimizasyon olmadan (debugging için önerilen)
gcc -g -O0 -Wall -o program program.c

# Cross-compile (ARM için)
arm-linux-gnueabihf-gcc -g -O0 -o program program.c
```

**-g flag'i:** Debug bilgilerini (sembol tablosu, satır numaraları) binary'ye ekler.

**-O0 flag'i:** Optimizasyonu kapatır. Optimizasyon, değişkenleri ve kod akışını değiştireceği için debugging'i zorlaştırır.

### GDB Temel Kullanım

```bash
# GDB'yi başlat
gdb ./program

# Argümanlarla programa başla
gdb --args ./program arg1 arg2
```

### GDB Komutları

**Program Kontrolü:**
```gdb
(gdb) run                     # Programı çalıştır
(gdb) run arg1 arg2           # Argümanlarla çalıştır
(gdb) start                   # main'de dur
(gdb) continue                # Devam et (c)
(gdb) next                    # Sonraki satır, fonksiyona girme (n)
(gdb) step                    # Sonraki satır, fonksiyona gir (s)
(gdb) finish                  # Mevcut fonksiyondan çık
(gdb) kill                    # Programı sonlandır
(gdb) quit                    # GDB'den çık (q)
```

**Breakpoint'ler:**
```gdb
(gdb) break main              # main fonksiyonunda dur
(gdb) break file.c:42         # file.c'nin 42. satırında dur
(gdb) break function_name     # Fonksiyon başında dur
(gdb) break *0x08048000       # Belirli adreste dur
(gdb) info breakpoints        # Breakpoint'leri listele
(gdb) delete 1                # 1 numaralı breakpoint'i sil
(gdb) disable 2               # 2 numaralı breakpoint'i devre dışı
(gdb) enable 2                # 2 numaralı breakpoint'i etkinleştir
(gdb) clear function_name     # Fonksiyondaki breakpoint'i sil
```

**Değişken ve Bellek İnceleme:**
```gdb
(gdb) print variable          # Değişken değerini yazdır (p)
(gdb) print/x variable        # Hex formatında
(gdb) print/t variable        # Binary formatında
(gdb) print *pointer          # Pointer değerini yazdır
(gdb) print array[5]          # Dizi elemanı
(gdb) print sizeof(struct)    # Yapı boyutu

(gdb) display variable        # Her adımda otomatik göster
(gdb) undisplay 1             # 1 numaralı display'i kaldır

(gdb) x/10x $sp               # Stack'ten 10 word hex göster
(gdb) x/s 0x08048000          # String olarak göster
(gdb) x/i $pc                 # Instruction olarak göster
```

**Stack ve Backtrace:**
```gdb
(gdb) backtrace               # Stack trace (bt)
(gdb) backtrace full          # Yerel değişkenlerle
(gdb) frame 2                 # 2 numaralı frame'e geç
(gdb) up                      # Üst frame'e geç
(gdb) down                    # Alt frame'e geç
(gdb) info frame              # Mevcut frame bilgisi
(gdb) info locals             # Yerel değişkenler
(gdb) info args               # Fonksiyon argümanları
```

**Kaynak Kod Görüntüleme:**
```gdb
(gdb) list                    # Kaynak kodu göster (l)
(gdb) list function_name      # Fonksiyonun kodunu göster
(gdb) list 1,50               # 1-50 satırları göster
```

### Watchpoint'ler

Bir değişkenin değeri değiştiğinde durma:

```gdb
(gdb) watch variable          # variable değişince dur
(gdb) rwatch variable         # variable okunduğunda dur
(gdb) awatch variable         # Her erişimde dur
(gdb) info watchpoints        # Watchpoint'leri listele
```

---

## 🌐 Remote Debugging (gdbserver)

Gömülü sistemlerde hedef cihaz (target) sınırlı kaynaklara sahip olduğundan, debugging genellikle host makineden uzaktan yapılır.

### Mimari

```
┌─────────────┐              ┌─────────────┐
│    HOST     │   TCP/IP     │   TARGET    │
│   (x86)     │◄────────────►│   (ARM)     │
│             │    :2345     │             │
│  arm-gdb    │              │  gdbserver  │
│  program    │              │  program    │
└─────────────┘              └─────────────┘
```

### Target Tarafında (BBB/RPi)

```bash
# gdbserver'ı başlat (port 2345'te dinle)
gdbserver :2345 ./program

# Veya çalışan programa bağlan
gdbserver :2345 --attach <PID>
```

### Host Tarafında

```bash
# Cross-debugger'ı başlat
arm-linux-gnueabihf-gdb ./program

# Target'a bağlan
(gdb) set sysroot /path/to/target/rootfs
(gdb) target remote 192.168.7.2:2345

# Artık normal GDB komutları kullanılabilir
(gdb) break main
(gdb) continue
(gdb) print variable
```

**Sysroot:** Target sistemin kütüphanelerinin host'ta bulunduğu dizin. Debugging sırasında sembol çözümlemesi için gereklidir.

---

## 💥 Core Dump Analizi

Bazı sinyaller (SIGSEGV, SIGABRT, SIGFPE gibi) programın sonlanmasıyla birlikte **core dosyası** oluşturulmasına neden olur. Core dosyası, programın çöktüğü andaki bellek durumunun bir kopyasıdır ve post-mortem analiz için kullanılır.

### Core Dump Etkinleştirme

```bash
# Mevcut limitleri görüntüle
ulimit -a

# Core dosyası boyut limitini kaldır
ulimit -c unlimited

# Kalıcı ayar için /etc/security/limits.conf'a ekle:
# *    soft    core    unlimited
# *    hard    core    unlimited
```

### Core Dosyası Konumu

Linux sistemlerinde core dosyasının nerede oluşturulacağı `/proc/sys/kernel/core_pattern` dosyasında belirlenir:

```bash
# Core pattern'i görüntüle
cat /proc/sys/kernel/core_pattern

# Olası çıktılar:
# core                                    # Çalışma dizininde "core" olarak
# /var/lib/systemd/coredump/core.%e.%p   # systemd-coredump kullanıyor
# |/usr/share/apport/apport %p %s ...    # apport kullanıyor
```

**Format belirteçleri:**

| Belirteç | Anlamı |
|----------|--------|
| %p | PID |
| %e | Executable adı |
| %t | Timestamp |
| %u | UID |
| %g | GID |
| %s | Sinyal numarası |

### coredumpctl (Systemd)

Modern Linux sistemlerinde core dump yönetimi için `coredumpctl` kullanılır:

```bash
# coredumpctl kur (gerekirse)
sudo apt install systemd-coredump

# Core dump listesi
coredumpctl list

# Son core dump hakkında bilgi
coredumpctl info

# Son core dump'ı GDB ile aç
coredumpctl gdb

# Belirli bir core dump'ı GDB ile aç
coredumpctl gdb <PID>

# Core dump'ı dosyaya çıkar
coredumpctl dump -o /tmp/core_backup
```

### GDB ile Core Dump Analizi

```bash
# Geleneksel yöntem
gdb ./program core

# Core dump analiz komutları
(gdb) backtrace          # Çökme noktasındaki stack trace
(gdb) info registers     # CPU yazmaçları
(gdb) info threads       # Thread bilgileri
(gdb) frame 0            # En üst frame'e git
(gdb) list               # Çökme noktasındaki kaynak kod
(gdb) print variable     # Değişken değerleri
```

**Örnek Core Dump Analizi:**
```c
/* segfault.c - Core dump oluşturan program */
#include <stdio.h>

int main(void)
{
    char *ptr = NULL;
    *ptr = 'x';    /* SIGSEGV - NULL pointer dereference */
    return 0;
}
```

```bash
$ gcc -g -o segfault segfault.c
$ ulimit -c unlimited
$ ./segfault
Segmentation fault (core dumped)

$ coredumpctl gdb
...
Core was generated by `./segfault'.
Program terminated with signal SIGSEGV, Segmentation fault.
#0  0x0000555555555145 in main () at segfault.c:6
6       *ptr = 'x';
(gdb) print ptr
$1 = 0x0
(gdb) backtrace
#0  0x0000555555555145 in main () at segfault.c:6
```

---

## 📊 strace ve ltrace

### strace - Sistem Çağrıları İzleme

strace, bir programın yaptığı tüm sistem çağrılarını (system calls) izler. Dosya erişimi, ağ işlemleri, süreç yönetimi gibi kernel ile olan tüm etkileşimler görülebilir.

```bash
# Temel kullanım
strace ./program

# Çıktıyı dosyaya yaz
strace -o trace.log ./program

# Sadece belirli sistem çağrıları
strace -e trace=open,read,write ./program
strace -e trace=file ./program        # Dosya işlemleri
strace -e trace=network ./program     # Ağ işlemleri
strace -e trace=process ./program     # Süreç işlemleri
strace -e trace=memory ./program      # Bellek işlemleri

# Çalışan prosese bağlan
strace -p <PID>

# Alt prosesleri de izle
strace -f ./program

# Zaman bilgisi ekle
strace -t ./program      # Saat:Dakika:Saniye
strace -tt ./program     # Mikrosaniye ile
strace -r ./program      # Göreli zaman

# İstatistik özeti
strace -c ./program

# Çağrı argümanlarının tam değerlerini göster
strace -s 256 ./program  # String'leri 256 karaktere kadar göster
```

**Örnek strace Çıktısı:**
```bash
$ strace ./hello
execve("./hello", ["./hello"], 0x7ffc... /* 52 vars */) = 0
brk(NULL)                               = 0x5555557a8000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
...
write(1, "Hello World!\n", 13)          = 13
exit_group(0)                           = ?
+++ exited with 0 +++
```

**strace ile Hata Ayıklama Senaryoları:**

| Senaryo | strace Kullanımı |
|---------|------------------|
| Dosya bulunamıyor | `strace -e trace=open,openat ./program` |
| Permission denied | `strace -e trace=access,open ./program` |
| Network problemi | `strace -e trace=network ./program` |
| Nereye takılıyor? | `strace ./program` ve son çağrıya bak |

### ltrace - Kütüphane Çağrıları İzleme

ltrace, programın çağırdığı paylaşımlı kütüphane (shared library) fonksiyonlarını izler:

```bash
# Temel kullanım
ltrace ./program

# Belirli fonksiyonlar
ltrace -e malloc+free ./program
ltrace -e printf+puts ./program

# Sistem çağrılarını da göster
ltrace -S ./program

# Çalışan prosese bağlan
ltrace -p <PID>

# Parametre detayları
ltrace -s 256 ./program
```

**Örnek ltrace Çıktısı:**
```bash
$ ltrace ./hello
__libc_start_main(0x401136, 1, 0x7ffc..., 
puts("Hello World!")                     = 13
+++ exited (status 0) +++
```

---

## ⚡ Profiling Araçları

Profiling, programın performans karakteristiklerini analiz etmektir: hangi fonksiyonlar en çok CPU kullanıyor, bellek kullanımı nasıl, darboğazlar nerede?

### time Komutu

En basit performans ölçümü:

```bash
$ time ./program
real    0m1.234s   # Gerçek (duvar saati) zaman
user    0m0.567s   # User mode CPU zamanı
sys     0m0.089s   # Kernel mode CPU zamanı

# Daha detaylı bilgi
/usr/bin/time -v ./program
```

**Çıktı Yorumlama:**
- **real > user + sys:** I/O beklemesi veya başka süreç beklentisi var
- **user >> sys:** CPU-bound uygulama
- **sys >> user:** I/O-bound uygulama

### perf - Linux Performance Counter

perf, Linux çekirdeğine entegre güçlü bir profiling aracıdır:

```bash
# Kurulum
sudo apt install linux-tools-common linux-tools-$(uname -r)

# Temel istatistikler
sudo perf stat ./program

# CPU sampling (profiling)
sudo perf record -g ./program
sudo perf report

# Canlı analiz
sudo perf top

# Fonksiyon bazlı profiling
sudo perf record -g --call-graph dwarf ./program
sudo perf report --sort=dso,symbol
```

**perf stat Örnek Çıktısı:**
```
Performance counter stats for './program':
         1,234.56 msec task-clock
            1,234      context-switches
               56      cpu-migrations
            2,345      page-faults
    3,456,789,012      cycles
    1,234,567,890      instructions              # 0.36 insn/cycle
      234,567,890      branches
        1,234,567      branch-misses             # 0.53%
```

### Valgrind - Bellek Analizi

Valgrind, bellek hatalarını ve sızıntılarını tespit eden güçlü bir araçtır:

```bash
# Bellek sızıntısı tespiti
valgrind --leak-check=full ./program

# Detaylı analiz
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./program

# Başlatılmamış değişken kullanımı
valgrind --track-origins=yes ./program

# Cache profiling
valgrind --tool=cachegrind ./program

# Call graph profiling
valgrind --tool=callgrind ./program
kcachegrind callgrind.out.*   # GUI ile görüntüle

# Heap profiling
valgrind --tool=massif ./program
ms_print massif.out.*
```

**Valgrind Hata Türleri:**

| Hata | Açıklama |
|------|----------|
| Invalid read/write | Geçersiz bellek erişimi |
| Conditional jump on uninitialized | Başlatılmamış değişken kullanımı |
| Memory leak | Bellek sızıntısı |
| Invalid free | Çift free veya geçersiz free |

---

## 🧠 Kernel Debugging

### Dynamic Debug

Runtime'da debug mesajlarını açıp kapatma:

```bash
# Dynamic debug kontrol dosyası
cat /sys/kernel/debug/dynamic_debug/control

# Modül bazlı debug aç
echo 'module mymodule +p' > /sys/kernel/debug/dynamic_debug/control

# Fonksiyon bazlı debug aç
echo 'func my_function +p' > /sys/kernel/debug/dynamic_debug/control

# Dosya bazlı debug aç
echo 'file mydriver.c +p' > /sys/kernel/debug/dynamic_debug/control

# Tüm debug'ları kapat
echo '-p' > /sys/kernel/debug/dynamic_debug/control
```

### ftrace - Kernel Function Tracer

```bash
# Mevcut tracer'ları listele
cat /sys/kernel/debug/tracing/available_tracers

# Function tracer'ı aktifle
echo function > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on

# Trace'i oku
cat /sys/kernel/debug/tracing/trace

# Tracing'i durdur
echo 0 > /sys/kernel/debug/tracing/tracing_on

# Belirli fonksiyonları izle
echo 'do_sys_open' > /sys/kernel/debug/tracing/set_ftrace_filter
echo 1 > /sys/kernel/debug/tracing/tracing_on

# Trace'i temizle
echo > /sys/kernel/debug/tracing/trace
```

### KGDB (Kernel GDB)

Kernel'ı GDB ile debug etmek için:

```bash
# Kernel boot parametrelerine ekle:
# kgdboc=ttyS0,115200 kgdbwait

# Host tarafında
gdb vmlinux
(gdb) target remote /dev/ttyUSB0
(gdb) continue
```

---

## 🔧 Binary Analiz Araçları

### objdump

Binary dosyayı disassemble etme:

```bash
# Disassembly
objdump -d program | less

# Kaynak kodla birlikte (-g ile derlenmişse)
objdump -S program | less

# Sembol tablosu
objdump -t program

# Tüm section'lar
objdump -h program
```

### readelf

ELF dosya formatını analiz etme:

```bash
# Tüm bilgiler
readelf -a program

# Header bilgisi
readelf -h program

# Section'lar
readelf -S program

# Sembol tablosu
readelf -s program

# Dynamic bağımlılıklar
readelf -d program
```

### nm

Sembol tablosunu görüntüleme:

```bash
# Sembolleri listele
nm program

# Undefined sembolleri göster
nm -u program

# Sıralı liste
nm --numeric-sort program

# Demangle (C++ için)
nm --demangle program
```

### strings

Binary'deki okunabilir string'leri bulma:

```bash
# Tüm string'ler
strings program

# Minimum uzunluk
strings -n 10 program

# Dosya offset'leri ile
strings -t x program
```

### ldd

Dinamik kütüphane bağımlılıklarını görme:

```bash
ldd ./program
```

---

## 📝 Debugging Cheatsheet

```bash
# === Process Bilgileri ===
ps aux | grep myprogram        # Process listesi
top -p <PID>                   # CPU/Memory kullanımı
htop                           # İnteraktif process viewer

# === Dosya Erişimi ===
lsof -p <PID>                  # Açık dosyalar
fuser -v /path/to/file         # Dosyayı kullanan process

# === Bellek ===
cat /proc/<PID>/maps           # Bellek haritası
cat /proc/<PID>/status         # Process durumu
pmap <PID>                     # Bellek haritası özet

# === Signal ===
kill -l                        # Signal listesi
kill -SIGSEGV <PID>            # Crash testi

# === Network ===
netstat -tulpn                 # Açık portlar
ss -tulpn                      # Socket statistics
tcpdump -i eth0                # Paket yakalama

# === Cross-Debug ===
arm-linux-gnueabihf-gdb program
(gdb) set sysroot /path/to/rootfs
(gdb) target remote 192.168.7.2:2345
```

---

## 🎯 Pratik Ödevler

1. Bir programı `-g` ile derle ve GDB ile debug et
2. Breakpoint koy, değişkenleri incele, stack trace al
3. Kasıtlı segfault oluştur ve core dump analizi yap
4. `strace` ile bir programın sistem çağrılarını izle
5. `valgrind` ile bellek sızıntısı tespit et
6. `perf` ile basit profiling yap
7. Remote debugging için gdbserver kullan

---

## 📚 Kaynaklar

- [GDB Manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)
- [Valgrind Manual](https://valgrind.org/docs/manual/)
- [Linux Perf Wiki](https://perf.wiki.kernel.org/)
- [Bootlin Debugging Training](https://bootlin.com/training/debugging/)

---

*Kurs sonu! Tebrikler!* 🎉
