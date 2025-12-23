# GPIO Pinout Cheatsheet

## BeagleBone Black Pin Numbering

BBB'de **üç farklı numaralandırma sistemi** vardır:

| Sistem | Format | Örnek | Kullanım Alanı |
|--------|--------|-------|----------------|
| **Fiziksel** | P9_XX, P8_XX | P9_12 | Fiziksel bağlantı |
| **SoC** | GPIOX_Y | GPIO1_28 | Aygıt sürücü yazımı |
| **Sysfs** | gpioZ | gpio60 | User-mode programlama |

**Dönüşüm formülü:** `Z = X * 32 + Y`

Örnek: GPIO1_28 → Z = 1 * 32 + 28 = **gpio60**

---

## BeagleBone Black P9 Header

```
        P9 (SOL TARAF - Ethernet tarafı)
    ┌─────────────────┬─────────────────┐
  1 │      GND        │      GND        │ 2
  3 │     3.3V        │     3.3V        │ 4
  5 │      5V         │      5V         │ 6
  7 │    SYS_5V       │    SYS_5V       │ 8
  9 │   PWR_BUT       │   SYS_RESET     │ 10
 11 │  UART4_RX       │    GPIO60 ★     │ 12
 13 │  UART4_TX       │   EHRPWM1A      │ 14
 15 │    GPIO48 ★     │   EHRPWM1B      │ 16
 17 │   I2C1_SCL      │   I2C1_SDA      │ 18
 19 │   I2C2_SCL      │   I2C2_SDA      │ 20
 21 │  UART2_TX       │  UART2_RX       │ 22
 23 │    GPIO49 ★     │  UART1_TX       │ 24
 25 │   GPIO117 ★     │  UART1_RX       │ 26
 27 │   GPIO115 ★     │   SPI1_CS0      │ 28
 29 │   SPI1_D0       │   SPI1_D1       │ 30
 31 │  SPI1_SCLK      │    VDD_ADC      │ 32
 33 │     AIN4        │      GND        │ 34
 35 │     AIN6        │     AIN5        │ 36
 37 │     AIN2        │     AIN3        │ 38
 39 │     AIN0        │     AIN1        │ 40
 41 │   CLKOUT2       │    GPIO7 ★      │ 42
 43 │      GND        │      GND        │ 44
 45 │      GND        │      GND        │ 46
    └─────────────────┴─────────────────┘
    ★ = Yaygın kullanılan GPIO pinleri
```

---

## BeagleBone Black P8 Header

```
        P8 (SAĞ TARAF - USB tarafı)
    ┌─────────────────┬─────────────────┐
  1 │      GND        │      GND        │ 2
  3 │    GPIO38       │    GPIO39       │ 4
  5 │    GPIO34       │    GPIO35       │ 6
  7 │    GPIO66 ★     │    GPIO67 ★     │ 8
  9 │    GPIO69       │    GPIO68       │ 10
 11 │    GPIO45 ★     │    GPIO44 ★     │ 12
 13 │   EHRPWM2B      │    GPIO26       │ 14
 15 │    GPIO47       │    GPIO46       │ 16
 17 │    GPIO27       │    GPIO65       │ 18
 19 │   EHRPWM2A      │    GPIO63       │ 20
 21 │    GPIO62       │    GPIO37       │ 22
 23 │    GPIO36       │    GPIO33       │ 24
 25 │    GPIO32       │    GPIO61 ★     │ 26
 27 │    GPIO86       │    GPIO88       │ 28
 29 │    GPIO87       │    GPIO89       │ 30
 31 │    GPIO10       │    GPIO11       │ 32
 33 │    GPIO9        │    GPIO81       │ 34
 35 │    GPIO8        │    GPIO80       │ 36
 37 │    GPIO78       │    GPIO79       │ 38
 39 │    GPIO76       │    GPIO77       │ 40
 41 │    GPIO74       │    GPIO75       │ 42
 43 │    GPIO72       │    GPIO73       │ 44
 45 │    GPIO70       │    GPIO71       │ 46
    └─────────────────┴─────────────────┘
    ★ = Yaygın kullanılan GPIO pinleri
```

