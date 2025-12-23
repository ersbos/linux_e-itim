# Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)

> **Konu:** Character Device Drivers, file_operations, IOCTL, Kullanıcı-Kernel İletişimi
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Aygıt sürücü türleri (Character, Block, Network)
- [x] Major ve Minor numaralar
- [x] /dev dizini ve cihaz dosyaları
- [x] Character device driver yapısı
- [x] file_operations yapısı (detaylı)
- [x] open, read, write, release implementasyonu
- [x] copy_to_user / copy_from_user
- [x] IOCTL mekanizması
- [x] Kullanıcı modu test programı
- [x] Automatic device node creation

---

## 📦 Aygıt Sürücü Türleri

Linux'ta üç temel aygıt sürücü türü vardır:

| Tür | Erişim Yöntemi | Örnek Cihazlar | /dev Örneği |
|-----|----------------|----------------|-------------|
| **Character** | Byte-by-byte (stream) | Seri port, klavye, fare, GPIO | `/dev/ttyS0`, `/dev/random` |
| **Block** | Blok halinde (sector) | Disk, SD kart, USB bellek | `/dev/sda`, `/dev/mmcblk0` |
| **Network** | Paket tabanlı | Ethernet, WiFi | `eth0`, `wlan0` (dosya yok) |

### Character Device Driver

Bu derste **Character Device Driver** üzerine odaklanıyoruz. Bunlar:

- Byte akışı (stream) olarak erişilir
- `read()`, `write()`, `open()`, `close()` system call'ları ile kullanılır
- `/dev` altında dosya olarak görünür
- Seri port, sensörler, GPIO için kullanılır

```
┌─────────────────────────────────────────────────────────────┐
│                     USER SPACE                               │
│                                                              │
│   ┌──────────────────────────────────────────────────────┐  │
│   │  User Program                                         │  │
│   │  fd = open("/dev/mydevice", O_RDWR);                 │  │
│   │  read(fd, buffer, size);                              │  │
│   │  write(fd, buffer, size);                             │  │
│   │  ioctl(fd, CMD, &arg);                                │  │
│   │  close(fd);                                           │  │
│   └────────────────────────┬─────────────────────────────┘  │
│                            │ System Calls                    │
├────────────────────────────┼────────────────────────────────┤
│                            ▼                                 │
│                     KERNEL SPACE                             │
│                                                              │
│   ┌──────────────────────────────────────────────────────┐  │
│   │  VFS (Virtual File System)                            │  │
│   │  → Major/Minor number ile driver bulma                │  │
│   └────────────────────────┬─────────────────────────────┘  │
│                            ▼                                 │
│   ┌──────────────────────────────────────────────────────┐  │
│   │  Character Device Driver                              │  │
│   │  → file_operations yapısı                             │  │
│   │  → .open, .read, .write, .release, .unlocked_ioctl   │  │
│   └────────────────────────┬─────────────────────────────┘  │
│                            ▼                                 │
│   ┌──────────────────────────────────────────────────────┐  │
│   │  Hardware (GPIO, UART, I2C, ...)                      │  │
│   └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔢 Major ve Minor Numaralar

Her cihaz dosyası **major** ve **minor** numaralara sahiptir:

```bash
ls -la /dev/tty*
# crw-rw-rw- 1 root tty 5, 0 Dec 23 10:00 /dev/tty
#                        ^  ^
#                        │  └── Minor number
#                        └── Major number
```

| Numara | Açıklama |
|--------|----------|
| **Major** | Hangi **aygıt sürücünün** kullanılacağını belirler |
| **Minor** | Aynı sürücüdeki **farklı cihazları** ayırır |

### Major Number Alma

```c
#include <linux/fs.h>

/* Statik major number (eski yöntem) */
#define MY_MAJOR 240
register_chrdev(MY_MAJOR, "mydevice", &fops);

/* Dinamik major number (önerilen) */
int major = register_chrdev(0, "mydevice", &fops);

/* Modern yöntem (alloc_chrdev_region) */
dev_t dev;
alloc_chrdev_region(&dev, 0, 1, "mydevice");
int major = MAJOR(dev);
int minor = MINOR(dev);
```

### dev_t Tipi

```c
#include <linux/kdev_t.h>

dev_t dev;

/* Major ve minor'dan dev_t oluştur */
dev = MKDEV(major, minor);

/* dev_t'den major ve minor çıkar */
int major = MAJOR(dev);
int minor = MINOR(dev);
```

---

## 📁 /dev Dizini ve Cihaz Dosyaları

### Manuel Oluşturma (mknod)

```bash
# Character device oluştur
sudo mknod /dev/mydevice c 240 0
#                        ^  ^   ^
#                        │  │   └── Minor number
#                        │  └── Major number
#                        └── c = character device

