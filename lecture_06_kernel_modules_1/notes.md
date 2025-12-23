# Ders 6: Kernel Modüller ve Aygıt Sürücüler (Bölüm 1)

> **Konu:** Çekirdek Modülleri Temelleri

---

## 📋 Bu Derste İşlenenler

- [x] User Mode vs Kernel Mode
- [x] Kernel modül yapısı
- [x] init_module / cleanup_module
- [x] printk ile loglama
- [x] Modül derleme (Makefile)
- [x] Modül yükleme/kaldırma (insmod/rmmod)

---

## 🔐 User Mode vs Kernel Mode

Modern işletim sistemleri **koruma mekanizması** kullanır:

### User Mode (Kullanıcı Modu)
- Normal programlar burada çalışır
- Belleğin her yerine erişemez
- Her makine komutunu kullanamaz
- Sistem kaynaklarına doğrudan erişemez
- `sudo` ile çalıştırsan bile User Mode'da kalır!

### Kernel Mode (Çekirdek Modu)
- İşletim sistemi çekirdeği burada çalışır
- Belleğe tam erişim
- Tüm makine komutlarını kullanabilir
- Donanıma doğrudan erişim
- **Aygıt sürücüler burada çalışır**

```
┌─────────────────────────────────────────┐
│            User Mode                     │
│  ┌──────┐  ┌──────┐  ┌──────┐          │
│  │ App1 │  │ App2 │  │ App3 │          │
│  └──┬───┘  └──┬───┘  └──┬───┘          │
│     │        │        │                 │
│     └────────┼────────┘                 │
│              │ System Calls             │
├──────────────▼──────────────────────────┤
│            Kernel Mode                   │
│  ┌──────────────────────────────────┐   │
│  │        Linux Kernel               │   │
│  │  ┌────────┐  ┌────────────────┐  │   │
│  │  │Modules │  │ Device Drivers │  │   │
│  │  └────────┘  └────────────────┘  │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

---

## 📦 Kernel Modül vs Aygıt Sürücü

| Kavram | Açıklama |
|--------|----------|
| **Kernel Modül** | Çekirdeğe yüklenen herhangi bir kod |
| **Aygıt Sürücü** | Kullanıcı modu ile iletişim kuran veya kesme işleyen modül |

> Her aygıt sürücü bir kernel modüldür, ama her kernel modül aygıt sürücü değildir.

---

## 🛠️ Kernel Modül Yapısı

Minimal bir kernel modül iki fonksiyon içerir:

```c
// Modül yüklendiğinde çağrılır (constructor)
int init_module(void);

// Modül kaldırıldığında çağrılır (destructor)
void cleanup_module(void);
```

### Modern Syntax

```c
static int __init my_init(void);
static void __exit my_exit(void);

module_init(my_init);
module_exit(my_exit);
```

---

## 📝 printk Fonksiyonu

Kernel mode'da `printf` kullanılamaz! Bunun yerine `printk` kullanılır:

```c
printk(KERN_INFO "Merhaba kernel!\n");
```

### Log Seviyeleri

| Makro | Değer | Açıklama |
|-------|-------|----------|
| `KERN_EMERG` | 0 | Acil durum |
| `KERN_ALERT` | 1 | Hemen aksiyon gerekli |
| `KERN_CRIT` | 2 | Kritik durum |
| `KERN_ERR` | 3 | Hata |
| `KERN_WARNING` | 4 | Uyarı |
| `KERN_NOTICE` | 5 | Normal ama önemli |
| `KERN_INFO` | 6 | Bilgilendirme |
| `KERN_DEBUG` | 7 | Debug mesajları |

### pr_xxx Makroları (Alternatif)

```c
pr_info("Merhaba!\n");      // = printk(KERN_INFO ...)
pr_err("Hata oluştu!\n");   // = printk(KERN_ERR ...)
pr_warn("Dikkat!\n");       // = printk(KERN_WARNING ...)
```

### Log Görüntüleme

```bash
# Kernel mesajlarını görüntüle
dmesg | tail -20

