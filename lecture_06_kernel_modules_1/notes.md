# Ders 6: Kernel Modüller ve Aygıt Sürücüler (Bölüm 1)

> **Konu:** Çekirdek Modülü Temelleri, printk, Modül Parametreleri
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] User Mode vs Kernel Mode (detaylı)
- [x] Çekirdek modülü vs Aygıt sürücü farkı
- [x] Çekirdek modülü yapısı
- [x] Derleme gereksinimleri
- [x] Makefile ve KBuild sistemi
- [x] printk ve log seviyeleri
- [x] module_init / module_exit makroları
- [x] static, __init, __exit kullanımı
- [x] Modül parametreleri (module_param)
- [x] sysfs ile modül bilgileri
- [x] Otomatik modül yükleme

---

## 🔐 User Mode vs Kernel Mode (Detaylı)

Modern işlemciler (x86, ARM) koruma mekanizmasına sahiptir. Bu mekanizma iki farklı çalışma modunu tanımlar:

### User Mode (Kullanıcı Modu)

- Normal programlar burada çalışır
- Belleğin **her yerine erişemez**
- Her makine komutunu **kullanamaz**
- Donanıma **doğrudan erişemez**
- `sudo` ile çalıştırsanız bile **User Mode**'da kalır!

```
                    ┌─────────────────────────────────────┐
                    │         USER MODE                    │
                    │  ┌─────┐  ┌─────┐  ┌─────┐          │
                    │  │bash │  │gcc  │  │vim  │  ...     │
                    │  └──┬──┘  └──┬──┘  └──┬──┘          │
                    │     │       │       │                │
                    │     └───────┼───────┘                │
                    │             │ System Calls           │
                    │             ▼                        │
┌───────────────────┼─────────────────────────────────────┼───────────────────┐
│                   │         KERNEL MODE                  │                   │
│                   │  ┌────────────────────────────┐     │                   │
│   Koruma          │  │      Linux Kernel           │     │    Tam Erişim    │
│   Mekanizması     │  │  ┌─────────┐ ┌──────────┐  │     │    (Bellek,      │
│   YOK!            │  │  │ Modules │ │ Drivers  │  │     │     Komutlar)    │
│                   │  │  └─────────┘ └──────────┘  │     │                   │
│                   │  └────────────────────────────┘     │                   │
└───────────────────┼─────────────────────────────────────┼───────────────────┘
                    │             │ Donanım Erişimi        │
                    │             ▼                        │
                    │  ┌────────────────────────────────┐ │
                    │  │   CPU, RAM, GPIO, I2C, SPI...  │ │
                    │  └────────────────────────────────┘ │
                    └─────────────────────────────────────┘
```

### Kernel Mode (Çekirdek Modu)

- İşletim sistemi çekirdeği burada çalışır
- Belleğe **tam erişim**
- **Tüm** makine komutlarını kullanabilir
- Donanıma **doğrudan erişim**
- **Aygıt sürücüler** burada çalışır

### Mod Geçişi

Kullanıcı modu programları sistem çağrıları (syscall) ile kernel moduna geçer:

```
User Mode Program ──► syscall çağrısı ──► Kernel Mode'a geçiş ──►
                                                │
        ◄─────────────────────────────────────────
                                                │
        Sistem fonksiyonu çalışır ──► Sonuç döner ──► User Mode'a dönüş
```

> [!IMPORTANT]
> `sudo` ile çalıştırmak programı kernel moduna **geçirmez**! sudo yalnızca root kullanıcı yetkisi verir.

---

## 📦 Çekirdek Modülü vs Aygıt Sürücü

| Kavram | Açıklama |
|--------|----------|
| **Çekirdek Modülü** | Çekirdeğe yüklenebilen **herhangi** bir kod parçası |
| **Aygıt Sürücü** | Kullanıcı modu ile **iletişim kuran** veya **kesmelere** yanıt veren çekirdek modülü |

