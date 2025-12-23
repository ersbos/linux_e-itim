# Ders 9: İletişim Protokolleri (I2C, SPI, UART)

> **Konu:** Gömülü Sistemlerde Seri İletişim Protokolleri
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Protokol karşılaştırması
- [x] UART/Serial iletişim temelleri
- [x] I2C protokolü (detaylı)
- [x] SPI protokolü
- [x] Linux'ta I2C araçları
- [x] C ile I2C programlama
- [x] DS3231 RTC modülü örneği

---

## 📊 Protokol Karşılaştırması

Gömülü sistemlerde çevre birimleriyle iletişim için üç temel seri protokol kullanılmaktadır. Her birinin kendine özgü avantajları ve kullanım alanları vardır:

| Özellik | UART | I2C | SPI |
|---------|------|-----|-----|
| **Hat Sayısı** | 2 (TX, RX) | 2 (SDA, SCL) | 4+ (MOSI, MISO, CLK, CS) |
| **Yön** | Full-duplex | Half-duplex | Full-duplex |
| **Senkronizasyon** | Asenkron | Senkron (clock) | Senkron (clock) |
| **Hız** | ~115200 bps tipik | 100-400 kHz | MHz seviyesi |
| **Cihaz Sayısı** | 1:1 | Multi-master/slave | 1 master, N slave |
| **Adresleme** | Yok | 7 veya 10 bit | Chip Select (CS) ile |
| **Mesafe** | Uzun (~15m) | Kısa (<1m) | Çok kısa (<30cm) |
| **Kullanım** | Debug, GPS, modem | Sensörler, EEPROM, RTC | Display, SD card, Flash |

---

## 📟 UART (Serial) Protokolü

### UART Nedir?

**UART** (Universal Asynchronous Receiver/Transmitter), asenkron seri iletişim protokolüdür. "Asenkron" olması clock hattı gerektirmemesi anlamına gelir; gönderici ve alıcı önceden anlaşılmış bir baud rate üzerinden haberleşir.

UART iletişimi iki ana hat üzerinden gerçekleşir:
- **TX (Transmit):** Veri gönderme hattı
- **RX (Receive):** Veri alma hattı

İki cihaz arasında UART bağlantısı şöyle yapılır:

```
┌─────────┐         ┌─────────┐
│ Device1 │ TX ──── RX │ Device2 │
│         │ RX ──── TX │         │
│         │ GND ─── GND│         │
└─────────┘         └─────────┘
```

TX ve RX hatlarının çapraz bağlandığına dikkat edin: birinin TX'i diğerinin RX'ine gider.

### UART Parametreleri

UART haberleşmesinde her iki tarafın da aynı parametreleri kullanması gerekir:

| Parametre | Tipik Değer | Açıklama |
|-----------|-------------|----------|
| **Baud Rate** | 9600, 115200 | Saniyedeki bit sayısı |
| **Data Bits** | 8 | Veri biti sayısı |
| **Stop Bits** | 1 | Durma biti sayısı |
| **Parity** | None | Hata kontrolü (N/E/O) |

Örneğin "115200 8N1" ifadesi: 115200 baud, 8 data bit, No parity, 1 stop bit anlamına gelir.

### Linux'ta UART

Linux'ta seri portlar `/dev/tty*` dosyaları olarak görünür:

```bash
# Seri portları listele
ls /dev/tty*

# BBB seri portları
ls /dev/ttyO*     # /dev/ttyO0, /dev/ttyO1, ...

# RPi seri portları
ls /dev/ttyAMA*   # /dev/ttyAMA0
ls /dev/ttyS*     # /dev/ttyS0

# USB-Serial adaptörler
ls /dev/ttyUSB*   # /dev/ttyUSB0
```

### Terminal Programları

```bash
# minicom ile bağlan
sudo minicom -D /dev/ttyUSB0 -b 115200

# screen ile bağlan
screen /dev/ttyUSB0 115200

# Basit okuma/yazma (test için)
echo "test" > /dev/ttyUSB0
cat /dev/ttyUSB0
```

### C ile UART Programlama

