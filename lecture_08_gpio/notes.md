# Ders 8: GPIO Programlama

> **Konu:** BeagleBone Black ve Raspberry Pi'da GPIO Kontrolü
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] GPIO temelleri ve kullanım amaçları
- [x] Pin numaralandırma sistemleri (Fiziksel, SoC, Sysfs)
- [x] BBB ve RPi header yapıları
- [x] Sysfs dosya sistemi ile GPIO kontrolü
- [x] C programlama ile LED yakma/söndürme
- [x] Button okuma (Polling yöntemi)
- [x] Kesme (Interrupt) mekanizması
- [x] select/poll fonksiyonları ile kesme
- [x] gpiod kütüphanesi

---

## 🔌 GPIO Nedir?

GPIO, **General Purpose Input/Output** (Genel Amaçlı Giriş/Çıkış) kısaltmasıdır. Gömülü sistemler mühendisi olarak GPIO pinleri ile dış dünyayla elektriksel iletişim kurabilirsiniz. Bu pinler üç temel amaçla kullanılmaktadır:

**OUT (Çıkış):** İlgili pine elektriksel işaret uygulamak anlamına gelir. Pin lojik 1 seviyesine (3.3V) çekildiğinde başka bir elektronik devreyi sürebilir. LED yakma, röle kontrolü, motor sürme gibi işlemler bu modda yapılır.

**IN (Giriş):** Dış dünyadaki bir birimin ilgili uca uyguladığı elektriksel işaretin okunması anlamına gelir. Düğme okuma, sensör değeri alma, dijital sinyal algılama gibi işlemler bu modda gerçekleştirilir.

**Interrupt (Kesme):** GPIO pinlerinin diğer bir özelliği kesme oluşturabilmesidir. Bu pinlere bağlanan aygıtların kesme sinyali üretmesini sağlayabilir, böylece sürekli pin durumunu kontrol etmek (polling) yerine olay tetiklendiğinde haberdar olabilirsiniz.

---

## 📍 BBB Pin Numaralandırma Sistemleri

BeagleBone Black'te GPIO pinlerinin numaralandırılması kafa karıştırıcı olabilir çünkü **üç farklı numaralandırma sistemi** kullanılmaktadır. Bu sistemlerin her birinin kullanıldığı bağlam farklıdır.

### BBB Header Yapısı

BBB'de kartın Ethernet ve 5V girişinin bulunduğu ucu karşı tarafa bakacak şekilde konumlandırıldığında, sol taraftaki sokete **P9**, sağ taraftaki sokete ise **P8** denilmektedir. Her sokette ikişer sütun halinde toplam 46 pin bulunmaktadır.

```
        P9 (Sol)                           P8 (Sağ)
┌─────────────┬──────────────┐   ┌─────────────┬──────────────┐
│ P9_01  GND  │ P9_02  GND   │   │ P8_01  GND  │ P8_02  GND   │
│ P9_03  3.3V │ P9_04  3.3V  │   │ P8_03       │ P8_04        │
│ P9_05  5V   │ P9_06  5V    │   │ P8_05       │ P8_06        │
│     ...     │     ...      │   │     ...     │     ...      │
│ P9_45  GND  │ P9_46  GND   │   │ P8_45       │ P8_46        │
└─────────────┴──────────────┘   └─────────────┴──────────────┘
```

### 1. Fiziksel Numaralandırma

Fiziksel numaralandırma, sokete göre yapılan numaralandırmadır ve breadboard bağlantılarında kullanılır. Format **P9_XX** veya **P8_XX** biçimindedir. P9 ve P8 soketi, XX ise pin numarasını belirtir. Sol sütun tek, sağ sütun çift numaralarla numaralandırılmıştır:

```
1   2
3   4
5   6
...
45  46
```

Örneğin **P9_12** soldaki soketin 12 numaralı pini, **P8_07** sağdaki soketin 7 numaralı pini anlamına gelmektedir.

### 2. SoC Numaralandırması