> **Her aygıt sürücü bir çekirdek modülüdür, fakat her çekirdek modülü aygıt sürücü DEĞİLDİR.**

### Mimari Karşılaştırma

| Özellik | Monolitik (Linux) | Mikrokernel |
|---------|-------------------|-------------|
| Sürücü konumu | Kernel mode | User mode |
| Performans | Yüksek | Düşük |
| Güvenlik | Daha riskli | Daha güvenli |
| Örnek | Linux, Windows | Minix, QNX |

---

## 🛠️ Derleme Gereksinimleri

### Gerekli Paketler

```bash
# Kernel headers (mecburi)
sudo apt install linux-headers-$(uname -r)

# Derleme araçları
sudo apt install build-essential

# Tam kernel kaynağı (opsiyonel)
sudo apt install linux-source
```

### Dizin Yapısı

```bash
# Kernel kaynak/headers konumu
/lib/modules/$(uname -r)/build    # Sembolik link → headers dizini
/usr/src/linux-headers-$(uname -r)  # Header dosyaları

# Kurulu modüller
/lib/modules/$(uname -r)/kernel/   # Sistem modülleri
```

---

## 📝 Temel Başlık Dosyaları

Her çekirdek modülünde minimum iki başlık dosyası include edilmelidir:

```c
#include <linux/module.h>   /* MODULE_LICENSE, module_init/exit */
#include <linux/kernel.h>   /* printk, KERN_INFO, pr_xxx */
```

> [!WARNING]
> Bu başlık dosyaları `/usr/include` içinde DEĞİL, kernel headers içindedir!
> Normal C kütüphaneleri (libc, pthread) çekirdek modüllerinde **kullanılamaz**!

---

## 📄 En Basit Çekirdek Modülü

```c
/* helloworld.c */

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
    printk(KERN_INFO "Hello World...\n");
    return 0;  /* 0 = başarı, negatif = hata */
}

void cleanup_module(void)
{
    printk(KERN_INFO "Goodbye World...\n");
}
```

### Fonksiyon Açıklamaları

| Fonksiyon | Çağrılma Zamanı | Geri Dönüş |
|-----------|-----------------|------------|
| `init_module()` | Modül **yüklendiğinde** | `0` (başarı) veya negatif errno |
| `cleanup_module()` | Modül **kaldırıldığında** | void |

---

## 🔧 Makefile (KBuild Sistemi)

Çekirdek modülleri normal `gcc` ile **derlenemez**! Kernel'ın KBuild sistemi kullanılmalıdır:

```makefile
# Makefile

obj-m += helloworld.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
```

### Makefile Açıklaması

| Satır | Açıklama |
|-------|----------|
| `obj-m += helloworld.o` | Derlenecek modül |
| `-C $(KDIR)` | Kernel build dizinine git |
| `M=$(PWD)` | Kaynak dosyaların bulunduğu dizin |
| `modules` | Modülleri derle |

### Parametreli Makefile

```makefile
obj-m += $(file).o

# Kullanım: make file=helloworld
```

### Cross-Compile Makefile (BBB için)

```makefile
CROSS_COMPILE = arm-linux-gnueabihf-
ARCH = arm

KDIR := /path/to/bbb/linux-source
PWD := $(shell pwd)

obj-m += $(file).o

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
```

---

## 🚀 Modül Yükleme ve Kaldırma

### insmod ve rmmod

```bash
# Derleme
make

# Yükleme
sudo insmod helloworld.ko

# Log kontrolü
dmesg | tail -5
# veya
tail /var/log/syslog

# Yüklü modülleri listele
lsmod | grep hello
cat /proc/modules | grep hello

# Kaldırma
sudo rmmod helloworld

# Log kontrolü
dmesg | tail -5
```

### modprobe (Bağımlılıklarla Yükleme)