# İzinleri ayarla
sudo chmod 666 /dev/mydevice
```

### Otomatik Oluşturma (udev)

Modern yöntem: Driver içinde `device_create()` çağrısı yapılır, udev otomatik olarak `/dev` girişi oluşturur.

---

## 📝 file_operations Yapısı

Character device driver'ın kalbi `file_operations` yapısıdır:

```c
#include <linux/fs.h>

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = device_open,
    .release        = device_release,
    .read           = device_read,
    .write          = device_write,
    .unlocked_ioctl = device_ioctl,
    .llseek         = device_llseek,
    .mmap           = device_mmap,
    /* ... ve daha fazlası */
};
```

### Temel file_operations Fonksiyonları

| Fonksiyon | Syscall | Açıklama |
|-----------|---------|----------|
| `.open` | `open()` | Cihaz açıldığında |
| `.release` | `close()` | Cihaz kapatıldığında |
| `.read` | `read()` | Cihazdan okuma |
| `.write` | `write()` | Cihaza yazma |
| `.unlocked_ioctl` | `ioctl()` | Özel kontrol komutları |
| `.llseek` | `lseek()` | Dosya pozisyonu değiştirme |
| `.mmap` | `mmap()` | Bellek mapping |

### Fonksiyon Prototipleri

```c
/* open: Cihaz açıldığında çağrılır */
static int device_open(struct inode *inode, struct file *file);

/* release: Cihaz kapatıldığında çağrılır */
static int device_release(struct inode *inode, struct file *file);

/* read: Kullanıcı cihazdan okuma yaptığında */
static ssize_t device_read(struct file *file, char __user *buf,
                           size_t len, loff_t *offset);

/* write: Kullanıcı cihaza yazdığında */
static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *offset);

/* ioctl: Özel kontrol komutları */
static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg);
```

---

## 🔄 copy_to_user / copy_from_user

Kernel ve user space arasında veri transferi için **özel fonksiyonlar** kullanılmalıdır:

> [!CAUTION]
> User space belleğine doğrudan erişim **YASAKTIR**!
> Kernel panic'e neden olur.

```c
#include <linux/uaccess.h>

/* Kernel → User (read işlemi) */
unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);

/* User → Kernel (write işlemi) */
unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
```

### Kullanım Örneği

```c
static ssize_t device_read(struct file *file, char __user *buf,
                           size_t len, loff_t *offset)
{
    char message[] = "Hello from kernel!\n";
    size_t message_len = strlen(message);
    unsigned long not_copied;

    if (*offset >= message_len)
        return 0;  /* EOF */

    if (len > message_len - *offset)
        len = message_len - *offset;

    not_copied = copy_to_user(buf, message + *offset, len);
    if (not_copied)
        return -EFAULT;

    *offset += len;
    return len;
}

static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *offset)
{
    char kernel_buf[256];
    unsigned long not_copied;

    if (len > sizeof(kernel_buf) - 1)
        len = sizeof(kernel_buf) - 1;

    not_copied = copy_from_user(kernel_buf, buf, len);
    if (not_copied)
        return -EFAULT;

    kernel_buf[len] = '\0';
    pr_info("Received from user: %s\n", kernel_buf);

    return len;
}
```

---

## 🔧 IOCTL Mekanizması (Detaylı)

**IOCTL (I/O Control)** = read/write dışında cihazı kontrol etme mekanizması.

### IOCTL Kullanım Alanları

- Cihaz ayarlarını okuma/yazma
- Cihaz durumunu sorgulama
- Özel komutlar çalıştırma
- Veri yapılarını transfer etme

### IOCTL Komut Numarası Oluşturma

```c
#include <linux/ioctl.h>

#define MYDEV_MAGIC 'k'  /* Benzersiz "magic number" */

/* Parametresiz komut */
#define MYDEV_RESET        _IO(MYDEV_MAGIC, 0)

/* Okuma komutu (Kernel → User) */
#define MYDEV_GET_VALUE    _IOR(MYDEV_MAGIC, 1, int)

/* Yazma komutu (User → Kernel) */
#define MYDEV_SET_VALUE    _IOW(MYDEV_MAGIC, 2, int)