```c
/* uart_example.c - UART iletişimi */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

void exit_sys(const char *msg);

int main(void)
{
    int fd;
    struct termios options;

    /* Port aç */
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0)
        exit_sys("open");

    /* Mevcut ayarları al */
    if (tcgetattr(fd, &options) < 0)
        exit_sys("tcgetattr");

    /* Baud rate: 115200 */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /* 8N1: 8 data bits, no parity, 1 stop bit */
    options.c_cflag &= ~PARENB;      /* No parity */
    options.c_cflag &= ~CSTOPB;      /* 1 stop bit */
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;          /* 8 data bits */

    /* Local mode, receiver enabled */
    options.c_cflag |= (CLOCAL | CREAD);

    /* Raw mode (non-canonical) */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    /* Read timeout settings */
    options.c_cc[VMIN] = 1;   /* Min 1 byte */
    options.c_cc[VTIME] = 10; /* 1 saniye timeout */

    /* Ayarları uygula */
    if (tcsetattr(fd, TCSANOW, &options) < 0)
        exit_sys("tcsetattr");

    /* Yazma */
    char *msg = "Hello UART!\r\n";
    if (write(fd, msg, strlen(msg)) < 0)
        exit_sys("write");
    printf("Gönderildi: %s", msg);

    /* Okuma */
    char buf[256];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Alındı: %s\n", buf);
    }

    close(fd);
    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## 🔗 I2C Protokolü

### I2C Nedir?

**I2C** (Inter-Integrated Circuit), Philips (şimdi NXP) tarafından geliştirilen senkron seri haberleşme protokolüdür. Yalnızca **iki hat** kullanarak birden fazla cihazla haberleşme sağlar:

- **SCL (Serial Clock Line):** Saat sinyali hattı
- **SDA (Serial Data Line):** Veri hattı

I2C iletişiminde hizmet alan tarafa **master**, hizmet veren tarafa **slave** denilmektedir. Master genellikle ana işlemci (BBB, RPi), slave ise sensör, EEPROM, RTC gibi çevre birimleridir.

```
        Master (BBB/RPi)        Slave 1          Slave 2          Slave 3
             │                    │                │                │
    SCL ─────┼────────────────────┼────────────────┼────────────────┤
             │                    │                │                │
    SDA ─────┼────────────────────┼────────────────┼────────────────┤
             │                    │                │                │
    GND ─────┴────────────────────┴────────────────┴────────────────┘
```

### I2C Özellikleri

**Senkron Haberleşme:** I2C'de clock (SCL) sinyali her zaman master tarafından üretilir. Veri (SDA) hattı clock sinyalinin çıkan kenarında örneklenir.

**Half-Duplex:** Master slave'e yazabilir, slave'den okuyabilir, ancak her iki işlem aynı anda yapılamaz. Önce yazmak, sonra okumak için arada RESTART koşulu kullanılabilir.

**Çoklu Slave Desteği:** Aynı hatta birden fazla slave bağlanabilir. Her slave'in 7-bit veya 10-bit benzersiz bir adresi vardır.

**Adres Tabanlı Seçim:** Hangi slave ile konuşulacağı adres byte'ı ile belirlenir. Master önce slave adresini gönderir, ilgili slave ACK verir.

### I2C Sinyal Protokolü

**START Koşulu:** SCL HIGH iken SDA'nın HIGH'dan LOW'a geçmesi iletimin başlangıcını işaret eder.

```
SDA: ──────┐      
           └─────────
           ↓ START
SCL: ──────────────── (HIGH)
```

**STOP Koşulu:** SCL HIGH iken SDA'nın LOW'dan HIGH'a geçmesi iletimin sonunu işaret eder.

```
SDA:       ┌────────
    ───────┘
           ↑ STOP
SCL: ──────────────── (HIGH)
```

**Veri Transferi:** Bitler MSB'den başlayarak (yüksek anlamlıdan düşüğe) SDA hattına kodlanır. Yazma SCL LOW iken, okuma SCL HIGH iken yapılır.

**ACK/NACK:** Her 8 bitlik veri transferinden sonra alıcı taraf acknowledge (ACK=LOW) veya not-acknowledge (NACK=HIGH) gönderir.

### I2C İletişim Senaryoları

**Tek Byte Yazma:**
```
START │ ADDR+W │ ACK │ DATA │ ACK │ STOP
```

**Çoklu Byte Yazma:**
```
START │ ADDR+W │ ACK │ DATA1 │ ACK │ DATA2 │ ACK │ DATA3 │ ACK │ STOP
```

**Tek Byte Okuma:**
```
START │ ADDR+R │ ACK │ DATA │ NACK │ STOP
```

**Önce Yazma, Sonra Okuma (Register Okuma):**
```
START │ ADDR+W │ ACK │ REG │ ACK │ RE-START │ ADDR+R │ ACK │ DATA │ NACK │ STOP
└────────────┬─────────────┘               └────────────┬────────────────────┘
     Register Seçimi                            Veri Okuma