---

## Raspberry Pi 40-Pin Header

```
        Raspberry Pi GPIO Header (3B+/4/5)
    ┌──────────────────┬──────────────────┐
  1 │      3.3V        │       5V         │ 2
  3 │   GPIO2 (SDA1)   │       5V         │ 4
  5 │   GPIO3 (SCL1)   │      GND         │ 6
  7 │      GPIO4       │   GPIO14 (TXD)   │ 8
  9 │      GND         │   GPIO15 (RXD)   │ 10
 11 │     GPIO17 ★     │     GPIO18       │ 12
 13 │     GPIO27 ★     │      GND         │ 14
 15 │     GPIO22 ★     │     GPIO23       │ 16
 17 │      3.3V        │     GPIO24       │ 18
 19 │  GPIO10 (MOSI)   │      GND         │ 20
 21 │   GPIO9 (MISO)   │     GPIO25       │ 22
 23 │  GPIO11 (SCLK)   │  GPIO8 (CE0)     │ 24
 25 │      GND         │  GPIO7 (CE1)     │ 26
 27 │   GPIO0 (ID_SD)  │  GPIO1 (ID_SC)   │ 28
 29 │      GPIO5       │      GND         │ 30
 31 │      GPIO6       │     GPIO12       │ 32
 33 │     GPIO13       │      GND         │ 34
 35 │     GPIO19       │     GPIO16       │ 36
 37 │     GPIO26       │     GPIO20       │ 38
 39 │      GND         │     GPIO21       │ 40
    └──────────────────┴──────────────────┘
    ★ = Yaygın kullanılan GPIO pinleri
```

---

## GPIO Sysfs Kullanımı

### GPIO'yu Export Et

```bash
# GPIO60'ı kullanılabilir yap
echo 60 | sudo tee /sys/class/gpio/export

# Sonuç: /sys/class/gpio/gpio60 dizini oluşur
```

### Yön Ayarla (Input/Output)

```bash
# Output olarak ayarla
echo "out" | sudo tee /sys/class/gpio/gpio60/direction

# Input olarak ayarla
echo "in" | sudo tee /sys/class/gpio/gpio60/direction
```

### Değer Oku/Yaz

```bash
# LED yak (high)
echo 1 | sudo tee /sys/class/gpio/gpio60/value

# LED söndür (low)
echo 0 | sudo tee /sys/class/gpio/gpio60/value

# Button oku
cat /sys/class/gpio/gpio51/value
```

### GPIO'yu Unexport

```bash
echo 60 | sudo tee /sys/class/gpio/unexport
```

---

## LED Devresi (Output)

```
VCC (3.3V)
    │
    │
   [LED]
    │
   [R 470Ω]
    │
  GPIO Pin (örn: P9_12 / GPIO60)
```

## Button Devresi (Input)

```
VCC (3.3V)
    │
   [R 10kΩ] (Pull-up)
    │
    ├──── GPIO Pin (örn: P9_16 / GPIO51)
    │
  [Button]
    │
   GND
```

---

## Hızlı Referans Tablosu (BBB)

| Fiziksel | SoC | Sysfs | Kullanım |
|----------|-----|-------|----------|
| P9_12 | GPIO1_28 | gpio60 | LED (OUT) |
| P9_15 | GPIO1_16 | gpio48 | LED (OUT) |
| P9_16 | GPIO1_19 | gpio51 | Button (IN) |
| P9_23 | GPIO1_17 | gpio49 | Genel GPIO |
| P8_07 | GPIO2_2 | gpio66 | Timer/GPIO |
| P8_08 | GPIO2_3 | gpio67 | Timer/GPIO |
| P8_11 | GPIO1_13 | gpio45 | Genel GPIO |
| P8_12 | GPIO1_12 | gpio44 | Genel GPIO |

---

*Detaylı bilgi için: lecture_08_gpio/notes.md*
