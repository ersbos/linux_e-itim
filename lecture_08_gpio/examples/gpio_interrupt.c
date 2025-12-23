/**
 * @file gpio_interrupt.c
 * @brief Kesme (interrupt) ile button okuma
 * 
 * Bu program polling yerine select() kullanarak
 * kesme tabanlı GPIO okuma yapar. CPU kullanımı çok düşüktür.
 * 
 * Devre bağlantısı (BBB):
 *   Button:
 *     - 3.3V --> 10kΩ --> P9_16 (GPIO51) --> Button --> GND
 * 
 * Derleme:
 *   gcc -o gpio_interrupt gpio_interrupt.c
 * 
 * Çalıştırma:
 *   sudo ./gpio_interrupt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>

#define GPIO_PIN        "51"    /* P9_16, GPIO1_19 */
#define GPIO_EXPORT     "/sys/class/gpio/export"
#define GPIO_UNEXPORT   "/sys/class/gpio/unexport"
#define GPIO_PATH       "/sys/class/gpio/gpio" GPIO_PIN

static volatile int running = 1;

void signal_handler(int sig);
void gpio_write(const char *path, const char *value);

int main(void)
{
    int fd;
    fd_set exceptfds;
    char buf[2];
    int press_count = 0;
    
    signal(SIGINT, signal_handler);
    
    printf("GPIO Interrupt Programi\n");
    printf("=======================\n");
    printf("GPIO Pin: %s (falling edge)\n", GPIO_PIN);
    printf("Ctrl+C ile cikis\n\n");
    
    /* GPIO'yu export et */
    gpio_write(GPIO_EXPORT, GPIO_PIN);
    usleep(100000);
    
    /* Input olarak ayarla */
    gpio_write(GPIO_PATH "/direction", "in");
    
    /* Falling edge kesme ayarla (button basıldığında 1->0) */
    gpio_write(GPIO_PATH "/edge", "falling");
    
    /* Value dosyasını aç */
    fd = open(GPIO_PATH "/value", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    /* İlk okuma (select öncesi gerekli) */
    if (read(fd, buf, 1) < 0) {
        perror("read");
        exit(1);
    }
    
    printf("Kesme bekleniyor... (Button'a basin)\n\n");
    
    while (running) {
        FD_ZERO(&exceptfds);
        FD_SET(fd, &exceptfds);
        
        /* select: kesme bekle (sonsuz timeout) */
        int ret = select(fd + 1, NULL, NULL, &exceptfds, NULL);
        
        if (ret < 0) {
            if (running) perror("select");
            break;
        }
        
        if (ret > 0 && FD_ISSET(fd, &exceptfds)) {
            /* Kesme tetiklendi! */
            lseek(fd, 0, SEEK_SET);
            if (read(fd, buf, 1) < 0) {
                perror("read");
                break;
            }
            
            press_count++;
            printf("[%03d] KESME! Button basildi.\n", press_count);
        }
    }
    
    /* Temizlik */
    printf("\nTemizlik yapiliyor...\n");
    printf("Toplam %d kez basildi.\n", press_count);
    
    close(fd);
    gpio_write(GPIO_UNEXPORT, GPIO_PIN);
    
    printf("Program sonlandi.\n");
    return 0;
}

void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

void gpio_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}
