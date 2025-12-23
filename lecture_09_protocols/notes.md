# Ders 9: İletişim Protokolleri (I2C, SPI, UART)

> **Konu:** Gömülü Sistemlerde Seri İletişim Protokolleri

---

## 📋 Bu Derste İşlenenler

- [x] UART/Serial iletişim
- [x] I2C protokolü
- [x] SPI protokolü
- [x] Linux'ta kullanımları
- [x] Örnek kodlar

---

## 📊 Protokol Karşılaştırması

| Özellik | UART | I2C | SPI |
|---------|------|-----|-----|
| **Hat Sayısı** | 2 (TX, RX) | 2 (SDA, SCL) | 4+ (MOSI, MISO, CLK, CS) |
| **Yön** | Full-duplex | Half-duplex | Full-duplex |
| **Hız** | 115200 bps tipik | 100-400 kHz | MHz seviyesi |
| **Cihaz Sayısı** | 1:1 | Multi-master/slave | 1 master, N slave |
| **Mesafe** | Uzun | Kısa (<1m) | Çok kısa |
| **Kullanım** | Debug, GPS, modem | Sensörler, EEPROM | Display, SD card |

---

## 📟 UART (Serial)

### UART Nedir?

**UART** = Universal Asynchronous Receiver/Transmitter

```
┌─────────┐         ┌─────────┐
│ Device1 │ TX ──── RX │ Device2 │
│         │ RX ──── TX │         │
│         │ GND ─── GND│         │
└─────────┘         └─────────┘
```

### Linux'ta UART

```bash
# Seri portları listele
ls /dev/tty*

# BBB: /dev/ttyO0, /dev/ttyO1, ...
# RPi: /dev/ttyAMA0, /dev/ttyS0, ...

# Minicom ile bağlan
sudo minicom -D /dev/ttyUSB0 -b 115200

# Screen ile bağlan
screen /dev/ttyUSB0 115200

# Basit okuma/yazma
echo "test" > /dev/ttyUSB0
cat /dev/ttyUSB0
```

### C ile UART

```c
/* uart_example.c */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main(void)
{
    int fd;
    struct termios options;
    
    /* Port aç */
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    /* Ayarları al */
    tcgetattr(fd, &options);
    
    /* Baud rate: 115200 */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
    /* 8N1 */
    options.c_cflag &= ~PARENB;  /* No parity */
    options.c_cflag &= ~CSTOPB;  /* 1 stop bit */
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      /* 8 bits */
    
    /* Raw mode */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    
    /* Ayarları uygula */
    tcsetattr(fd, TCSANOW, &options);
    
    /* Yazma */
    char *msg = "Hello UART!\r\n";
    write(fd, msg, strlen(msg));
    
    /* Okuma */
    char buf[256];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Received: %s\n", buf);
    }
    
    close(fd);
    return 0;
}
```

---

## 🔗 I2C Protokolü

### I2C Nedir?

**I2C** = Inter-Integrated Circuit (Philips)

```
        ┌──────────────────────────────────┐
   3.3V ┤                                  │
        │  ┌────┐    ┌────┐    ┌────┐     │
  SDA ──┼──┤Sens├────┤EPRM├────┤RTC ├─────┤
        │  └──┬─┘    └──┬─┘    └──┬─┘     │
  SCL ──┼─────┴─────────┴─────────┴───────┤
        │                                  │
   GND ─┴──────────────────────────────────┘
```

### Linux'ta I2C

```bash
# I2C araçları kur
sudo apt install i2c-tools

# I2C bus'ları listele
ls /dev/i2c-*

# Cihazları tara
sudo i2cdetect -y 1

# Register oku
sudo i2cget -y 1 0x68 0x00

# Register yaz
sudo i2cset -y 1 0x68 0x00 0x01
```

### C ile I2C

```c
/* i2c_example.c */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS     "/dev/i2c-1"
#define DEVICE_ADDR 0x68  /* Örnek: DS3231 RTC */

int main(void)
{
    int fd;
    unsigned char reg = 0x00;  /* Seconds register */
    unsigned char data;
    
    /* I2C bus aç */
    fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    /* Slave adresini ayarla */
    if (ioctl(fd, I2C_SLAVE, DEVICE_ADDR) < 0) {
        perror("ioctl");
        close(fd);
        return 1;
    }
    
    /* Register oku */
    if (write(fd, &reg, 1) != 1) {
        perror("write");
        close(fd);
        return 1;
    }
    
    if (read(fd, &data, 1) != 1) {
        perror("read");
        close(fd);
        return 1;
    }
    
    printf("Register 0x%02X = 0x%02X\n", reg, data);
    
    close(fd);
    return 0;
}
```

---

## ⚡ SPI Protokolü

### SPI Nedir?

**SPI** = Serial Peripheral Interface (Motorola)

```
        Master                 Slave
    ┌──────────┐           ┌──────────┐
    │   MOSI   ├───────────┤   MOSI   │
    │   MISO   ├───────────┤   MISO   │
    │   SCLK   ├───────────┤   SCLK   │
    │   CS0    ├───────────┤   CS     │
    └──────────┘           └──────────┘
```

### Linux'ta SPI

```bash
# SPI cihazlarını listele
ls /dev/spidev*

# SPI test
# Loopback: MOSI-MISO bağla
sudo apt install spi-tools
spi-pipe -d /dev/spidev0.0 -s 1000000 < /dev/urandom | hexdump
```

### C ile SPI

```c
/* spi_example.c */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED  1000000  /* 1 MHz */

int main(void)
{
    int fd;
    unsigned char tx[] = {0x9F, 0x00, 0x00, 0x00};  /* JEDEC ID komutu */
    unsigned char rx[4] = {0};
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 4,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
    };
    
    fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    /* SPI modu ayarla */
    unsigned char mode = SPI_MODE_0;
    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    
    /* Transfer yap */
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI transfer");
        close(fd);
        return 1;
    }
    
    printf("Received: %02X %02X %02X %02X\n", 
           rx[0], rx[1], rx[2], rx[3]);
    
    close(fd);
    return 0;
}
```

---

## 🔧 BBB'de Protokol Pinleri

| Protokol | Pin | Açıklama |
|----------|-----|----------|
| UART1 TX | P9_24 | Serial çıkış |
| UART1 RX | P9_26 | Serial giriş |
| I2C1 SDA | P9_18 | I2C data |
| I2C1 SCL | P9_17 | I2C clock |
| I2C2 SDA | P9_20 | I2C data |
| I2C2 SCL | P9_19 | I2C clock |
| SPI0 MOSI | P9_18 | SPI data out |
| SPI0 MISO | P9_21 | SPI data in |
| SPI0 CLK | P9_22 | SPI clock |
| SPI0 CS | P9_17 | Chip select |

---

## 🎯 Pratik

1. USB-UART ile PC ve BBB/RPi arasında iletişim kur
2. I2C ile sensör (DS3231 RTC veya DHT) oku
3. `i2cdetect` ile bağlı cihazları bul
4. Bir sensörden veri oku ve ekrana yazdır

---

*Sonraki ders: Buildroot ve Yocto*
