# Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)

> **Konu:** Character Device Drivers, IOCTL, Kullanıcı-Kernel İletişimi

---

## 📋 Bu Derste İşlenenler

- [x] Character device driver yapısı
- [x] Major/minor number
- [x] file_operations yapısı
- [x] open, read, write, release
- [x] IOCTL mekanizması
- [x] /dev girişi oluşturma

---

## 📦 Aygıt Sürücü Türleri

| Tür | Açıklama | Örnek |
|-----|----------|-------|
| **Character** | Byte-by-byte erişim | `/dev/ttyS0`, `/dev/null` |
| **Block** | Blok erişim | `/dev/sda`, `/dev/mmcblk0` |
| **Network** | Ağ arayüzü | `eth0`, `wlan0` |

Bu derste **character device driver** odaklanıyoruz.

---

## 🔢 Major ve Minor Numaralar

```bash
# /dev girişlerini listele
ls -la /dev/tty*

# crw-rw-rw- 1 root tty 5, 0 Dec 23 10:00 /dev/tty
#                        ^  ^
#                        |  +-- Minor number
#                        +-- Major number
```

| Numara | Açıklama |
|--------|----------|
| **Major** | Aygıt sürücüyü tanımlar |
| **Minor** | Aynı sürücüdeki farklı cihazları ayırır |

### Dinamik Numara Alma

```c
#include <linux/fs.h>

// Dinamik major number al
int major = register_chrdev(0, "mydevice", &fops);

// veya modern yöntem
dev_t dev;
alloc_chrdev_region(&dev, 0, 1, "mydevice");
```

---

## 📝 file_operations Yapısı

```c
#include <linux/fs.h>

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = device_open,
    .read    = device_read,
    .write   = device_write,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};
```

### Fonksiyon İmzaları

```c
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *buf, 
                           size_t len, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *offset);
static long device_ioctl(struct file *file, unsigned int cmd, 
                         unsigned long arg);
```

---

## 💻 Basit Character Device Driver

```c
/* simple_char.c */

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
MODULE_DESCRIPTION("Basit character device driver");

static int major_number;
static struct class *device_class;
static struct device *device_dev;
static char message[BUFFER_SIZE] = {0};
static int message_len;
static int open_count = 0;

/* Fonksiyon prototipleri */
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};

static int __init simple_init(void)
{
    /* Major number al */
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register major number\n");
        return major_number;
    }
    pr_info("Registered with major number %d\n", major_number);

    /* Device class oluştur */
    device_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(device_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(device_class);
    }

    /* Device oluştur */
    device_dev = device_create(device_class, NULL, 
                               MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device_dev)) {
        class_destroy(device_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(device_dev);
    }

    pr_info("Device created: /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit simple_exit(void)
{
    device_destroy(device_class, MKDEV(major_number, 0));
    class_unregister(device_class);
    class_destroy(device_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("Device removed\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    open_count++;
    pr_info("Device opened %d time(s)\n", open_count);
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer,
                        size_t len, loff_t *offset)
{
    int bytes_to_copy;
    int bytes_not_copied;

    if (*offset >= message_len)
        return 0;

    bytes_to_copy = min(len, (size_t)(message_len - *offset));
    bytes_not_copied = copy_to_user(buffer, message + *offset, bytes_to_copy);

    *offset += bytes_to_copy - bytes_not_copied;
    
    pr_info("Sent %d bytes to user\n", bytes_to_copy - bytes_not_copied);
    return bytes_to_copy - bytes_not_copied;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer,
                         size_t len, loff_t *offset)
{
    int bytes_to_copy = min(len, (size_t)(BUFFER_SIZE - 1));
    int bytes_not_copied;

    bytes_not_copied = copy_from_user(message, buffer, bytes_to_copy);
    message_len = bytes_to_copy - bytes_not_copied;
    message[message_len] = '\0';

    pr_info("Received %d bytes from user\n", message_len);
    return message_len;
}

module_init(simple_init);
module_exit(simple_exit);
```

---

## 🔧 IOCTL Mekanizması

### IOCTL Nedir?

**IOCTL (I/O Control)** = read/write dışında cihazı kontrol etme mekanizması.

### IOCTL Komut Tanımlama

```c
#include <linux/ioctl.h>

#define MYDEV_MAGIC 'k'

/* Okuma komutu */
#define MYDEV_GET_VALUE _IOR(MYDEV_MAGIC, 1, int)

/* Yazma komutu */
#define MYDEV_SET_VALUE _IOW(MYDEV_MAGIC, 2, int)

/* Okuma-yazma komutu */
#define MYDEV_GETSET    _IOWR(MYDEV_MAGIC, 3, int)

/* Parametresiz komut */
#define MYDEV_RESET     _IO(MYDEV_MAGIC, 4)
```

### IOCTL Handler

```c
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int value;
    
    switch (cmd) {
    case MYDEV_GET_VALUE:
        value = internal_value;
        if (copy_to_user((int __user *)arg, &value, sizeof(value)))
            return -EFAULT;
        break;
        
    case MYDEV_SET_VALUE:
        if (copy_from_user(&value, (int __user *)arg, sizeof(value)))
            return -EFAULT;
        internal_value = value;
        break;
        
    case MYDEV_RESET:
        internal_value = 0;
        break;
        
    default:
        return -ENOTTY;
    }
    
    return 0;
}
```

### Kullanıcı Modunda IOCTL

```c
/* userspace_test.c */
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define MYDEV_MAGIC 'k'
#define MYDEV_SET_VALUE _IOW(MYDEV_MAGIC, 2, int)

int main(void)
{
    int fd = open("/dev/mydevice", O_RDWR);
    int value = 42;
    
    ioctl(fd, MYDEV_SET_VALUE, &value);
    
    close(fd);
    return 0;
}
```

---

## 🧪 Test Etme

```bash
# Modülü yükle
sudo insmod simple_char.ko

# Log kontrol
dmesg | tail -5

# /dev girişi oluştu mu?
ls -la /dev/simplechar

# Yazma testi
echo "Merhaba" | sudo tee /dev/simplechar

# Okuma testi
sudo cat /dev/simplechar

# Kaldır
sudo rmmod simple_char
```

---

## 🎯 Pratik

1. `simple_char.c` modülünü derle ve yükle
2. `/dev/simplechar` cihazına yaz ve oku
3. IOCTL komutu ekle
4. Kullanıcı mod test programı yaz

---

*Sonraki ders: GPIO Programlama*