```

### BBB'de I2C Donanımı

BeagleBone Black'te 3 adet I2C denetleyici bulunmaktadır:

| Denetleyici | Aygıt Dosyası | Pinler | Kullanım |
|-------------|---------------|--------|----------|
| I2C0 | /dev/i2c-0 | - | Kart içi (EEPROM) |
| I2C1 | /dev/i2c-1 | P9_17 (SCL), P9_18 (SDA) | Kullanıcı |
| I2C2 | /dev/i2c-2 | P9_19 (SCL), P9_20 (SDA) | Kullanıcı |

I2C pinlerini kullanmadan önce pin modunu ayarlamamız gerekebilir:

```bash
# Pin modunu I2C olarak ayarla
config-pin P9_19 i2c
config-pin P9_20 i2c
```

### Raspberry Pi'da I2C

Raspberry Pi'da I2C varsayılan olarak devre dışıdır. Etkinleştirmek için:

```bash
sudo raspi-config
# → Interface Options → I2C → Enable
```

Pin bağlantıları:
- **GPIO2 (Pin 3):** I2C1 SDA
- **GPIO3 (Pin 5):** I2C1 SCL

### Linux'ta I2C Araçları

```bash
# i2c-tools kurulumu
sudo apt install i2c-tools

# I2C bus'larını listele
ls /dev/i2c-*

# Bağlı cihazları tara (bus 2'de)
sudo i2cdetect -y 2

# Örnek çıktı:
#      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
# 00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
# 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
# ...
# 60: -- -- -- -- -- -- -- -- 68 -- -- -- -- -- -- --
# 68 = DS3231 RTC adresi

# Belirli adresten register oku
sudo i2cget -y 2 0x68 0x00    # Register 0x00'ı oku

# Belirli adrese register yaz
sudo i2cset -y 2 0x68 0x00 0x01   # 0x01 değerini register 0x00'a yaz

# Tüm register'ları dump et
sudo i2cdump -y 2 0x68
```

---

## 💻 C ile I2C Programlama

Linux'ta I2C programlaması için `/dev/i2c-N` aygıt dosyası açılır ve `ioctl` + `read`/`write` fonksiyonları kullanılır.

### Temel I2C İşlemleri

```c
/* i2c_basic.c - Temel I2C işlemleri */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS     "/dev/i2c-2"
#define SLAVE_ADDR  0x68    /* DS3231 RTC adresi */

void exit_sys(const char *msg);

