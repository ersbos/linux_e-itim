/* ioctl_driver.c - IOCTL örneği içeren Character Device Driver
 * Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)
 * 
 * Bu modül IOCTL komutlarını destekleyen bir karakter cihazı oluşturur.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "ioctldev"
#define CLASS_NAME  "ioctlclass"

/* IOCTL komut tanımları */
#define IOCTL_MAGIC 'k'
#define IOCTL_RESET        _IO(IOCTL_MAGIC, 0)
#define IOCTL_GET_VALUE    _IOR(IOCTL_MAGIC, 1, int)
#define IOCTL_SET_VALUE    _IOW(IOCTL_MAGIC, 2, int)
#define IOCTL_GET_INFO     _IOR(IOCTL_MAGIC, 3, struct device_info)

/* Bilgi yapısı */
struct device_info {
    int value;
    int access_count;
    char name[32];
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("IOCTL ornegi iceren character device driver");
MODULE_VERSION("1.0");

/* Global değişkenler */
static int major_number;
static struct class *device_class;
static struct device *device_dev;
static int internal_value = 0;
static int access_count = 0;

static int dev_open(struct inode *inode, struct file *file)
{
    access_count++;
    pr_info("%s: Device opened (access #%d)\n", DEVICE_NAME, access_count);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("%s: Device closed\n", DEVICE_NAME);
    return 0;
}

/* IOCTL handler */
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int value;
    struct device_info info;
    void __user *user_ptr = (void __user *)arg;

    switch (cmd) {
    case IOCTL_RESET:
        pr_info("%s: IOCTL_RESET\n", DEVICE_NAME);
        internal_value = 0;
        break;

    case IOCTL_GET_VALUE:
        pr_info("%s: IOCTL_GET_VALUE = %d\n", DEVICE_NAME, internal_value);
        if (copy_to_user(user_ptr, &internal_value, sizeof(int)))
            return -EFAULT;
        break;

    case IOCTL_SET_VALUE:
        if (copy_from_user(&value, user_ptr, sizeof(int)))
            return -EFAULT;
        internal_value = value;
        pr_info("%s: IOCTL_SET_VALUE = %d\n", DEVICE_NAME, internal_value);
        break;

    case IOCTL_GET_INFO:
        info.value = internal_value;
        info.access_count = access_count;
        strncpy(info.name, DEVICE_NAME, sizeof(info.name) - 1);
        info.name[sizeof(info.name) - 1] = '\0';
        
        if (copy_to_user(user_ptr, &info, sizeof(struct device_info)))
            return -EFAULT;
        pr_info("%s: IOCTL_GET_INFO\n", DEVICE_NAME);
        break;

    default:
        pr_warn("%s: Unknown IOCTL command: %u\n", DEVICE_NAME, cmd);
        return -ENOTTY;
    }

    return 0;
}

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = dev_open,
    .release        = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

static int __init ioctl_init(void)
{
    pr_info("%s: Initializing\n", DEVICE_NAME);

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
        return major_number;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    device_class = class_create(CLASS_NAME);
#else
    device_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
    if (IS_ERR(device_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(device_class);
    }

    device_dev = device_create(device_class, NULL,
                               MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device_dev)) {
        class_destroy(device_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(device_dev);
    }

    pr_info("%s: Device created at /dev/%s (major=%d)\n",
            DEVICE_NAME, DEVICE_NAME, major_number);
    return 0;
}

static void __exit ioctl_exit(void)
{
    device_destroy(device_class, MKDEV(major_number, 0));
    class_unregister(device_class);
    class_destroy(device_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("%s: Device removed\n", DEVICE_NAME);
}

module_init(ioctl_init);
module_exit(ioctl_exit);