SoC numaralandırması, AM335x SoC tasarımında kullanılan içsel numaralandırma sistemidir. AM335x SoC içerisinde GPIO uçlarını kontrol eden **4 adet denetleyici** vardır ve her denetleyici 32 çıkışlıdır. Bu denetleyicilerin uçları P9 ve P8 soketlerindeki pinlere karışık biçimde dağıtılmıştır.

Format **GPIOX_Y** biçimindedir. X denetleyici numarasını (0-3), Y ise o denetleyicinin hangi ucunun pine bağlandığını (0-31) belirtir. Örneğin **GPIO1_28** birinci denetleyicinin 28 numaralı ucunu temsil eder.

Fiziksel numaradan hareketle SoC numaralandırması doğrudan hesaplanamaz; bu eşleşmeler kart tasarımına göre belirlenmiştir.

### 3. Sysfs (Aygıt Sürücü) Numaralandırması

Sysfs numaralandırması, Linux aygıt sürücülerinin ve kullanıcı modu programlarının kullandığı numaralandırma sistemidir. Bu sistemde her pin **gpioZ** biçiminde temsil edilir. Z değeri şu formülle hesaplanır:

```
Z = X * 32 + Y
```

Örneğin:
- GPIO1_28 → gpio60 (1 × 32 + 28 = 60)
- GPIO1_16 → gpio48 (1 × 32 + 16 = 48)
- GPIO3_21 → gpio117 (3 × 32 + 21 = 117)

### Hangi Sistem Nerede Kullanılır?

| Bağlam | Tercih Edilen Sistem |
|--------|---------------------|
| Fiziksel devre bağlantısı | Fiziksel (P9_12, P8_07) |
| Aygıt sürücü yazımı | SoC (GPIO1_28) |
| Kullanıcı modu programlama | Sysfs (gpio60) |

### Yaygın Kullanılan Pinler

| Fiziksel | SoC | Sysfs | Açıklama |
|----------|-----|-------|----------|
| P9_01 | - | - | GND |
| P9_03 | - | - | 3.3V |
| P9_12 | GPIO1_28 | gpio60 | LED için ideali |
| P9_15 | GPIO1_16 | gpio48 | LED alternatif |
| P9_16 | GPIO1_19 | gpio51 | Button için ideal |
| P9_23 | GPIO1_17 | gpio49 | GPIO |
| P9_17 | - | - | I2C1_SCL / SPI0_CS0 |
| P9_18 | - | - | I2C1_SDA / SPI0_MISO |
| P9_19 | - | - | I2C2_SCL |
| P9_20 | - | - | I2C2_SDA |
| P9_24 | - | - | UART1_TXD |
| P9_26 | - | - | UART1_RXD |

---

## 🍓 Raspberry Pi Pin Numaralandırma

Raspberry Pi'da 40 pinlik GPIO header bulunmaktadır. İki numaralandırma sistemi vardır:

**Board Numaralandırma:** Fiziksel pin numaraları (1-40)
**BCM Numaralandırma:** Broadcom SoC GPIO numaraları

```
   3.3V  (1) (2)  5V
  GPIO2  (3) (4)  5V
  GPIO3  (5) (6)  GND
  GPIO4  (7) (8)  GPIO14
   GND   (9) (10) GPIO15
 GPIO17 (11) (12) GPIO18
 GPIO27 (13) (14) GND
 GPIO22 (15) (16) GPIO23
   3.3V (17) (18) GPIO24
 GPIO10 (19) (20) GND
  GPIO9 (21) (22) GPIO25
 GPIO11 (23) (24) GPIO8
   GND  (25) (26) GPIO7
```

---

## 🔧 Test Devreleri

GPIO pinleriyle OUT ve IN işlemlerinin testi için basit devreler kullanılır.

### LED (OUT) Devresi

OUT testi için en basit devre bir LED'in yakılıp söndürülmesidir:

```
GPIO pin (P9_12, gpio60)
    │    
    ├───[R 470Ω]───|>|───⏚ GND
                   A K
```

