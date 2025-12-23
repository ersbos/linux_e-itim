# Ders 1: Gömülü Linux'a Giriş

> **Tarih:** 2025-12-23  
> **Konu:** Gömülü Sistemler ve ARM Mimarisi

---

## 📋 Bu Derste İşlenenler

- [x] Gömülü sistem tanımı ve özellikleri
- [x] Gerçek zamanlı sistemler (Hard/Soft Real-Time)
- [x] ARM mimarisi (RISC vs CISC)
- [x] SBC'ler: Raspberry Pi, BeagleBone Black
- [x] Gömülü Linux vs Desktop Linux

---

## 📝 Gömülü Sistem Nedir?

**Gömülü sistemler (Embedded Systems)**, asıl amacı bilgisayar olmayan fakat bilgisayar devresi içeren sistemlerdir. Belirli bir işlevi yerine getirmek için optimize edilmiş donanım ve yazılım bileşenlerinden oluşurlar.

### Örnekler
- Elektronik tartılar
- Biyomedikal aygıtlar
- GPS cihazları
- Otomobil kontrol panelleri
- Akıllı ev sistemleri
- Turnike geçiş sistemleri

### Gömülü Sistemlerin Temel Özellikleri

| Özellik | Açıklama |
|---------|----------|
| **Özel Amaçlı** | Genel değil, belirli işlevleri gerçekleştirir |
| **Düşük Kapasite** | Masaüstü sistemlere göre daha az RAM/CPU |
| **Düşük Güç** | Batarya ile çalışabilir, taşınabilir |
| **Gerçek Zamanlı** | Belirli zaman diliminde yanıt verir |
| **Minimal UI** | Küçük LCD, düğmeler, tuş takımı |
| **Düşük Maliyet** | Seri üretim için uygun fiyatlı |

---

## ⏱️ Gerçek Zamanlı Sistemler (Real-Time Systems)

Gömülü sistemlerde işlemlerin sadece doğru sonuç vermesi değil, bu sonucu **belirli bir zaman dilimi içerisinde** vermesi kritiktir.

### Hard Real-Time (Sert Gerçek Zamanlı)
- Belirlenen sürenin (deadline) aşılması → **Sistem başarısız**
- Hayati tehlike veya maddi hasar yaratabilir
- **Örnekler:**
  - Hava yastığı tetikleme
  - ABS fren sistemleri
  - Kalp pili
  - Uçuş kontrol sistemleri

### Soft Real-Time (Yumuşak Gerçek Zamanlı)
- Sürenin aşılması → **Performans düşer** (ama sistem çalışmaya devam)
- Kullanıcı deneyimi etkilenir
- **Örnekler:**
  - Video oynatıcılar
  - Ağ paketlerinin işlenmesi
  - VoIP sistemleri
  - Oyun konsolları

---

## 🖥️ Gömülü Linux vs Desktop Linux

| Özellik | Desktop Linux | Gömülü Linux |
|---------|---------------|--------------|
| **Boyut** | GB seviyesi | MB seviyesi |
| **Boot Süresi** | 10-60 saniye | Milisaniye-saniye |
| **RAM** | 4-64 GB | 256MB-4GB |
| **Depolama** | SSD/HDD TB | Flash MB-GB |
| **UI** | Genellikle var | Opsiyonel/yok |
| **Paket Yönetimi** | Full (apt, yum) | Minimal/yok |
| **Kernel** | Generic | Özelleştirilmiş |

---

## 🔧 İşlem Birimleri

Gömülü sistemlerde üç temel işlem birimi kullanılır:

### 1. Mikrodenetleyiciler (MCU - Microcontroller Unit)
- **Tek çip bilgisayar**: İşlemci + RAM + Flash + IO
- Düşük güç, düşük maliyet
- Bare-metal veya RTOS ile programlanır
- **Örnekler:** Arduino, STM32, ESP32, PIC

### 2. Mikroişlemciler (MPU - Microprocessor Unit)
- Sadece işlem birimi (CPU)
- Harici RAM ve Flash gerektirir
- Tam işletim sistemi çalıştırabilir
- **Örnekler:** Intel x86, AMD, ARM Cortex-A

### 3. SoC (System on Chip)
- İşlemci + GPU + RAM + Çevre birimleri tek çipte
- Mobil cihazlar ve SBC'lerde kullanılır
- **Örnekler:** Broadcom BCM2711 (RPi4), TI AM335x (BBB)

---

## 💻 ARM Mimarisi

### RISC vs CISC

| Özellik | CISC (Intel x86) | RISC (ARM) |
|---------|------------------|------------|
| **Komut Sayısı** | Çok (1000+) | Az (100-200) |
| **Komut Uzunluğu** | Değişken (1-15 byte) | Sabit (4 byte) |
| **Yazmaç Sayısı** | Az (8-16) | Çok (32+) |
| **Güç Tüketimi** | Yüksek | Düşük |
| **Pipeline** | Karmaşık | Verimli |
| **Load/Store** | Memory-register ops | Sadece load/store |

### ARM İşlemci Profilleri

