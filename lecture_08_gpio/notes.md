# Ders 8: GPIO Programlama

> **Konu:** BeagleBone Black ve Raspberry Pi'da GPIO Kontrolü

---

## 📋 Bu Derste İşlenenler

- [x] GPIO temelleri (Input/Output/Interrupt)
- [x] Pin numaralandırma sistemleri
- [x] Sysfs ile GPIO kontrolü
- [x] C programı ile LED yakma
- [x] Button okuma
- [x] Kesme (interrupt) kullanımı

---

## 🔌 GPIO Nedir?

**GPIO (General Purpose Input/Output)** = Genel Amaçlı Giriş/Çıkış

Her GPIO pini üç modda kullanılabilir:

| Mod | Açıklama | Örnek Kullanım |
|-----|----------|----------------|
| **Output** | Pinden sinyal gönder | LED yakma, röle kontrolü |
| **Input** | Pinden sinyal oku | Button, sensör okuma |
| **Interrupt** | Sinyal değişiminde olay tetikle | Anlık tepki gereken uygulamalar |

---

## 📍 BBB Pin Numaralandırma

BeagleBone Black'te **3 farklı numaralandırma sistemi** vardır:

### 1. Fiziksel Numaralandırma
- Format: `P9_XX` veya `P8_XX`
- Breadboard bağlantısı için kullanılır
- Örnek: `P9_12` (soldaki soketin 12. pini)

### 2. SoC Numaralandırma
- Format: `GPIOX_Y`
- Aygıt sürücü yazımında kullanılır
- Örnek: `GPIO1_28`

### 3. Sysfs (Aygıt Sürücü) Numaralandırma
- Format: `gpioZ`
- User-mode programlamada kullanılır
- `Z = X * 32 + Y`
- Örnek: `GPIO1_28` → `gpio60`

### Yaygın Pinler

| Fiziksel | SoC | Sysfs | Kullanım |
|----------|-----|-------|----------|
| P9_12 | GPIO1_28 | gpio60 | LED (Output) |
| P9_15 | GPIO1_16 | gpio48 | LED (Output) |
| P9_16 | GPIO1_19 | gpio51 | Button (Input) |

---

## 🔧 Sysfs ile GPIO Kontrolü

### 1. GPIO'yu Aktifleştir (Export)

```bash
# gpio60'ı kullanılabilir yap
echo 60 | sudo tee /sys/class/gpio/export

# Kontrol: yeni dizin oluştu
ls /sys/class/gpio/gpio60/
# Çıktı: active_low  device  direction  edge  label  power  subsystem  uevent  value
```

### 2. Yön Ayarla (Direction)

```bash
# Output olarak ayarla (LED için)
echo "out" | sudo tee /sys/class/gpio/gpio60/direction

# Input olarak ayarla (Button için)
echo "in" | sudo tee /sys/class/gpio/gpio51/direction
```

### 3. Değer Oku/Yaz

```bash
# LED yak (HIGH)
echo 1 | sudo tee /sys/class/gpio/gpio60/value

# LED söndür (LOW)
echo 0 | sudo tee /sys/class/gpio/gpio60/value

# Button değerini oku
cat /sys/class/gpio/gpio51/value
# Basılı değil: 1, Basılı: 0
```

### 4. GPIO'yu Deaktifleştir (Unexport)

```bash
echo 60 | sudo tee /sys/class/gpio/unexport
```

---

## 💡 LED Devresi

```
GPIO Pin (P9_12, gpio60)
    │
   [R 470Ω]
    │
   [LED]
    │
   GND (P9_01)
```

**Alternatif (LED anota bağlı):**
```
VCC (3.3V)
    │
   [LED]
    │
   [R 470Ω]
    │
GPIO Pin
```

---

## 🔘 Button Devresi

```
VCC (3.3V, P9_03)
    │
   [R 10kΩ] (Pull-up direnç)
    │
    ├──── GPIO Pin (P9_16, gpio51)
    │
  [BUTTON]
    │
   GND (P9_01)
```

- Button basılı değil → GPIO'da `1` (HIGH)
- Button basılı → GPIO'da `0` (LOW)

---

## 💻 C ile LED Yakma/Söndürme