```bash
# Sistem modüllerini modprobe ile yükle
sudo modprobe g_ether

# Kaldır
sudo modprobe -r g_ether
```

> [!NOTE]
> `modprobe` modülleri `/lib/modules/$(uname -r)/` altında arar.
> Kendi modüllerimiz için `insmod` kullanılır.

### modinfo - Modül Bilgisi

```bash
modinfo helloworld.ko

# Çıktı:
# filename:       /path/helloworld.ko
# license:        GPL
# srcversion:     ABC123...
# depends:
# vermagic:       6.8.0-90-generic SMP preempt...
```

---

## 📝 printk Fonksiyonu (Detaylı)

`printk` = Kernel'ın printf fonksiyonu. Mesajlar log dosyalarına yazılır.

### Kullanım

```c
printk(KERN_INFO "Hello World! Value = %d\n", value);
```

### Log Seviyeleri

| Makro | Değer | Açıklama | Kullanım |
|-------|-------|----------|----------|
| `KERN_EMERG` | 0 | Sistem kullanılamaz | Felaket durumları |
| `KERN_ALERT` | 1 | Hemen aksiyon gerekli | Kritik hatalar |
| `KERN_CRIT` | 2 | Kritik durum | Donanım hataları |
| `KERN_ERR` | 3 | Hata | Genel hatalar ⭐ |
| `KERN_WARNING` | 4 | Uyarı | Uyarı mesajları ⭐ |
| `KERN_NOTICE` | 5 | Normal ama önemli | Bildirimler |
| `KERN_INFO` | 6 | Bilgilendirme | Genel bilgi ⭐ |
| `KERN_DEBUG` | 7 | Debug | Sadece geliştirme |

### KERN_XXX Makro Yapısı

```c
#define KERN_SOH     "\001"        /* ASCII Start Of Header */
#define KERN_INFO    KERN_SOH "6"  /* Bilgilendirme */

/* Yani: */
printk(KERN_INFO "Hello");
/* eşittir: */
printk("\0016Hello");
```

### pr_xxx Makroları (Alternatif)

Daha kısa ve okunabilir syntax:

```c
pr_emerg("Acil durum!\n");     /* = printk(KERN_EMERG ...) */
pr_alert("Uyarı!\n");          /* = printk(KERN_ALERT ...) */
pr_crit("Kritik!\n");          /* = printk(KERN_CRIT ...) */
pr_err("Hata oluştu!\n");      /* = printk(KERN_ERR ...) */
pr_warn("Dikkat!\n");          /* = printk(KERN_WARNING ...) */
pr_notice("Bildirim\n");       /* = printk(KERN_NOTICE ...) */
pr_info("Bilgi\n");            /* = printk(KERN_INFO ...) */
pr_debug("Debug\n");           /* = printk(KERN_DEBUG ...) */
```

### Log Görüntüleme

```bash
# dmesg (kernel ring buffer)
dmesg | tail -20
dmesg -w           # Canlı takip
dmesg -T           # Okunabilir zaman
dmesg -l err,warn  # Sadece hata ve uyarı

# Syslog
tail -f /var/log/syslog

# journalctl (systemd)
journalctl -k -f
```

---

## 🔄 module_init ve module_exit Makroları

Varsayılan `init_module` ve `cleanup_module` isimleri **değiştirilebilir**:

```c
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int __init helloworld_init(void)
{
    pr_info("Hello World!\n");
    return 0;
}

static void __exit helloworld_exit(void)
{
    pr_info("Goodbye World!\n");
}

module_init(helloworld_init);   /* init fonksiyonunu belirt */
module_exit(helloworld_exit);   /* exit fonksiyonunu belirt */
```

### __init ve __exit Makroları

| Makro | Etki |
|-------|------|
| `__init` | Fonksiyon yükleme sonrası bellekten atılır (yer tasarrufu) |
| `__exit` | Kernel'a gömülü modüllerde fonksiyon dahil edilmez |

### static Kullanımı