| Profil | Açıklama | Kullanım Alanı |
|--------|----------|----------------|
| **Cortex-A** (Application) | Yüksek performans | Linux, Android, masaüstü |
| **Cortex-R** (Realtime) | Düşük gecikme | Otomotiv, endüstriyel |
| **Cortex-M** (Microcontroller) | Düşük güç | IoT, sensörler, MCU |

### ARM Versiyonları

| Versiyon | Bit | Örnekler |
|----------|-----|----------|
| ARMv7-A | 32-bit | Cortex-A8 (BBB), Cortex-A7 |
| ARMv8-A | 64-bit | Cortex-A53 (RPi3), Cortex-A72 (RPi4) |
| ARMv8.2-A | 64-bit | Cortex-A76 (RPi5) |

---

## 📦 SBC (Single Board Computer) Karşılaştırması

### Raspberry Pi Serisi

| Model | SoC | CPU | RAM | GPIO | Fiyat |
|-------|-----|-----|-----|------|-------|
| **Pi 3B+** | BCM2837 | Cortex-A53 1.4GHz | 1GB | 40-pin | ~$35 |
| **Pi 4B** | BCM2711 | Cortex-A72 1.5GHz | 2-8GB | 40-pin | ~$55+ |
| **Pi 5** | BCM2712 | Cortex-A76 2.4GHz | 4-8GB | 40-pin | ~$60+ |

### BeagleBone Black

| Model | SoC | CPU | RAM | GPIO | Fiyat |
|-------|-----|-----|-----|------|-------|
| **BBB** | AM335x | Cortex-A8 1GHz | 512MB | 2x46-pin | ~$55 |
| **BBB Wireless** | AM335x | Cortex-A8 1GHz | 512MB | 2x46-pin | ~$70 |

### Hangi SBC Ne Zaman?

| Kullanım | Önerilen |
|----------|----------|
| Genel amaç, masaüstü | Raspberry Pi 4/5 |
| GPIO yoğun projeler | BeagleBone Black |
| Düşük maliyetli prototip | Raspberry Pi Zero |
| Endüstriyel uygulama | BeagleBone AI |
| AI/ML projeleri | Raspberry Pi 5 / Jetson |

---

## 💻 Temel Terminal Komutları

```bash
# Sistem bilgisi
uname -a              # Kernel ve sistem bilgisi
uname -r              # Kernel versiyonu
uname -m              # Makine tipi (armv7l, aarch64, x86_64)

# CPU bilgisi
cat /proc/cpuinfo     # CPU detayları
lscpu                 # CPU özeti

# Bellek bilgisi
free -h               # RAM kullanımı
cat /proc/meminfo     # Detaylı bellek bilgisi

# Disk bilgisi
df -h                 # Disk kullanımı
lsblk                 # Blok cihazlar

# Sistem yükü
uptime                # Çalışma süresi ve yük
top                   # Canlı işlem izleme
htop                  # Gelişmiş işlem izleme (kurulmalı)
```

---

## 🔧 Örnek Kod

### hello_embedded.c

```c
/**
 * @file hello_embedded.c
 * @brief İlk gömülü Linux programı
 * 
 * Derleme (host):
 *   gcc -o hello hello_embedded.c
 * 
 * Derleme (cross - BBB için):
 *   arm-linux-gnueabihf-gcc -o hello hello_embedded.c
 * 
 * Derleme (cross - RPi 64-bit için):
 *   aarch64-linux-gnu-gcc -o hello hello_embedded.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("Merhaba Gömülü Linux Dünyası!\n");
    printf("=====================================\n");
    
    // Hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    }
    
    // Kernel bilgisi
    printf("\n--- Sistem Bilgisi ---\n");
    system("uname -a");
    
    printf("\n--- CPU Bilgisi ---\n");
    system("cat /proc/cpuinfo | grep -E 'model name|Hardware|Revision' | head -3");
    
    printf("\n--- Bellek Bilgisi ---\n");
    system("free -h | head -2");
    
    return 0;
}
```

---

## ❓ Sorular / Tartışma Konuları

1. Neden gömülü sistemlerde C dili tercih edilir?
2. RTOS ile Linux arasındaki farklar nelerdir?
3. 32-bit vs 64-bit ARM sistemlerde pratik farklar nelerdir?

---

## 📚 Kaynaklar

- [Bootlin Embedded Linux Training](https://bootlin.com/training/)
- [The Linux Kernel Documentation](https://docs.kernel.org/)
- [ARM Developer Documentation](https://developer.arm.com/documentation)
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [BeagleBoard Documentation](https://docs.beagleboard.org/)

---

## 🎯 Ödev / Pratik

- [ ] Raspberry Pi veya BeagleBone Black'e Linux kur
- [ ] SSH ile bağlan ve sistem bilgilerini görüntüle
- [ ] `hello_embedded.c` programını derle ve çalıştır
- [ ] Cross-compilation dene (host'ta derle, target'ta çalıştır)

---

> [!NOTE]
> Bu ders gömülü Linux'un temellerini oluşturur. Sonraki derslerde boot süreci, kernel, ve aygıt sürücülerini inceleyeceğiz.

---

*Son güncelleme: 2025-12-23*
