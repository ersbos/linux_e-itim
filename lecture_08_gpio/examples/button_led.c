/**
 * @file button_led.c
 * @brief Button ile LED kontrolü
 * 
 * Devre bağlantısı (BBB):
 *   LED:
 *     - P9_12 (GPIO60) --> 470Ω --> LED --> GND
 *   
 *   Button:
 *     - 3.3V --> 10kΩ --> P9_16 (GPIO51) --> Button --> GND
 * 
 * Çalışma:
 *   - Button basılı değil (GPIO51 = 1): LED sönük
 *   - Button basılı (GPIO51 = 0): LED yanık
 * 
 * Derleme:
 *   gcc -o button_led button_led.c
 *   arm-linux-gnueabihf-gcc -o button_led button_led.c  # Cross-compile
 * 
 * Çalıştırma:
 *   sudo ./button_led
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

/* GPIO pinleri */
#define LED_PIN         "60"    /* P9_12, GPIO1_28 - Output */
#define BUTTON_PIN      "51"    /* P9_16, GPIO1_19 - Input */

#define GPIO_EXPORT     "/sys/class/gpio/export"
#define GPIO_UNEXPORT   "/sys/class/gpio/unexport"

#define LED_PATH        "/sys/class/gpio/gpio" LED_PIN
#define BUTTON_PATH     "/sys/class/gpio/gpio" BUTTON_PIN

static volatile int running = 1;

void signal_handler(int sig);
int gpio_export(const char *pin);
int gpio_unexport(const char *pin);
void gpio_write(const char *path, const char *value);
int gpio_read(int fd);

int main(void)
{
    int led_fd, button_fd;
    int button_state, prev_state = 1;
    int led_state = 0;
    
    signal(SIGINT, signal_handler);
    
    printf("Button-LED Kontrol Programi\n");
    printf("===========================\n");
    printf("LED: GPIO%s, Button: GPIO%s\n", LED_PIN, BUTTON_PIN);
    printf("Ctrl+C ile cikis\n\n");
    
    /* GPIO'ları export et */
    gpio_export(LED_PIN);
    gpio_export(BUTTON_PIN);
    usleep(100000);
    
    /* Yönleri ayarla */
    gpio_write(LED_PATH "/direction", "out");
    gpio_write(BUTTON_PATH "/direction", "in");
    
    /* Dosyaları aç */
    led_fd = open(LED_PATH "/value", O_RDWR);
    button_fd = open(BUTTON_PATH "/value", O_RDONLY);
    
    if (led_fd < 0 || button_fd < 0) {
        perror("GPIO dosyalari acilamadi");
        exit(1);
    }
    
    printf("Baslamak icin hazir!\n");
    printf("Button'a basin...\n\n");
    
    while (running) {
        /* Button durumunu oku */
        button_state = gpio_read(button_fd);
        
        /* Durum değişikliği tespit et */
        if (button_state != prev_state) {
            if (button_state == 0) {
                /* Button basıldı */
                led_state = 1;
                printf("Button: PRESSED  -> LED: ON\n");
            } else {
                /* Button bırakıldı */
                led_state = 0;
                printf("Button: RELEASED -> LED: OFF\n");
            }
            
            /* LED'i güncelle */
            lseek(led_fd, 0, SEEK_SET);
            char c = led_state ? '1' : '0';
            write(led_fd, &c, 1);
            
            prev_state = button_state;
        }
        
        usleep(10000);  /* 10ms polling */
    }
    
    /* Temizlik */
    printf("\nTemizlik yapiliyor...\n");
    
    lseek(led_fd, 0, SEEK_SET);
    write(led_fd, "0", 1);
    
    close(led_fd);
    close(button_fd);
    
    gpio_unexport(LED_PIN);
    gpio_unexport(BUTTON_PIN);
    
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
    write(fd, pin, strlen(pin));
    close(fd);
    return 0;
}

int gpio_unexport(const char *pin)
{
    int fd = open(GPIO_UNEXPORT, O_WRONLY);
    if (fd < 0) return -1;
    write(fd, pin, strlen(pin));
    close(fd);
    return 0;
}

void gpio_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

int gpio_read(int fd)
{
    char buf[2] = {0};
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 1);
    return (buf[0] == '1') ? 1 : 0;
}