Bu devrede LED'in kısa ucu (katot) toprağa, uzun ucu (anot) önce bir dirence sonra GPIO pinine bağlanmıştır. Direnç olarak 220Ω-470Ω arası küçük dirençler kullanabilirsiniz. **Asla dirençsiz bağlamayın** - bu LED'e veya GPIO pinine zarar verebilir.

### Button (IN) Devresi

IN testi için tipik devre pull-up dirençli bir düğme devresidir:

```
    +3.3V (P9_03)
       │
      [10kΩ]   ← Pull-up direnç
       │
       ├────── GPIO pin (P9_16, gpio51)
       │
    [Buton]
       │
      ⏚ GND (P9_01)
```

Bu devrede düğmenin bir ucu toprağa, diğer ucu GPIO pinine bağlıdır. GPIO pinine bağlı uç aynı zamanda 10kΩ gibi büyük bir dirençle 3.3V'a bağlanmıştır. Bu sayede:
- Düğmeye **basılmamışsa**: pin 3.3V'a çekilir → **lojik 1** (HIGH)
- Düğmeye **basılmışsa**: pin toprağa kısa devre olur → **lojik 0** (LOW)

---

## 🔧 GPIO Programlama Seçenekleri

Linux'ta GPIO programlaması için çeşitli yöntemler vardır:

1. **sysfs dosya sistemi:** `/sys/class/gpio` dizinindeki dosyalar üzerinden programlama
2. **/dev/gpiochipX aygıt sürücüsü:** Karakter aygıt sürücüsü üzerinden ioctl komutlarıyla
3. **gpiod kütüphanesi:** Modern, yüksek seviyeli kütüphane (gpiochipX kullanır)
4. **Bellek tabanlı IO:** Aygıt sürücü yazarak doğrudan SoC denetleyicisine erişim

Biz bu derste sysfs yöntemini detaylı inceleyeceğiz, ardından gpiod kütüphanesine de değineceğiz.

---

## 📁 Sysfs ile GPIO Kontrolü

Linux çekirdeğindeki GPIO aygıt sürücüsü, dosya tabanlı erişim için `/sys/class/gpio` dizinini kullanmaktadır. Bu dizinde şu girişler bulunur:

```bash
$ ls /sys/class/gpio/
export   gpio114  gpio15   gpio3    gpio48   gpio60   gpiochip0
unexport gpio115  gpio20   gpio30   gpio49   gpio61   gpiochip32
...
```

Her gpio dizini içinde önemli dosyalar vardır:

```bash
$ ls /sys/class/gpio/gpio60/
active_low  device  direction  edge  label  power  subsystem  uevent  value
```

### Önemli Dosyalar

| Dosya | Amaç |
|-------|------|
| **direction** | Pin yönünü ayarlar: "in" veya "out" |
| **value** | Pin değerini okur/yazar: "0" veya "1" |
| **edge** | Kesme tetikleme: "rising", "falling", "both" |
| **active_low** | Değer tersine çevirme |

### 1. Direction Ayarlama

```bash
# Output olarak ayarla (LED için)
echo "out" | sudo tee /sys/class/gpio/gpio60/direction

# Input olarak ayarla (Button için)
echo "in" | sudo tee /sys/class/gpio/gpio51/direction
```

### 2. Value Okuma/Yazma

```bash
# LED yak (pin'e 3.3V gönder)
echo "1" | sudo tee /sys/class/gpio/gpio60/value

# LED söndür (pin'i 0V'a çek)
echo "0" | sudo tee /sys/class/gpio/gpio60/value

# Button değerini oku
cat /sys/class/gpio/gpio51/value
# Çıktı: basılı değilse "1", basılıysa "0"
```

### 3. Export ve Unexport

Bir GPIO pini kullanmadan önce export edilmelidir. Bazı sistemlerde pinler zaten export edilmiş olabilir:

```bash
# Pin'i export et
echo 60 | sudo tee /sys/class/gpio/export

# Pin'i unexport et (serbest bırak)
echo 60 | sudo tee /sys/class/gpio/unexport
```

