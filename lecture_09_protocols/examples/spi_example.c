/* spi_example.c - SPI iletişim örneği
 * Ders 9: İletişim Protokolleri
 * 
 * Bu program SPI bus üzerinden veri transferi yapar.
 * 
 * Derleme: gcc -o spi_example spi_example.c
 * Çalıştırma: sudo ./spi_example /dev/spidev0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

/* Varsayılan SPI parametreleri */
#define SPI_MODE        SPI_MODE_0
#define SPI_BITS        8
#define SPI_SPEED       1000000  /* 1 MHz */

/* Hata mesajı ve çıkış */
void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* SPI yapılandırması */
int configure_spi(int fd, int mode, int bits, int speed)
{
    /* SPI modu */
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("SPI_IOC_WR_MODE");
        return -1;
    }
    
    /* Bit sayısı */
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("SPI_IOC_WR_BITS_PER_WORD");
        return -1;
    }
    
    /* Hız */
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("SPI_IOC_WR_MAX_SPEED_HZ");
        return -1;
    }
    
    return 0;
}

/* SPI bilgilerini görüntüle */
void show_spi_info(int fd)
{
    unsigned char mode;
    unsigned char bits;
    unsigned int speed;
    
    ioctl(fd, SPI_IOC_RD_MODE, &mode);
    ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    
    printf("SPI Ayarlari:\n");
    printf("  Mode: %d\n", mode);
    printf("  Bits per word: %d\n", bits);
    printf("  Max speed: %d Hz\n", speed);
    printf("\n");
}

/* Full-duplex SPI transfer */
int spi_transfer(int fd, unsigned char *tx, unsigned char *rx, int len)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = SPI_SPEED,
        .delay_usecs = 0,
        .bits_per_word = SPI_BITS,
        .cs_change = 0,
    };
    
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI_IOC_MESSAGE");
        return -1;
    }
    
    return 0;
}

/* Hex dump */
void hex_dump(const char *label, unsigned char *buf, int len)
{
    int i;
    printf("%s: ", label);
    for (i = 0; i < len; i++)
        printf("0x%02X ", buf[i]);
    printf("\n");
}

/* Loopback testi (MOSI-MISO bağlı ise) */
void loopback_test(int fd)
{
    unsigned char tx[] = {0xAA, 0x55, 0x12, 0x34, 0xDE, 0xAD, 0xBE, 0xEF};
    unsigned char rx[sizeof(tx)] = {0};
    int i;
    int match = 1;
    
    printf("Loopback Testi (MOSI-MISO bagli olmali)\n");
    printf("=======================================\n");
    
    hex_dump("TX", tx, sizeof(tx));
    
    if (spi_transfer(fd, tx, rx, sizeof(tx)) < 0) {
        printf("Transfer hatasi!\n");
        return;
    }
    
    hex_dump("RX", rx, sizeof(rx));
    
    /* Karşılaştır */
    for (i = 0; i < sizeof(tx); i++) {
        if (tx[i] != rx[i]) {
            match = 0;
            break;
        }
    }
    
    if (match) {
        printf("Sonuc: BASARILI (TX = RX)\n");
    } else {
        printf("Sonuc: FARKLI (Loopback bagli olmayabilir)\n");
    }
}

/* örnek: SPI Flash okuma (ID komutu) */
void read_flash_id(int fd)
{
    unsigned char tx[4] = {0x9F, 0x00, 0x00, 0x00};  /* JEDEC ID komutu */
    unsigned char rx[4] = {0};
    
    printf("\nSPI Flash ID Okuma (0x9F komutu)\n");
    printf("================================\n");
    
    if (spi_transfer(fd, tx, rx, sizeof(tx)) < 0) {
        printf("Transfer hatasi!\n");
        return;
    }
    
    printf("Manufacturer ID: 0x%02X\n", rx[1]);
    printf("Device ID: 0x%02X 0x%02X\n", rx[2], rx[3]);
    
    /* Bilinen üreticiler */
    switch (rx[1]) {
        case 0xEF: printf("Uretici: Winbond\n"); break;
        case 0xC2: printf("Uretici: Macronix\n"); break;
        case 0x20: printf("Uretici: Micron\n"); break;
        case 0x01: printf("Uretici: Spansion\n"); break;
        case 0x1F: printf("Uretici: Atmel\n"); break;
        default:   printf("Uretici: Bilinmiyor\n"); break;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    char *device = "/dev/spidev0.0";

    /* Argüman kontrolü */
    if (argc >= 2)
        device = argv[1];

    printf("SPI Iletisim Ornegi\n");
    printf("===================\n");
    printf("Cihaz: %s\n\n", device);

    /* SPI cihazını aç */
    fd = open(device, O_RDWR);
    if (fd < 0)
        exit_sys("open");

    /* Yapılandırma */
    if (configure_spi(fd, SPI_MODE, SPI_BITS, SPI_SPEED) < 0) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* Ayarları görüntüle */
    show_spi_info(fd);

    /* Testler */
    loopback_test(fd);
    read_flash_id(fd);

    close(fd);
    printf("\nSPI baglantisi kapatildi.\n");

    return 0;
}
