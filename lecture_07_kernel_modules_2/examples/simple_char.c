/* simple_char.c - Basit Character Device Driver
 * Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)
 * 
 * Bu modül /dev/simplechar cihazı oluşturur.
 * Kullanıcı bu cihaza yazabilir ve okuyabilir.
 * 
 * Derleme: make
 * Yükleme: sudo insmod simple_char.ko
 * Test: echo "test" | sudo tee /dev/simplechar && sudo cat /dev/simplechar
 * Kaldırma: sudo rmmod simple_char
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "simplechar"
#define CLASS_NAME  "simpleclass"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("Basit character device driver ornegi");
MODULE_VERSION("1.0");

/* Global değişkenler */
static int major_number;
static struct class *device_class;
static struct device *device_dev;
static char message[BUFFER_SIZE];
static int message_len;
static int open_count = 0;
static DEFINE_MUTEX(device_mutex);

/* open: Cihaz açıldığında çağrılır */
static int dev_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&device_mutex)) {
        pr_warn("%s: Device is busy\n", DEVICE_NAME);
        return -EBUSY;
    }
    
    open_count++;
    pr_info("%s: Device opened %d time(s)\n", DEVICE_NAME, open_count);
    return 0;
}

/* release: Cihaz kapatıldığında çağrılır */
static int dev_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&device_mutex);
    pr_info("%s: Device closed\n", DEVICE_NAME);
    return 0;
}

/* read: Kullanıcı cihazdan okuma yaptığında */
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

/* write: Kullanıcı cihaza yazdığında */
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

    pr_info("%s: Received %d bytes from user\n", DEVICE_NAME, message_len);

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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    device_class = class_create(CLASS_NAME);
#else
    device_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
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

    /* Başlangıç mesajı */
    strcpy(message, "Hello from kernel driver!\n");
    message_len = strlen(message);

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
