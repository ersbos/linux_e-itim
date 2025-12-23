/* ioctl_test.c - IOCTL driver test programı (User-space)
 * Ders 7: Kernel Modüller ve Aygıt Sürücüler (Bölüm 2)
 * 
 * Bu program ioctl_driver.ko modülünü test eder.
 * 
 * Derleme: gcc -o ioctl_test ioctl_test.c
 * Çalıştırma: sudo ./ioctl_test
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

/* IOCTL komut tanımları - driver ile aynı olmalı */
#define IOCTL_MAGIC 'k'
#define IOCTL_RESET        _IO(IOCTL_MAGIC, 0)
#define IOCTL_GET_VALUE    _IOR(IOCTL_MAGIC, 1, int)
#define IOCTL_SET_VALUE    _IOW(IOCTL_MAGIC, 2, int)
#define IOCTL_GET_INFO     _IOR(IOCTL_MAGIC, 3, struct device_info)

/* Bilgi yapısı - driver ile aynı olmalı */
struct device_info {
    int value;
    int access_count;
    char name[32];
};

#define DEVICE_PATH "/dev/ioctldev"

int main(void)
{
    int fd;
    int value;
    struct device_info info;

    printf("IOCTL Test Programı\n");
    printf("===================\n\n");

    /* Cihazı aç */
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("open");
        printf("Hata: %s acilamadi. Modul yuklu mu?\n", DEVICE_PATH);
        return 1;
    }
    printf("Cihaz acildi: %s\n\n", DEVICE_PATH);

    /* Test 1: Değer ayarla */
    value = 42;
    printf("Test 1: Deger ayarla (SET_VALUE = %d)\n", value);
    if (ioctl(fd, IOCTL_SET_VALUE, &value) < 0) {
        perror("ioctl SET_VALUE");
    }

    /* Test 2: Değer oku */
    value = 0;
    printf("Test 2: Deger oku (GET_VALUE)\n");
    if (ioctl(fd, IOCTL_GET_VALUE, &value) < 0) {
        perror("ioctl GET_VALUE");
    } else {
        printf("         Okunan deger: %d\n", value);
    }

    /* Test 3: Bilgi yapısı al */
    printf("Test 3: Cihaz bilgisi al (GET_INFO)\n");
    memset(&info, 0, sizeof(info));
    if (ioctl(fd, IOCTL_GET_INFO, &info) < 0) {
        perror("ioctl GET_INFO");
    } else {
        printf("         Cihaz adi: %s\n", info.name);
        printf("         Deger: %d\n", info.value);
        printf("         Erisim sayisi: %d\n", info.access_count);
    }

    /* Test 4: Reset */
    printf("Test 4: Reset (RESET)\n");
    if (ioctl(fd, IOCTL_RESET) < 0) {
        perror("ioctl RESET");
    }

    /* Test 5: Reset sonrası değer kontrolü */
    printf("Test 5: Reset sonrasi deger kontrolu\n");
    if (ioctl(fd, IOCTL_GET_VALUE, &value) < 0) {
        perror("ioctl GET_VALUE");
    } else {
        printf("         Deger (sifir olmali): %d\n", value);
    }

    close(fd);
    printf("\nTest tamamlandi.\n");

    return 0;
}