/* Okuma-Yazma komutu */
#define MYDEV_RW_DATA      _IOWR(MYDEV_MAGIC, 3, struct mydata)
```

### IOCTL Makro Açıklamaları

| Makro | Yön | Açıklama |
|-------|-----|----------|
| `_IO(type, nr)` | - | Veri transferi yok |
| `_IOR(type, nr, datatype)` | R | Kernel → User okuma |
| `_IOW(type, nr, datatype)` | W | User → Kernel yazma |
| `_IOWR(type, nr, datatype)` | R/W | Her iki yön |

### Kernel Tarafı IOCTL Handler

```c
static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg)
{
    int value;
    int __user *user_ptr = (int __user *)arg;

    switch (cmd) {
    case MYDEV_RESET:
        pr_info("IOCTL: Reset command\n");
        internal_value = 0;
        break;

    case MYDEV_GET_VALUE:
        pr_info("IOCTL: Get value\n");
        if (copy_to_user(user_ptr, &internal_value, sizeof(int)))
            return -EFAULT;
        break;

    case MYDEV_SET_VALUE:
        pr_info("IOCTL: Set value\n");
        if (copy_from_user(&value, user_ptr, sizeof(int)))
            return -EFAULT;
        internal_value = value;
        break;

    default:
        return -ENOTTY;  /* Bilinmeyen komut */
    }

    return 0;
}
```

### User Space IOCTL Kullanımı

```c
/* userspace_test.c */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Aynı IOCTL tanımları */
#define MYDEV_MAGIC 'k'
#define MYDEV_RESET        _IO(MYDEV_MAGIC, 0)
#define MYDEV_GET_VALUE    _IOR(MYDEV_MAGIC, 1, int)
#define MYDEV_SET_VALUE    _IOW(MYDEV_MAGIC, 2, int)

int main(void)
{
    int fd;
    int value;

    fd = open("/dev/mydevice", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    /* Değer ayarla */
    value = 42;
    if (ioctl(fd, MYDEV_SET_VALUE, &value) < 0) {
        perror("ioctl SET");
    }

    /* Değer oku */
    if (ioctl(fd, MYDEV_GET_VALUE, &value) < 0) {
        perror("ioctl GET");
    }
    printf("Value: %d\n", value);

    /* Reset */
    if (ioctl(fd, MYDEV_RESET) < 0) {
        perror("ioctl RESET");
    }

    close(fd);
    return 0;
}
```

---

## 💻 Tam Character Device Driver Örneği

```c
/* simple_char.c - Basit Character Device Driver */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simplechar"
#define CLASS_NAME  "simpleclass"
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("Basit character device driver ornegi");
MODULE_VERSION("1.0");

/* Global değişkenler */
static int major_number;
static struct class *device_class;
static struct device *device_dev;
static char message[BUFFER_SIZE] = {0};
static int message_len;
static int open_count = 0;

/* file_operations fonksiyonları */
static int dev_open(struct inode *inode, struct file *file)
{
    open_count++;
    pr_info("%s: Device opened %d time(s)\n", DEVICE_NAME, open_count);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("%s: Device closed\n", DEVICE_NAME);
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    int bytes_to_copy;
    int bytes_not_copied;

    /* EOF kontrolü */
    if (*offset >= message_len)
        return 0;

    /* Okunacak byte sayısı */
    bytes_to_copy = min(len, (size_t)(message_len - *offset));

    /* Kernel → User kopyalama */
    bytes_not_copied = copy_to_user(buf, message + *offset, bytes_to_copy);
    if (bytes_not_copied)
        return -EFAULT;

    *offset += bytes_to_copy;
    pr_info("%s: Sent %d bytes to user\n", DEVICE_NAME, bytes_to_copy);

    return bytes_to_copy;
}

static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *offset)
{
    int bytes_to_copy;
    int bytes_not_copied;

    /* Buffer boyutunu aşma */
    bytes_to_copy = min(len, (size_t)(BUFFER_SIZE - 1));

    /* User → Kernel kopyalama */
    bytes_not_copied = copy_from_user(message, buf, bytes_to_copy);
    if (bytes_not_copied)
        return -EFAULT;

    message_len = bytes_to_copy;
    message[message_len] = '\0';

    pr_info("%s: Received %d bytes: %s\n", DEVICE_NAME, message_len, message);

    return bytes_to_copy;
}

/* file_operations yapısı */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