---

## 💻 C ile LED Yakma/Söndürme

Yukarıdaki sysfs dosyalarını C programından da açıp işlem yapabiliriz:

```c
/* led-blink.c - LED yakıp söndürme */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void write_file(const char *path, const char *cmd);
void exit_sys(const char *msg);

int main(void)
{
    int fd;

    /* Direction'ı OUTPUT olarak ayarla */
    write_file("/sys/class/gpio/gpio60/direction", "out");

    /* Value dosyasını aç */
    if ((fd = open("/sys/class/gpio/gpio60/value", O_RDWR)) == -1)
        exit_sys("open");

    printf("LED blink başlıyor... (Ctrl+C ile çıkış)\n");

    /* Sonsuz döngüde yak/söndür */
    for (;;) {
        lseek(fd, 0, SEEK_SET);  /* Dosya göstericisini başa al */
        if (write(fd, "1", 1) == -1)
            exit_sys("write");
        printf("LED: ON\n");
        sleep(1);

        lseek(fd, 0, SEEK_SET);
        if (write(fd, "0", 1) == -1)
            exit_sys("write");
        printf("LED: OFF\n");
        sleep(1);
    }

    close(fd);
    return 0;
}

void write_file(const char *path, const char *cmd)
{
    int fd;

    if ((fd = open(path, O_WRONLY)) == -1)
        exit_sys("open");

    if (write(fd, cmd, strlen(cmd)) == -1)
        exit_sys("write");

    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

> [!NOTE]
> Yazma işlemlerinde dosya göstericisi ilerletildiği için her yazmadan önce `lseek(fd, 0, SEEK_SET)` ile dosya göstericisini başa çekmeliyiz. Ancak bu GPIO aygıt sürücüsü dosya göstericisini dikkate almadan çalışabilmektedir.

---

## 💻 C ile Button Okuma (Polling)

Polling yöntemiyle sürekli döngü içinde pini okuyarak düğme durumunu takip edebiliriz:

```c
/* button-read.c - Button durumu okuma (polling) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void write_file(const char *path, const char *cmd);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    char status, prev_status = '1';

    /* Direction'ı INPUT olarak ayarla */
    write_file("/sys/class/gpio/gpio51/direction", "in");

    /* Value dosyasını aç */
    if ((fd = open("/sys/class/gpio/gpio51/value", O_RDONLY)) == -1)
        exit_sys("open");

    printf("Button okuma başlıyor... (Ctrl+C ile çıkış)\n");

    for (;;) {
        lseek(fd, 0, SEEK_SET);
        if (read(fd, &status, 1) == -1)
            exit_sys("read");

        /* Sadece değişiklik olduğunda yazdır */
        if (status != prev_status) {
            if (status == '0')
                printf("Button: PRESSED\n");
            else
                printf("Button: RELEASED\n");
            prev_status = status;
        }
        usleep(10000);  /* 10ms bekleme */
    }

    close(fd);
    return 0;
}