int main(void)
{
    int fd;
    uint8_t reg = 0x00;  /* Seconds register */
    uint8_t data;

    /* I2C bus'ı aç */
    if ((fd = open(I2C_BUS, O_RDWR)) < 0)
        exit_sys("open");

    /* Slave adresini ayarla */
    if (ioctl(fd, I2C_SLAVE, SLAVE_ADDR) < 0)
        exit_sys("ioctl I2C_SLAVE");

    /* Register seç (yazma) */
    if (write(fd, &reg, 1) != 1)
        exit_sys("write");

    /* Register'ı oku */
    if (read(fd, &data, 1) != 1)
        exit_sys("read");

    printf("Register 0x%02X = 0x%02X\n", reg, data);

    close(fd);
    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

### DS3231 RTC Modülü Örneği

DS3231, yaygın kullanılan bir I2C RTC (Real-Time Clock) modülüdür. Slave adresi 0x68'dir.

**Register Haritası:**

| Adres | Register | Format |
|-------|----------|--------|
| 0x00 | Seconds | BCD |
| 0x01 | Minutes | BCD |
| 0x02 | Hours | BCD |
| 0x03 | Day | BCD |
| 0x04 | Date | BCD |
| 0x05 | Month/Century | BCD |
| 0x06 | Year | BCD |
| 0x11 | Temp MSB | Binary |
| 0x12 | Temp LSB | Binary |

**BCD (Binary Coded Decimal):** Her 4 bit bir ondalık basamağı temsil eder.

```c
/* BCD dönüşüm makroları */
#define BCD_TO_DEC(val)  ((((val) >> 4) * 10) + ((val) & 0x0F))
#define DEC_TO_BCD(val)  ((((val) / 10) << 4) | ((val) % 10))
```

**Tam Örnek: DS3231'den Saat Okuma**

```c
/* ds3231_time.c - DS3231'den saat okuma */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS         "/dev/i2c-2"
#define DS3231_ADDR     0x68

#define REG_SECONDS     0x00
#define REG_MINUTES     0x01
#define REG_HOURS       0x02
#define REG_DATE        0x04
#define REG_MONTH       0x05
#define REG_YEAR        0x06
#define REG_TEMP_MSB    0x11
#define REG_TEMP_LSB    0x12

#define BCD_TO_DEC(val) ((((val) >> 4) * 10) + ((val) & 0x0F))

void exit_sys(const char *msg);

/* Tek register okuma */
int read_register(int fd, uint8_t reg)
{
    uint8_t data;

    if (write(fd, &reg, 1) != 1)
        return -1;
    if (read(fd, &data, 1) != 1)
        return -1;

    return data;
}

/* Birden fazla register okuma */
int read_registers(int fd, uint8_t start_reg, uint8_t *buffer, size_t len)
{
    if (write(fd, &start_reg, 1) != 1)
        return -1;
    if (read(fd, buffer, len) != (ssize_t)len)
        return -1;

    return 0;
}

int main(void)
{
    int fd;
    uint8_t time_data[7];
    int temp_msb, temp_lsb;
    float temperature;

    /* I2C bus'ı aç */
    if ((fd = open(I2C_BUS, O_RDWR)) < 0)
        exit_sys("open");

    /* Slave adresini ayarla */
    if (ioctl(fd, I2C_SLAVE, DS3231_ADDR) < 0)
        exit_sys("ioctl");

    /* Saat verilerini oku (7 byte: sec, min, hour, day, date, month, year) */
    if (read_registers(fd, REG_SECONDS, time_data, 7) < 0)
        exit_sys("read_registers");

    /* Sıcaklık oku */
    temp_msb = read_register(fd, REG_TEMP_MSB);
    temp_lsb = read_register(fd, REG_TEMP_LSB);
    temperature = (float)temp_msb + ((temp_lsb >> 6) * 0.25f);

    /* Sonuçları yazdır */
    printf("═══════════════════════════════════\n");
    printf("       DS3231 RTC Bilgileri        \n");
    printf("═══════════════════════════════════\n");
    printf("Saat:  %02X:%02X:%02X\n",
           time_data[2] & 0x3F,  /* Hours (mask 24h mode bits) */
           time_data[1],         /* Minutes */
           time_data[0]);        /* Seconds */

    printf("Tarih: %02X/%02X/20%02X\n",
           time_data[4],         /* Date */
           time_data[5] & 0x1F,  /* Month */
           time_data[6]);        /* Year */

    printf("Sıcaklık: %.2f°C\n", temperature);
    printf("═══════════════════════════════════\n");

    close(fd);
    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

**Derleme ve Çalıştırma:**
```bash
gcc -o ds3231_time ds3231_time.c
sudo ./ds3231_time
```

---

## ⚡ SPI Protokolü

### SPI Nedir?

**SPI** (Serial Peripheral Interface), Motorola tarafından geliştirilen yüksek hızlı, full-duplex senkron seri protokolüdür. I2C'den daha hızlıdır ancak daha fazla hat gerektirir.

SPI hatları:
- **MOSI (Master Out Slave In):** Master'dan slave'e veri
- **MISO (Master In Slave Out):** Slave'den master'a veri
- **SCLK (Serial Clock):** Saat sinyali
- **CS/SS (Chip Select / Slave Select):** Slave seçimi

```
    Master                 Slave
┌──────────┐           ┌──────────┐
│   MOSI   ├───────────┤   MOSI   │
│   MISO   ├───────────┤   MISO   │
│   SCLK   ├───────────┤   SCLK   │
│   CS0    ├───────────┤   CS     │
└──────────┘           └──────────┘
```

### SPI vs I2C

| Özellik | SPI | I2C |
|---------|-----|-----|
| Hız | Yüksek (MHz) | Düşük (kHz) |
| Full-duplex | Evet | Hayır |
| Hat sayısı | 4+ | 2 |
| Slave başına hat | Her slave için ayrı CS | Tek SDA/SCL |
| Adresleme | CS pin ile | Slave adresi ile |

### Linux'ta SPI

```bash
# SPI cihazlarını listele
ls /dev/spidev*

# SPI test (loopback: MOSI-MISO bağla)
sudo apt install spi-tools
spi-pipe -d /dev/spidev0.0 -s 1000000 < /dev/urandom | hexdump
```

### C ile SPI Programlama

```c
/* spi_example.c - SPI iletişimi */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>

#define SPI_DEVICE  "/dev/spidev0.0"
#define SPI_SPEED   1000000  /* 1 MHz */

void exit_sys(const char *msg);

int main(void)
{
    int fd;
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    /* SPI cihazı aç */
    fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0)
        exit_sys("open");

    /* SPI modu ayarla */
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
        exit_sys("ioctl mode");

    /* Bit sayısı ayarla */
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
        exit_sys("ioctl bits");

    /* Hız ayarla */
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
        exit_sys("ioctl speed");

    /* Transfer yapısı */
    uint8_t tx[] = {0x9F, 0x00, 0x00, 0x00};  /* JEDEC ID komutu */
    uint8_t rx[4] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 4,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
    };

    /* Transfer yap */
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0)
        exit_sys("SPI transfer");

    printf("Received: %02X %02X %02X %02X\n",
           rx[0], rx[1], rx[2], rx[3]);

    close(fd);
    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## 🔧 BBB Protokol Pinleri Özeti

