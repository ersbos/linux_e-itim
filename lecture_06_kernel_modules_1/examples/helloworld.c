/**
 * @file helloworld.c
 * @brief İlk Kernel Modülü - Hello World
 * 
 * Derleme:
 *   make
 * 
 * Yükleme:
 *   sudo insmod helloworld.ko
 * 
 * Log kontrolü:
 *   dmesg | tail -5
 * 
 * Kaldırma:
 *   sudo rmmod helloworld
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

/* Modül meta bilgileri */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("Ilk Hello World kernel modulu");
MODULE_VERSION("1.0");

/**
 * @brief Modül yüklendiğinde çağrılır
 * @return 0 başarı, negatif değer hata
 */
static int __init hello_init(void)
{
    pr_info("=================================\n");
    pr_info("Hello Kernel World!\n");
    pr_info("Modul yuklendi.\n");
    pr_info("=================================\n");
    
    return 0;
}

/**
 * @brief Modül kaldırıldığında çağrılır
 */
static void __exit hello_exit(void)
{
    pr_info("=================================\n");
    pr_info("Goodbye Kernel World!\n");
    pr_info("Modul kaldirildi.\n");
    pr_info("=================================\n");
}

module_init(hello_init);
module_exit(hello_exit);