void write_file(const char *path, const char *cmd)
{
    int fd;
    if ((fd = open(path, O_WRONLY)) == -1)
        exit_sys("open");
    if (write(fd, cmd, strlen(cmd)) == -1)
        exit_sys("write");
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## ⚡ Kesme (Interrupt) Mekanizması

Polling yöntemi CPU zamanı açısından verimsizdir çünkü sürekli pini kontrol eder. Bunun yerine **kesme mekanizması** kullanarak pin değeri değiştiğinde haberdar olabiliriz.

### Edge Konfigürasyonu

Kesme kullanmadan önce pini IN olarak ayarlamalı ve **edge** dosyasına tetikleme türünü yazmalıyız:

| Edge Değeri | Anlamı |
|-------------|--------|
| `rising` | 0→1 geçişinde kesme oluştur |
| `falling` | 1→0 geçişinde kesme oluştur |
| `both` | Her iki yönde kesme oluştur |

```bash
# Button için: düğmeye basıldığında (1→0)
echo "in" | sudo tee /sys/class/gpio/gpio51/direction
echo "falling" | sudo tee /sys/class/gpio/gpio51/edge
```

### select/poll Fonksiyonları

sysfs ile GPIO kesme işlemlerini ele almak için **multiplexed I/O** fonksiyonları olan `select`, `poll` veya `epoll` kullanılmalıdır. GPIO aygıt sürücüsü kesme olaylarını **exceptional** durum olarak raporlar.

#### select ile Kesme Örneği

```c
/* gpio-interrupt.c - select ile kesme */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

void write_file(const char *path, const char *cmd);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    fd_set exceptfds;
    char buf;

    /* GPIO konfigürasyonu */
    write_file("/sys/class/gpio/gpio51/direction", "in");
    write_file("/sys/class/gpio/gpio51/edge", "falling");

    /* Value dosyasını aç */
    if ((fd = open("/sys/class/gpio/gpio51/value", O_RDONLY)) == -1)
        exit_sys("open");

    /* İlk okuma (select öncesi gerekli) */
    if (read(fd, &buf, 1) == -1)
        exit_sys("read");

    printf("Kesme bekleniyor... (Ctrl+C ile çıkış)\n");

    for (;;) {
        FD_ZERO(&exceptfds);
        FD_SET(fd, &exceptfds);

        /* select: kesme bekle (exceptfds kullanılır) */
        if (select(fd + 1, NULL, NULL, &exceptfds, NULL) == -1)
            exit_sys("select");

        if (FD_ISSET(fd, &exceptfds)) {
            lseek(fd, 0, SEEK_SET);
            if (read(fd, &buf, 1) == -1)
                exit_sys("read");
            printf("Kesme oluştu! Button basıldı.\n");
        }
    }

    close(fd);
    return 0;
}

void write_file(const char *path, const char *cmd)
{
    int fd;
    if ((fd = open(path, O_WRONLY)) == -1)
        exit_sys("open");
    if (write(fd, cmd, strlen(cmd)) == -1)
        exit_sys("write");
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

> [!IMPORTANT]
> select blokeyi çözdüğünde **mutlaka value dosyası okunmalıdır**. Aksi halde sonraki select çağrıları bloke olmaz. Ayrıca döngüye girmeden önce bir kez read yapılmalıdır.

#### poll ile Kesme Örneği

```c
/* gpio-interrupt-poll.c - poll ile kesme */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

void write_file(const char *path, const char *cmd);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    struct pollfd pollfds[1];
    char buf;

    write_file("/sys/class/gpio/gpio51/direction", "in");
    write_file("/sys/class/gpio/gpio51/edge", "falling");

    if ((fd = open("/sys/class/gpio/gpio51/value", O_RDONLY)) == -1)
        exit_sys("open");

    if (read(fd, &buf, 1) == -1)
        exit_sys("read");

    pollfds[0].fd = fd;
    pollfds[0].events = POLLPRI;  /* GPIO kesme = POLLPRI */

    printf("Kesme bekleniyor (poll)...\n");

    for (;;) {
        if (poll(pollfds, 1, -1) == -1)
            exit_sys("poll");

        if (pollfds[0].revents & POLLPRI) {
            lseek(fd, 0, SEEK_SET);
            if (read(fd, &buf, 1) == -1)
                exit_sys("read");
            printf("Kesme oluştu!\n");
        }
    }

    close(fd);
    return 0;
}