| Protokol | Pin | Fonksiyon |
|----------|-----|-----------|
| **UART1** TX | P9_24 | Serial çıkış |
| **UART1** RX | P9_26 | Serial giriş |
| **I2C1** SDA | P9_18 | I2C data |
| **I2C1** SCL | P9_17 | I2C clock |
| **I2C2** SDA | P9_20 | I2C data |
| **I2C2** SCL | P9_19 | I2C clock |
| **SPI0** MOSI | P9_18 | SPI data out |
| **SPI0** MISO | P9_21 | SPI data in |
| **SPI0** CLK | P9_22 | SPI clock |
| **SPI0** CS | P9_17 | Chip select |

> [!WARNING]
> Bazı pinler (örn. P9_17, P9_18) birden fazla fonksiyon için multiplexed edilmiştir. Aynı anda hem I2C1 hem SPI0 kullanamazsınız.

---

## 🔧 Raspberry Pi Protokol Pinleri

| Protokol | Pin (BCM) | Pin (Board) | Fonksiyon |
|----------|-----------|-------------|-----------|
| **UART** TX | GPIO14 | 8 | Serial out |
| **UART** RX | GPIO15 | 10 | Serial in |
| **I2C1** SDA | GPIO2 | 3 | I2C data |
| **I2C1** SCL | GPIO3 | 5 | I2C clock |
| **SPI0** MOSI | GPIO10 | 19 | SPI data out |
| **SPI0** MISO | GPIO9 | 21 | SPI data in |
| **SPI0** CLK | GPIO11 | 23 | SPI clock |
| **SPI0** CE0 | GPIO8 | 24 | Chip select 0 |
| **SPI0** CE1 | GPIO7 | 26 | Chip select 1 |

---

## 🎯 Pratik Ödevler

1. USB-UART dönüştürücü ile PC ve BBB/RPi arasında iletişim kurun
2. DS3231 RTC modülü bağlayın ve saat bilgisini okuyun
3. `i2cdetect` ile bağlı I2C cihazlarını tarayın
4. DS3231'e saat yazın (saati ayarlayın)
5. Bir I2C sensörden (BME280, MPU6050, vb.) veri okuyun
6. SPI ile bir cihaza (SD kart, LCD, vb.) veri gönderin

---

## 📚 Kaynaklar

- [Linux I2C Documentation](https://www.kernel.org/doc/Documentation/i2c/)
- [DS3231 Datasheet](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf)
- [BeagleBone Black Pinout](https://docs.beagleboard.org/latest/boards/beaglebone/black/)
- [Raspberry Pi GPIO Pinout](https://pinout.xyz/)

---

*Sonraki ders: Buildroot ve Yocto*
