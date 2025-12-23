/**
 * @file params_module.c
 * @brief Parametreli Kernel Modülü
 * 
 * Derleme:
 *   make
 * 
 * Yükleme (parametreli):
 *   sudo insmod params_module.ko count=5 msg="Merhaba Dunya"
 * 
 * Parametre okuma:
 *   cat /sys/module/params_module/parameters/count
 *   cat /sys/module/params_module/parameters/msg
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Kursu");
MODULE_DESCRIPTION("Parametreli kernel modulu ornegi");
MODULE_VERSION("1.0");

/* Modül parametreleri */
static int count = 1;
static char *msg = "Varsayilan mesaj";
static int values[5] = {1, 2, 3, 4, 5};
static int values_count = 5;

/* Parametre tanımları */
module_param(count, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(count, "Mesajin tekrar sayisi (varsayilan: 1)");

module_param(msg, charp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(msg, "Yazdirilacak mesaj metni");

module_param_array(values, int, &values_count, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(values, "Ornek tamsayi dizisi");

static int __init params_init(void)
{
    int i;
    
    pr_info("========================================\n");
    pr_info("Parametreli Modul Yuklendi\n");
    pr_info("========================================\n");
    
    pr_info("Parametre count = %d\n", count);
    pr_info("Parametre msg   = \"%s\"\n", msg);
    
    pr_info("Dizi values[%d] = { ", values_count);
    for (i = 0; i < values_count; i++) {
        pr_cont("%d ", values[i]);
    }
    pr_cont("}\n");
    
    pr_info("----------------------------------------\n");
    
    /* Mesajı count kez yazdır */
    for (i = 0; i < count; i++) {
        pr_info("[%d] %s\n", i + 1, msg);
    }
    
    pr_info("========================================\n");
    
    return 0;
}

static void __exit params_exit(void)
{
    pr_info("========================================\n");
    pr_info("Parametreli Modul Kaldirildi\n");
    pr_info("========================================\n");
}

module_init(params_init);
module_exit(params_exit);