# Canlı takip
dmesg -w

# veya syslog
tail -f /var/log/syslog
```

---

## 💻 Hello World Kernel Modül

```c
/* helloworld.c */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kurs");
MODULE_DESCRIPTION("Ilk kernel modul");
MODULE_VERSION("1.0");

static int __init hello_init(void)
{
    printk(KERN_INFO "Hello Kernel World!\n");
    return 0;  // 0 = başarılı
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye Kernel World!\n");
}

module_init(hello_init);
module_exit(hello_exit);
```

---

## 🔧 Makefile

```makefile
obj-m += helloworld.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

# Yardımcı hedefler
load:
	sudo insmod helloworld.ko

unload:
	sudo rmmod helloworld

log:
	dmesg | tail -10

.PHONY: all clean load unload log
```

---

## 🚀 Derleme ve Test

```bash
# Derle
make

# Yükle
sudo insmod helloworld.ko

# Log kontrol
dmesg | tail -5

# Yüklü modülleri listele
lsmod | grep hello

# Kaldır
sudo rmmod helloworld

# Log kontrol
dmesg | tail -5
```

---

## 📂 Modül Bilgileri

```bash
# Modül bilgisi
modinfo helloworld.ko

# Çıktı örneği:
# filename:       /path/helloworld.ko
# license:        GPL
# author:         Kurs
# description:    Ilk kernel modul
# version:        1.0
# srcversion:     ABC123...
# depends:
# vermagic:       6.8.0-90-generic SMP preempt...
```

---

## 🔍 sysfs ve /proc

Yüklenen her modül için sistem bilgileri:

```bash
# /sys/module altında
ls /sys/module/helloworld/

# /proc/modules dosyasında
cat /proc/modules | grep hello

# lsmod (okunaklı format)
lsmod | grep hello
```

---

## ⚙️ Modül Parametreleri

Modüllere parametre geçirilebilir:

```c
#include <linux/moduleparam.h>

static int count = 1;
static char *msg = "default";

module_param(count, int, S_IRUGO);
module_param(msg, charp, S_IRUGO);

MODULE_PARM_DESC(count, "Tekrar sayisi");
MODULE_PARM_DESC(msg, "Mesaj metni");
```

### Parametre ile Yükleme

```bash
sudo insmod mymodule.ko count=5 msg="Merhaba"
```

### Parametre Görüntüleme

```bash
cat /sys/module/mymodule/parameters/count
cat /sys/module/mymodule/parameters/msg
```

---

## ⚠️ Önemli Notlar

> [!CAUTION]
> Kernel modülünde hata = Sistem çökmesi (kernel panic)!
> Test için sanal makine veya ayrı bir SBC kullanın.

> [!WARNING]
> Kernel modüllerinde kullanıcı modundaki kütüphaneler (libc, pthread vb.) **kullanılamaz**!

> [!NOTE]
> Her kernel versiyonu için modül yeniden derlenmeli. Kernel güncellemesi sonrası eski .ko dosyaları çalışmaz.

---

## 📚 Export Edilmiş Fonksiyonlar

Kernel modülleri sadece **export edilmiş** fonksiyonları kullanabilir:

```bash
# Export edilmiş sembolleri listele
cat /proc/kallsyms | head -50

# Modülün kullandığı sembolleri gör
nm helloworld.ko
```

**Dokümantasyon:** [kernel.org/doc/html/latest/core-api](https://docs.kernel.org/core-api/)

---

## 🎯 Pratik

1. `helloworld.ko` modülünü derle ve test et
2. `module_param` ile parametre ekle
3. Parametre değerine göre farklı mesaj yazdır
4. `dmesg` çıktısını incele

---

*Sonraki ders: Character device drivers, file_operations*
