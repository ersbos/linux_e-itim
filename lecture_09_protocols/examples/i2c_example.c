/* i2c_example.c - I2C iletişim örneği
 * Ders 9: İletişim Protokolleri
 * 
 * Bu program I2C bus üzerinden bir cihazla iletişim kurar.
 * Örnek: DS3231 RTC modülü (adres 0x68)
 * 
 * Derleme: gcc -o i2c_example i2c_example.c
 * Çalıştırma: sudo ./i2c_example /dev/i2c-1 0x68
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

/* BCD dönüşüm makroları */
#define BCD_TO_DEC(x) (((x) >> 4) * 10 + ((x) & 0x0F))
#define DEC_TO_BCD(x) ((((x) / 10) << 4) | ((x) % 10))

/* Hata mesajı ve çıkış */
void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* I2C register okuma */
int i2c_read_byte(int fd, int reg)
{
    unsigned char buf[1];
    
    /* Register adresini gönder */
    buf[0] = (unsigned char)reg;
    if (write(fd, buf, 1) != 1) {
        perror("i2c write (register)");
        return -1;
    }
    
    /* Değeri oku */
    if (read(fd, buf, 1) != 1) {
        perror("i2c read");
        return -1;
    }
    
    return buf[0];
}

/* I2C register yazma */
int i2c_write_byte(int fd, int reg, int value)
{
    unsigned char buf[2];
    
    buf[0] = (unsigned char)reg;
    buf[1] = (unsigned char)value;
    
    if (write(fd, buf, 2) != 2) {
        perror("i2c write");
        return -1;
    }
    
    return 0;
}

/* I2C cihaz tarama */
void i2c_scan(int fd)
{
    int addr;
    int found = 0;
    
    printf("I2C Bus Taramasi\n");
    printf("================\n\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    
    for (addr = 0; addr < 128; addr++) {
        if (addr % 16 == 0)
            printf("%02x: ", addr);
        
        /* Adres atla (reserved) */
        if (addr < 0x03 || addr > 0x77) {
            printf("   ");
        } else {
            /* Slave adresini ayarla */
            if (ioctl(fd, I2C_SLAVE, addr) < 0) {
                printf("XX ");
            } else {
                /* Cihaz var mı test et */
                char buf[1];
                if (read(fd, buf, 1) == 1) {
                    printf("%02x ", addr);
                    found++;
                } else {
                    printf("-- ");
                }
            }
        }
        
        if (addr % 16 == 15)
            printf("\n");
    }
    
    printf("\n%d cihaz bulundu.\n", found);
}

/* DS3231 RTC zamanı okuma */
void read_ds3231_time(int fd)
{
    int sec, min, hour, day, date, month, year;
    
    printf("\nDS3231 RTC Okuma\n");
    printf("================\n");
    
    sec   = BCD_TO_DEC(i2c_read_byte(fd, 0x00) & 0x7F);
    min   = BCD_TO_DEC(i2c_read_byte(fd, 0x01));
    hour  = BCD_TO_DEC(i2c_read_byte(fd, 0x02) & 0x3F);  /* 24 saat formatı */
    day   = BCD_TO_DEC(i2c_read_byte(fd, 0x03));
    date  = BCD_TO_DEC(i2c_read_byte(fd, 0x04));
    month = BCD_TO_DEC(i2c_read_byte(fd, 0x05) & 0x1F);
    year  = BCD_TO_DEC(i2c_read_byte(fd, 0x06));
    
    printf("Tarih: %02d/%02d/20%02d\n", date, month, year);
    printf("Saat:  %02d:%02d:%02d\n", hour, min, sec);
    
    /* Sıcaklık sensörü (DS3231'de var) */
    int temp_msb = i2c_read_byte(fd, 0x11);
    int temp_lsb = i2c_read_byte(fd, 0x12);
    float temp = temp_msb + (temp_lsb >> 6) * 0.25;
    printf("Sicaklik: %.2f C\n", temp);
}

int main(int argc, char *argv[])
{
    int fd;
    char *device = "/dev/i2c-1";
    int slave_addr = 0x68;  /* DS3231 varsayılan adresi */

    /* Argüman kontrolü */
    if (argc >= 2)
        device = argv[1];
    if (argc >= 3)
        slave_addr = strtol(argv[2], NULL, 16);

    printf("I2C Iletisim Ornegi\n");
    printf("===================\n");
    printf("Cihaz: %s\n", device);
    printf("Slave adresi: 0x%02x\n\n", slave_addr);

    /* I2C bus'ı aç */
    fd = open(device, O_RDWR);
    if (fd < 0)
        exit_sys("open");

    /* Argüman olarak "scan" verilmişse tarama yap */
    if (argc >= 2 && strcmp(argv[1], "scan") == 0) {
        i2c_scan(fd);
        close(fd);
        return 0;
    }

    /* Slave adresini ayarla */
    if (ioctl(fd, I2C_SLAVE, slave_addr) < 0)
        exit_sys("ioctl I2C_SLAVE");

    /* Test: İlk register'ı oku */
    int value = i2c_read_byte(fd, 0x00);
    if (value < 0) {
        printf("Hata: Cihaz yanit vermiyor.\n");
        printf("i2cdetect -y 1 komutu ile cihazlari kontrol edin.\n");
        close(fd);
        return 1;
    }

    printf("Register 0x00 degeri: 0x%02x\n", value);

    /* DS3231 ise zamanı oku */
    if (slave_addr == 0x68) {
        read_ds3231_time(fd);
    }

    close(fd);
    printf("\nI2C baglantisi kapatildi.\n");

    return 0;
}