void write_file(const char *path, const char *cmd)
{
    int fd;
    if ((fd = open(path, O_WRONLY)) == -1) exit_sys("open");
    if (write(fd, cmd, strlen(cmd)) == -1) exit_sys("write");
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## 📚 gpiod Kütüphanesi

**gpiod** kütüphanesi, GPIO işlemleri için modern ve önerilen yöntemdir. sysfs yerine doğrudan `/dev/gpiochipN` aygıt sürücülerini kullanır.

### Kurulum

```bash
# Kütüphane kurulumu
sudo apt install libgpiod-dev

# Komut satırı araçları
sudo apt install gpiod
```

### Komut Satırı Araçları

```bash
# GPIO chip'lerini listele
gpiodetect

# Belirli chip'in hatlarını göster
gpioinfo gpiochip1

# Pin'i output olarak ayarla ve değer gönder
gpioset --mode=wait gpiochip1 28=1

# Pin değerini oku
gpioget gpiochip1 19

# Kesme izle
gpiomon --falling-edge gpiochip1 19
```

### C ile gpiod Kullanımı

```c
/* gpiod-led.c - gpiod ile LED yakma */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gpiod.h>

void exit_sys(const char *msg);

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int value = 1;

    /* Chip aç */
    if ((chip = gpiod_chip_open_by_name("gpiochip1")) == NULL)
        exit_sys("gpiod_chip_open_by_name");

    /* Hat (line) al - GPIO1_28 = 28 */
    if ((line = gpiod_chip_get_line(chip, 28)) == NULL)
        exit_sys("gpiod_chip_get_line");

    /* Output olarak ayarla, başlangıç değeri 0 */
    if (gpiod_line_request_output(line, "led-app", 0) == -1)
        exit_sys("gpiod_line_request_output");

    printf("LED blink başlıyor...\n");

    for (int i = 0; i < 20; i++) {
        if (gpiod_line_set_value(line, value) == -1)
            exit_sys("gpiod_line_set_value");
        value = !value;
        sleep(1);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

**Derleme:**
```bash
gcc -o gpiod-led gpiod-led.c -lgpiod
```

---

## 🐍 Python ile GPIO

Python'da sysfs veya RPi.GPIO/gpiozero kütüphaneleri kullanılabilir:

```python
#!/usr/bin/env python3
"""led_blink.py - Python ile LED yakma"""

import time

GPIO_PIN = "60"
GPIO_PATH = f"/sys/class/gpio/gpio{GPIO_PIN}"

def write_file(path, value):
    try:
        with open(path, 'w') as f:
            f.write(value)
    except Exception as e:
        print(f"Error: {e}")

def main():
    # Direction ayarla
    write_file(f"{GPIO_PATH}/direction", "out")

    print("LED blink başlıyor...")

    with open(f"{GPIO_PATH}/value", 'w') as f:
        while True:
            f.write('1')
            f.flush()
            f.seek(0)
            print("LED: ON")
            time.sleep(0.5)

            f.write('0')
            f.flush()
            f.seek(0)
            print("LED: OFF")
            time.sleep(0.5)

if __name__ == "__main__":
    main()
```

---

## ⚠️ Önemli Uyarılar

> [!CAUTION]
> GPIO pinleri **3.3V toleranslıdır**! 5V uygulamayın, SoC'u kalıcı olarak hasarlayabilirsiniz.

> [!WARNING]
> LED'i **dirençsiz** bağlamayın - LED veya GPIO pini hasar görebilir. 220Ω-470Ω arası direnç kullanın.

> [!TIP]
> Button devrelerinde **pull-up veya pull-down direnç** kullanarak kararlı sinyal alın. Aksi halde "floating" durumu oluşur ve rastgele değerler okunabilir.

---

## 🎯 Pratik Ödevler

1. LED'i 0.5 saniye aralıklarla yak/söndür
2. Button'a basıldığında LED yak, bırakıldığında söndür
3. Button ile LED toggle (her basışta durum değişsin)
4. Kesme kullanarak button sayacı yap
5. İki LED ile binary sayaç (0-3)
6. gpiod kütüphanesiyle aynı örnekleri tekrarla

---

## 📚 Kaynaklar

- [BeagleBone Black GPIO Documentation](https://docs.beagleboard.org/)
- [libgpiod Documentation](https://libgpiod.readthedocs.io/)
- [Raspberry Pi GPIO](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)

---

*Sonraki ders: İletişim Protokolleri (I2C, SPI, UART)*