Fonksiyonları **static** yapmak önerilir:

```c
static int __init my_init(void)  { ... }
static void __exit my_exit(void) { ... }
```

**Nedenleri:**
- İsim çakışmalarını önler
- Internal linkage sağlar
- Compiler optimizasyonu

---

## ⚙️ Modül Parametreleri (module_param)

Modüllere yükleme sırasında parametre geçirilebilir:

### Parametre Tanımlama

```c
#include <linux/module.h>
#include <linux/moduleparam.h>

static int count = 1;
static char *msg = "Varsayilan mesaj";

module_param(count, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param(msg, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

MODULE_PARM_DESC(count, "Tekrar sayisi");
MODULE_PARM_DESC(msg, "Yazdirilacak mesaj");
```

### module_param Syntax

```c
module_param(name, type, perm);
```

| Parametre | Açıklama |
|-----------|----------|
| `name` | Değişken adı (global olmalı) |
| `type` | Veri tipi |
| `perm` | sysfs dosya izinleri |

### Desteklenen Tipler

| Tip | C Karşılığı |
|-----|-------------|
| `int` | int |
| `uint` | unsigned int |
| `long` | long |
| `ulong` | unsigned long |
| `short` | short |
| `ushort` | unsigned short |
| `bool` | bool |
| `invbool` | Ters bool |
| `charp` | char * (string) |

### Dizi Parametreleri

```c
static int values[5] = {1, 2, 3, 4, 5};
static int values_count = 5;

module_param_array(values, int, &values_count, S_IRUGO);
MODULE_PARM_DESC(values, "Tamsayi dizisi");
```

### Parametre ile Yükleme

```bash
# Basit parametreler
sudo insmod helloworld.ko count=10 msg="Merhaba"

# String parametre (tırnak dikkat!)
sudo insmod helloworld.ko msg="\"Bu bir test\""

# Dizi parametresi
sudo insmod helloworld.ko values=10,20,30,40,50
```

---

## 📂 sysfs ile Modül Bilgileri

Her yüklenen modül için `/sys/module/<modül_adı>/` dizini oluşur:

```bash
ls -l /sys/module/helloworld/
# toplam 0
# -r--r--r-- 1 root root 4096 ... coresize
# drwxr-xr-x 2 root root    0 ... holders
# -r--r--r-- 1 root root 4096 ... initsize
# -r--r--r-- 1 root root 4096 ... initstate
# drwxr-xr-x 2 root root    0 ... notes
# drwxr-xr-x 2 root root    0 ... parameters    ← Parametreler burada!
# -r--r--r-- 1 root root 4096 ... refcnt
# drwxr-xr-x 2 root root    0 ... sections
# -r--r--r-- 1 root root 4096 ... srcversion
```

### Parametre Değerlerini Okuma

```bash
# Parametre değerini oku
cat /sys/module/helloworld/parameters/count
cat /sys/module/helloworld/parameters/msg

# Parametre değerini değiştir (izin varsa)
echo 50 | sudo tee /sys/module/helloworld/parameters/count
```

---

## 🔄 Otomatik Modül Yükleme

### 1. /etc/modules Dosyası

```bash
# /etc/modules dosyasına modül ekle
echo "helloworld" | sudo tee -a /etc/modules
```

### 2. systemd Servis Dosyası

```ini
# /etc/systemd/system/my-module.service

[Unit]
Description=Modül Yükleme Servisi
After=network.target

[Service]
Type=oneshot
ExecStart=/sbin/modprobe helloworld
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable my-module.service
```

### 3. /etc/rc.local (Eski Yöntem)

```bash
# /etc/rc.local
sudo insmod /path/to/helloworld.ko
```

### 4. modprobe için Kurulum

```bash
# Modülü sistem dizinine kopyala
sudo cp helloworld.ko /lib/modules/$(uname -r)/kernel/drivers/misc/

# Bağımlılık veritabanını güncelle
sudo depmod -a

# Artık modprobe ile yüklenebilir
sudo modprobe helloworld
```

