/**
 * @file led_blink.c
 * @brief GPIO ile LED yakıp söndürme
 * 
 * Devre bağlantısı (BBB):
 *   - P9_12 (GPIO60) --> 470Ω direnç --> LED anot
 *   - LED katot --> GND (P9_01)
 * 
 * Derleme (BBB üzerinde):
 *   gcc -o led_blink led_blink.c
 * 
 * Cross-compile (Host'ta):
 *   arm-linux-gnueabihf-gcc -o led_blink led_blink.c
 * 
 * Çalıştırma:
 *   sudo ./led_blink
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

/* GPIO ayarları - ihtiyaca göre değiştirin */
#define GPIO_PIN        "60"    /* P9_12, GPIO1_28 */
#define GPIO_EXPORT     "/sys/class/gpio/export"
#define GPIO_UNEXPORT   "/sys/class/gpio/unexport"
#define GPIO_PATH       "/sys/class/gpio/gpio" GPIO_PIN
#define GPIO_DIRECTION  GPIO_PATH "/direction"
#define GPIO_VALUE      GPIO_PATH "/value"

/* Blink parametreleri */
#define BLINK_DELAY_MS  500

/* Global değişkenler */
static volatile int running = 1;

/* Fonksiyon prototipleri */
void signal_handler(int sig);
int gpio_export(const char *pin);
int gpio_unexport(const char *pin);
int gpio_set_direction(const char *dir);
int gpio_set_value(int fd, int value);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    int value = 0;
    int count = 0;
    
    /* SIGINT (Ctrl+C) yakalayıcı */
    signal(SIGINT, signal_handler);
    
    printf("LED Blink Programi\n");
    printf("==================\n");
    printf("GPIO Pin: %s\n", GPIO_PIN);
    printf("Ctrl+C ile cikis\n\n");
    
    /* GPIO'yu export et */
    if (gpio_export(GPIO_PIN) < 0) {
        fprintf(stderr, "GPIO export basarisiz (zaten export edilmis olabilir)\n");
    }
    
    /* Biraz bekle (sysfs hazırlanması için) */
    usleep(100000);
    
    /* Output olarak ayarla */
    if (gpio_set_direction("out") < 0) {
        exit_sys("direction ayarlanamadi");
    }
    
    /* Value dosyasını aç */
    fd = open(GPIO_VALUE, O_RDWR);
    if (fd < 0) {
        exit_sys("value dosyasi acilamadi");
    }
    
    printf("LED blink basliyor...\n");
    
    /* Ana döngü */
    while (running) {
        /* LED durumunu değiştir */
        if (gpio_set_value(fd, value) < 0) {
            break;
        }
        
        printf("\r[%05d] LED: %s ", count++, value ? "ON " : "OFF");
        fflush(stdout);
        
        value = !value;
        usleep(BLINK_DELAY_MS * 1000);
    }
    
    /* Temizlik */
    printf("\n\nTemizlik yapiliyor...\n");
    
    /* LED'i söndür */
    gpio_set_value(fd, 0);
    close(fd);
    
    /* GPIO'yu unexport et */
    gpio_unexport(GPIO_PIN);
    
    printf("Program sonlandi.\n");
    return 0;
}

void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

int gpio_export(const char *pin)
{
    int fd = open(GPIO_EXPORT, O_WRONLY);
    if (fd < 0) return -1;
    
    int ret = write(fd, pin, strlen(pin));
    close(fd);
    
    return (ret > 0) ? 0 : -1;
}

int gpio_unexport(const char *pin)
{
    int fd = open(GPIO_UNEXPORT, O_WRONLY);
    if (fd < 0) return -1;
    
    int ret = write(fd, pin, strlen(pin));
    close(fd);
    
    return (ret > 0) ? 0 : -1;
}

int gpio_set_direction(const char *dir)
{
    int fd = open(GPIO_DIRECTION, O_WRONLY);
    if (fd < 0) return -1;
    
    int ret = write(fd, dir, strlen(dir));
    close(fd);
    
    return (ret > 0) ? 0 : -1;
}

int gpio_set_value(int fd, int value)
{
    char buf = value ? '1' : '0';
    
    if (lseek(fd, 0, SEEK_SET) < 0) return -1;
    if (write(fd, &buf, 1) < 0) return -1;
    
    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