```c
/* led_blink.c - LED yakıp söndürme */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_PIN "60"
#define GPIO_PATH "/sys/class/gpio/gpio" GPIO_PIN

void write_file(const char *path, const char *value);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    
    /* GPIO'yu export et (eğer yapılmadıysa) */
    write_file("/sys/class/gpio/export", GPIO_PIN);
    usleep(100000);  /* 100ms bekle (sysfs hazırlanması için) */
    
    /* Yönü ayarla: output */
    write_file(GPIO_PATH "/direction", "out");
    
    /* Value dosyasını aç */
    fd = open(GPIO_PATH "/value", O_RDWR);
    if (fd < 0) {
        exit_sys("open value");
    }
    
    printf("LED blink basliyor... (Ctrl+C ile cikis)\n");
    
    /* LED'i 1 saniye aralıklarla yak/söndür */
    while (1) {
        if (write(fd, "1", 1) < 0) exit_sys("write");
        printf("LED: ON\n");
        sleep(1);
        
        lseek(fd, 0, SEEK_SET);
        
        if (write(fd, "0", 1) < 0) exit_sys("write");
        printf("LED: OFF\n");
        sleep(1);
        
        lseek(fd, 0, SEEK_SET);
    }
    
    close(fd);
    return 0;
}

void write_file(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        /* Export zaten yapılmış olabilir, hata değil */
        return;
    }
    write(fd, value, strlen(value));
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## 💻 C ile Button Okuma

```c
/* button_read.c - Button durumu okuma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_PIN "51"
#define GPIO_PATH "/sys/class/gpio/gpio" GPIO_PIN

void write_file(const char *path, const char *value);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    char buf[2];
    char prev = '1';
    
    /* GPIO'yu export et */
    write_file("/sys/class/gpio/export", GPIO_PIN);
    usleep(100000);
    
    /* Yönü ayarla: input */
    write_file(GPIO_PATH "/direction", "in");
    
    /* Value dosyasını aç */
    fd = open(GPIO_PATH "/value", O_RDONLY);
    if (fd < 0) {
        exit_sys("open value");
    }
    
    printf("Button okuma basliyor... (Ctrl+C ile cikis)\n");
    
    while (1) {
        lseek(fd, 0, SEEK_SET);
        
        if (read(fd, buf, 1) < 0) exit_sys("read");
        
        /* Sadece değişiklik olduğunda yazdır */
        if (buf[0] != prev) {
            if (buf[0] == '0') {
                printf("Button: PRESSED\n");
            } else {
                printf("Button: RELEASED\n");
            }
            prev = buf[0];
        }
        
        usleep(10000);  /* 10ms */
    }
    
    close(fd);
    return 0;
}

void write_file(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## ⚡ Kesme (Interrupt) Kullanımı

Polling yerine kesme kullanarak daha verimli çalışma:

### Edge Türleri

| Edge | Açıklama |
|------|----------|
| `rising` | 0→1 geçişinde tetikle |
| `falling` | 1→0 geçişinde tetikle |
| `both` | Her iki yönde tetikle |

### Sysfs ile Kesme Ayarı

```bash
# Input olarak ayarla
echo "in" | sudo tee /sys/class/gpio/gpio51/direction

# Falling edge kesme (button basıldığında)
echo "falling" | sudo tee /sys/class/gpio/gpio51/edge
```

### C ile Kesme Kullanımı (select/poll)

```c
/* gpio_interrupt.c - Kesme ile button okuma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define GPIO_PIN "51"
#define GPIO_PATH "/sys/class/gpio/gpio" GPIO_PIN

void write_file(const char *path, const char *value);
void exit_sys(const char *msg);

int main(void)
{
    int fd;
    fd_set exceptfds;
    char buf[2];
    
    /* GPIO export ve ayarla */
    write_file("/sys/class/gpio/export", GPIO_PIN);
    usleep(100000);
    write_file(GPIO_PATH "/direction", "in");
    write_file(GPIO_PATH "/edge", "falling");
    
    fd = open(GPIO_PATH "/value", O_RDONLY);
    if (fd < 0) exit_sys("open");
    
    /* İlk okuma (select öncesi gerekli) */
    if (read(fd, buf, 1) < 0) exit_sys("read");
    
    printf("Kesme bekleniyor...\n");
    
    while (1) {
        FD_ZERO(&exceptfds);
        FD_SET(fd, &exceptfds);
        
        /* select: kesme bekle */
        if (select(fd + 1, NULL, NULL, &exceptfds, NULL) < 0) {
            exit_sys("select");
        }
        
        if (FD_ISSET(fd, &exceptfds)) {
            lseek(fd, 0, SEEK_SET);
            if (read(fd, buf, 1) < 0) exit_sys("read");
            
            printf("Kesme tetiklendi! Button basildi.\n");
        }
    }
    
    close(fd);
    return 0;
}

void write_file(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, value, strlen(value));
    close(fd);
}

void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
```

---

## 🐍 Python ile GPIO

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
    except:
        pass

def main():
    # Export
    write_file("/sys/class/gpio/export", GPIO_PIN)
    time.sleep(0.1)
    
    # Direction
    write_file(f"{GPIO_PATH}/direction", "out")
    
    print("LED blink başlıyor...")
    
    with open(f"{GPIO_PATH}/value", 'w') as f:
        while True:
            f.write('1')
            f.flush()
            print("LED: ON")
            time.sleep(0.5)
            
            f.seek(0)
            f.write('0')
            f.flush()
            print("LED: OFF")
            time.sleep(0.5)
            
            f.seek(0)

if __name__ == "__main__":
    main()
```

---

## 🎯 Pratik Ödevler

1. LED'i 0.5 saniye aralıklarla yak/söndür
2. Button'a basıldığında LED yak, bırakıldığında söndür
3. Button'a basıldığında LED durumunu değiştir (toggle)
4. İki LED ile binary sayaç yap (0-3)
5. Kesme kullanarak button sayacı yap

---

## ⚠️ Dikkat Edilmesi Gerekenler

> [!CAUTION]
> GPIO pinleri 3.3V toleranslıdır! 5V uygulamayın, çipi yakarsınız.

> [!WARNING]
> LED'i dirençsiz bağlamayın, LED veya GPIO hasar görebilir.

> [!TIP]
> Pull-up/pull-down dirençler kullanarak kararlı sinyal alın.

---

*Detaylı pinout için: cheatsheets/gpio_pinout.md*