---

## 📊 Modül Bilgi Makroları

```c
MODULE_LICENSE("GPL");                           /* Zorunlu! */
MODULE_AUTHOR("Kaan ASLAN");                     /* Yazar */
MODULE_DESCRIPTION("Ornek kernel modulu");       /* Açıklama */
MODULE_VERSION("1.0");                           /* Versiyon */
MODULE_ALIAS("alternatif_isim");                 /* Alternatif isim */
```

---

## 💻 Tam Örnek: Parametreli Modül

```c
/* params_demo.c */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("Parametreli kernel modulu ornegi");
MODULE_VERSION("1.0");

/* Parametreler */
static int repeat_count = 1;
static char *message = "Varsayilan mesaj";
static int numbers[5] = {1, 2, 3, 4, 5};
static int numbers_count = 5;

/* Parametre tanımları */
module_param(repeat_count, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(repeat_count, "Mesajin tekrar sayisi");

module_param(message, charp, S_IRUGO);
MODULE_PARM_DESC(message, "Yazdirilacak mesaj");

module_param_array(numbers, int, &numbers_count, S_IRUGO);
MODULE_PARM_DESC(numbers, "Ornek sayi dizisi");

static int __init params_init(void)
{
    int i;

    pr_info("=== Parametreli Modul Yuklendi ===\n");
    pr_info("repeat_count = %d\n", repeat_count);
    pr_info("message = \"%s\"\n", message);

    pr_info("numbers[%d] = { ", numbers_count);
    for (i = 0; i < numbers_count; i++) {
        pr_cont("%d ", numbers[i]);
    }
    pr_cont("}\n");

    for (i = 0; i < repeat_count; i++) {
        pr_info("[%d] %s\n", i + 1, message);
    }

    return 0;
}

static void __exit params_exit(void)
{
    pr_info("=== Parametreli Modul Kaldirildi ===\n");
}

module_init(params_init);
module_exit(params_exit);
```

### Test

```bash
# Derle
make file=params_demo

# Varsayılan parametrelerle yükle
sudo insmod params_demo.ko
dmesg | tail -10

# Parametrelerle yükle
sudo rmmod params_demo
sudo insmod params_demo.ko repeat_count=3 message="Merhaba Linux"
dmesg | tail -10

# sysfs'ten parametre oku
cat /sys/module/params_demo/parameters/repeat_count

# Kaldır
sudo rmmod params_demo
```

---

## ⚠️ Dikkat Edilmesi Gerekenler

> [!CAUTION]
> Kernel modülünde hata = **Kernel Panic** = Sistem çökmesi!
> Test için sanal makine veya ayrı SBC kullanın.

> [!WARNING]
> Çekirdek modüllerinde libc, pthread gibi user-mode kütüphaneleri **KULLANILAMAZ**!

> [!IMPORTANT]
> Her kernel versiyonu için modül **yeniden derlenmeli**. Farklı versiyon = uyumsuzluk.

---

## 📚 Export Edilmiş Fonksiyonlar

Modüller yalnızca kernel'ın **export ettiği** fonksiyonları kullanabilir:

```bash
# Export edilmiş sembolleri listele
cat /proc/kallsyms | head -50

# T = text (fonksiyon), t = local, B = bss, D = data
```

**Dokümantasyon:** [kernel.org/core-api](https://docs.kernel.org/core-api/kernel-api.html)

---

## 🎯 Pratik Ödevler

1. `helloworld.ko` modülünü derle ve yükle
2. `dmesg` ile mesajları kontrol et
3. Parametre ekle ve farklı değerlerle test et
4. `/sys/module/helloworld/` içeriğini incele
5. Cross-compile yaparak BBB'de çalıştır

---

*Sonraki ders: Character Device Drivers, file_operations, IOCTL*