/* Module init */
static int __init simple_init(void)
{
    pr_info("%s: Initializing driver\n", DEVICE_NAME);

    /* 1. Major number al (dinamik) */
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("%s: Failed to register major number\n", DEVICE_NAME);
        return major_number;
    }
    pr_info("%s: Registered with major number %d\n", DEVICE_NAME, major_number);

    /* 2. Device class oluştur */
    device_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(device_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("%s: Failed to create class\n", DEVICE_NAME);
        return PTR_ERR(device_class);
    }
    pr_info("%s: Device class created\n", DEVICE_NAME);

    /* 3. Device oluştur (/dev/simplechar) */
    device_dev = device_create(device_class, NULL,
                               MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device_dev)) {
        class_destroy(device_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("%s: Failed to create device\n", DEVICE_NAME);
        return PTR_ERR(device_dev);
    }
    pr_info("%s: Device created at /dev/%s\n", DEVICE_NAME, DEVICE_NAME);

    return 0;
}

/* Module exit */
static void __exit simple_exit(void)
{
    device_destroy(device_class, MKDEV(major_number, 0));
    class_unregister(device_class);
    class_destroy(device_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("%s: Device removed\n", DEVICE_NAME);
}

module_init(simple_init);
module_exit(simple_exit);
```

### Makefile

```makefile
obj-m += simple_char.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod simple_char.ko

unload:
	sudo rmmod simple_char

test:
	@echo "=== Test başlıyor ==="
	@echo "Merhaba Linux" | sudo tee /dev/simplechar
	@sudo cat /dev/simplechar
	@echo "=== Test bitti ==="

.PHONY: all clean load unload test
```

---

## 🧪 Test Etme

```bash
# 1. Derle
make

# 2. Yükle
sudo insmod simple_char.ko

# 3. Log kontrol
dmesg | tail -10

# 4. /dev girişi oluştu mu?
ls -la /dev/simplechar

# 5. Yazma testi
echo "Merhaba Kernel" | sudo tee /dev/simplechar

# 6. Okuma testi
sudo cat /dev/simplechar

# 7. Kaldır
sudo rmmod simple_char
```

---

## 📊 Hata Kodları

Kernel fonksiyonları negatif errno değerleri döndürür:

| Kod | Makro | Açıklama |
|-----|-------|----------|
| -1 | `-EPERM` | İzin yok |
| -2 | `-ENOENT` | Dosya yok |
| -5 | `-EIO` | I/O hatası |
| -12 | `-ENOMEM` | Bellek yok |
| -13 | `-EACCES` | Erişim engellendi |
| -14 | `-EFAULT` | Geçersiz adres |
| -16 | `-EBUSY` | Cihaz meşgul |
| -19 | `-ENODEV` | Cihaz yok |
| -22 | `-EINVAL` | Geçersiz argüman |
| -25 | `-ENOTTY` | Uygunsuz ioctl |

```c
#include <linux/errno.h>

/* Hata döndürme */
return -EINVAL;  /* Geçersiz argüman */
return -ENOMEM;  /* Bellek yok */
return -EFAULT;  /* copy_to/from_user hatası */
```

---

## 🔒 Concurrency ve Mutex

Birden fazla process aynı anda driver'a erişebilir:

```c
#include <linux/mutex.h>

static DEFINE_MUTEX(my_mutex);

static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    ssize_t ret;

    /* Kilidi al */
    if (mutex_lock_interruptible(&my_mutex))
        return -ERESTARTSYS;

    /* Kritik bölge */
    ret = /* ... okuma işlemi ... */;

    /* Kilidi bırak */
    mutex_unlock(&my_mutex);

    return ret;
}
```

---

## 📁 Modern cdev API

Daha modern ve esnek yöntem:

```c
#include <linux/cdev.h>

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

static int __init my_init(void)
{
    /* Dinamik major/minor al */
    alloc_chrdev_region(&dev_num, 0, 1, "mydevice");

    /* cdev yapısını hazırla */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    /* Kernel'a ekle */
    cdev_add(&my_cdev, dev_num, 1);

    /* Class ve device oluştur */
    my_class = class_create(THIS_MODULE, "myclass");
    device_create(my_class, NULL, dev_num, NULL, "mydevice");

    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
}
```

---

## 🎯 Pratik Ödevler

1. `simple_char.ko` modülünü derle ve yükle
2. `/dev/simplechar` cihazına yaz ve oku
3. IOCTL komutu ekle (GET/SET_VALUE)
4. Kullanıcı modu test programı yaz
5. Cross-compile yaparak BBB'de çalıştır

---

## 📚 Kaynaklar

- [Linux Device Drivers (LDD3)](https://lwn.net/Kernel/LDD3/) - Ücretsiz online
- [kernel.org Driver API](https://docs.kernel.org/driver-api/)
- [Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)

---

*Sonraki ders: GPIO Programlama (Ders 8)*
