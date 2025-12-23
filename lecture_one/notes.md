# Ders 1: Gömülü Linux'a Giriş

> **Konu:** Gömülü Sistemler, ARM Mimarisi, SBC'ler
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Gömülü sistem tanımı ve özellikleri
- [x] İşlem birimleri (MCU, MPU, SoC, SoM, SBC)
- [x] RISC vs CISC mimarisi (detaylı karşılaştırma)
- [x] ARM tarihçesi ve terminoloji
- [x] ARM profilleri ve versiyonları
- [x] Gerçek zamanlı sistemler (Hard/Soft)
- [x] SBC'ler: Raspberry Pi, BeagleBone Black
- [x] Gömülü Linux vs Desktop Linux

---

## 📝 Gömülü Sistem Nedir?

Asıl amacı bilgisayar olmayan fakat bilgisayar devresi içeren sistemlere genel olarak **gömülü sistemler (embedded systems)** denilmektedir. Yani gömülü sistemler, başka amaçları gerçekleştirmek için tasarlanmış olan aygıtların içerisindeki bilgisayar sistemleridir. Bu sistemler belirli işlevleri yerine getirmek için optimize edilmiş donanım ve yazılım bileşenlerinden oluşurlar.

Gömülü sistemler günümüzde hayatımızın her alanında karşımıza çıkmaktadır. Elektronik tartılar, biyomedikal aygıtlar, GPS cihazları, kapı güvenlik sistemleri, otomobillerdeki kontrol panelleri, akıllı ev sistemleri, turnike geçiş sistemleri (validatörler), müzik kutuları ve daha pek çok cihaz birer gömülü sistemdir. Bu sistemlerde en çok kullanılan programlama dili C'dir, ancak genel amaçlı işletim sistemlerinin yüklenebildiği SBC'lerde (Single Board Computer) Python, C++ gibi diğer programlama dilleri de yaygın olarak kullanılmaktadır.

### Gömülü Sistemlerin Temel Özellikleri

**Özel Amaçlı Tasarım:** Gömülü sistemler genel değil, belirli bir amaca yönelik işlemleri gerçekleştirmek üzere tasarlanmıştır. Bu sistemlerdeki yazılımlar da genel amaçlı değil, belirli bir görevi yerine getirmeye odaklanmaktadır. Örneğin bir çamaşır makinesi kontrol kartı yalnızca çamaşır yıkama programlarını yönetir, başka bir iş yapmaz.

**Düşük Bilgi İşlem Kapasitesi:** Gömülü sistemlerde kullanılan işlemciler, genel amaçlı masaüstü işlemcilerden genellikle daha yavaş olma eğilimindedir. Bellek miktarları (RAM ve Flash) genel amaçlı bilgisayar sistemlerine göre oldukça düşüktür. Bu durum maliyetleri de düşürmektedir.

**Düşük Güç Tüketimi:** Gömülü sistemler genellikle daha düşük güç harcamaktadır. Bu özellik, bataryalarla beslenebilmelerini ve fiziksel taşınabilirliklerini artırmaktadır. Ancak güç tüketimi her zaman kritik olmayabilir; örneğin bir araba kantarında gömülü sistemin harcadığı güç önemsizdir çünkü sistem zaten büyük bir güç kaynağına sahiptir.

**Gerçek Zamanlı İşleme:** Gömülü sistemlerin önemli bir bölümü gerçek zamanlı (real-time) olaylarla ilişkilidir. Bu sistemler dış dünyadaki değişimlere karşı belirli bir zaman dilimi içinde yanıt vermek zorundadır. Örneğin bir kalp pili ritim bozukluğunu algıladığında anında müdahale etmelidir.

**Minimal Kullanıcı Arayüzü:** Gömülü sistemlerin bazılarında hiç giriş/çıkış birimi olmayabilir, bazılarında ise yalnızca düğmeler, küçük tuş takımları veya basit LCD ekranlar bulunur. Klavye, fare ve büyük monitör gibi gelişmiş arayüz birimleri genellikle kullanılmaz.

**Düşük Maliyet:** Gömülü sistemlerdeki donanım birimleri genel amaçlı bilgisayarlara kıyasla çok daha ucuzdur. Bu özellik özellikle seri üretim için büyük avantaj sağlar.

---

## 🔧 İşlem Birimleri

Gömülü sistemlerde farklı karmaşıklık ve yeteneklerde çeşitli işlem birimleri kullanılmaktadır. Bu birimleri anlamak, doğru sistemi seçmek için kritik öneme sahiptir.

### Mikrodenetleyiciler (MCU - Microcontroller Unit)

Mikrodenetleyiciler, tek bir çip içerisine yerleştirilmiş komple bir bilgisayar sistemi gibi düşünülebilir. Tipik bir mikrodenetleyicide bir işlemci (processor), kendi içerisinde RAM ve Flash EPROM bellek, dış dünya ile haberleşmek için kullanılan giriş/çıkış (I/O) birimleri ve çeşitli çevre birimleri (peripherals) bulunmaktadır.

Mikrodenetleyicilerin işlem kapasiteleri ve içerdikleri bellek miktarları düşük olma eğilimindedir ani bu özellik onları ucuz ve az güç tüketen bileşenler haline getirir. Gömülü sistemlerde en yaygın kullanılan işlem birimi mikrodenetleyicilerdir. Arduino, STM32, ESP32, PIC, AVR gibi popüler platformlar mikrodenetleyici tabanlıdır. Bu sistemler genellikle işletim sistemi olmadan "bare-metal" olarak veya gerçek zamanlı işletim sistemleri (RTOS) ile programlanır.

### Mikroişlemciler (MPU - Microprocessor Unit)

Mikroişlemciler, bilgisayar sisteminde aritmetik, mantıksal, bitsel işlemler ve karşılaştırma gibi bilgi işlem faaliyetlerini gerçekleştiren ana birimdir. Mikroişlemciler entegre devre biçiminde üretilmiş olup kavramsal olarak CPU (Central Processing Unit) olarak da adlandırılır.

Mikrodenetleyicilerden farklı olarak, mikroişlemciler yalnızca işlem birimidir; harici RAM, ROM ve çevre birimlerine ihtiyaç duyarlar. Bu nedenle mikrodenetleyicilere kıyasla daha karmaşık devre tasarımı gerektirirler. Ancak çok daha yüksek işlem kapasitesine sahiptirler ve tam teşekküllü işletim sistemlerini çalıştırabilirler. Intel x86, AMD ve ARM Cortex-A serisi mikroişlemcilere örnektir.

### SoC (System on Chip)

Bazı üreticiler, ayrı birimler olarak tasarlanmış mikroişlemcileri, RAM'leri, ROM'ları ve diğer çevre birimlerini tek bir entegre devrenin içerisine sıkıştırmaktadır. Bu entegre devrelere SoC (System on Chip) denilmektedir.

SoC kavramı mikrodenetleyicilere benzer görünse de aslında onlardan farklıdır. SoC'lar içerisindeki işlemcilerin ve belleklerin kapasiteleri çok daha yüksektir. Bunlar özel amaçlı üretilirler ve devrelerde kullanılmaları mikrodenetleyiciler kadar kolay değildir. En büyük avantajları az yer kaplamalarıdır.

Raspberry Pi kitlerinde Broadcom firmasının BCM2835, BCM2836, BCM2837, BCM2711 ve BCM2712 numaralı SoC çipleri kullanılmaktadır. BeagleBone Black'te ise Texas Instruments'ın AM335x SoC'u bulunmaktadır. SoC'ların RAM içermesi zorunlu değildir; Raspberry Pi 1, 2, 3 modellerindeki SoC'lar RAM içerirken, Pi 4 ve 5 modellerinde RAM ayrı bir çip olarak bulunmaktadır.

### SoM (System on Module)

SoM kavramı, bir işlemci ve onunla ilişkili bazı birimlerin monte edildiği küçük kartları belirtmek için kullanılmaktadır. SoM'lar SoC'ları içerebilir ancak bunun yanında ek bileşenler de barındırır. Örneğin bir SoM, işlemci, RAM, I/O denetleyicisi ve güç yönetim birimi içeren bir kart olabilir.

Raspberry Pi Pico ve Raspberry Pi Compute Module, SBC'den ziyade birer SoM olarak ele alınabilir. SoM kavramını SoC ile SBC arasında bir yere konumlandırabilirsiniz.

### SBC (Single Board Computer)

Küçük bir kit (baskılı devre) üzerine monte edilmiş bilgisayarlara SBC (Single Board Computer) denilmektedir. Genellikle bu kitlerde SoC'lar, RAM'ler, çevre birimleri ve giriş/çıkış işlemleri için uçlar bulunmaktadır. Raspberry Pi'lar ve BeagleBoard'lar SBC'lere güzel örneklerdir.

SBC'ler, klavye, fare ve monitör takılarak bir masaüstü bilgisayar gibi kullanılabilir. Bu özellik sayesinde Linux başta olmak üzere Android ve hatta Windows gibi işletim sistemleri yüklenebilmektedir. Biz bu kursta SBC'ler üzerinde gömülü Linux geliştirme yapacağız.

---

## 💻 RISC vs CISC Mimarisi

Mikroişlemciler tasarım mimarilerine göre iki ana kategoriye ayrılır: CISC (Complex Instruction Set Computers) ve RISC (Reduced Instruction Set Computers). Intel firmasının x86 ailesi CISC tasarımına örnek gösterilirken, ARM, MIPS, PowerPC ve RISC-V işlemcileri tipik RISC tasarımlarıdır.

Her ne kadar CISC ve RISC isimleri komut kümesiyle ilgili olarak verilmişse de, bu iki mimari başka açılardan da önemli farklılıklar göstermektedir. Önceleri işlemcilerin çok sayıda makine komutuna sahip olması avantaj olarak görülüyordu. Ancak zamanla bunun dezavantaj oluşturduğu anlaşıldı ve modern işlemci tasarımlarında RISC mimarisi baskın hale geldi.

### Detaylı Karşılaştırma

**Komut Sayısı ve Karmaşıklığı:** CISC işlemcilerde çok sayıda (binlerce) makine komutu bulunur. Bazı komutlar basit işlemler yaparken bazıları oldukça karmaşık işlemler gerçekleştirir. RISC işlemcilerde ise az sayıda (100-200 arası) temel makine komutu bulunur. Bu komutlar daha fazla transistör kullanılarak daha hızlı çalışacak biçimde optimize edilmiştir. CISC'teki karmaşık bir komut, RISC'te birkaç basit komutla karşılanır, ancak toplam işlem süresi genellikle daha kısadır.

**Yazmaç (Register) Sayısı:** CISC işlemcilerde az sayıda (8-16) yazmaç bulunurken, RISC işlemcilerde fazla sayıda (32 veya daha fazla) yazmaç vardır. Az yazmaç olunca aynı değerlerin tekrar tekrar yüklenmesi gerekir ve bu performansı düşürür. Ayrıca CISC'te bazı komutlar yalnızca belirli yazmaçlarla kullanılabilirken (örneğin Intel'de MUL komutu EAX yazmacını kullanmak zorundadır), RISC'te her işlem herhangi bir yazmaçla yapılabilir.

**Komut Uzunluğu:** CISC işlemcilerde komutlar farklı uzunluklarda olabilir. Örneğin Intel x86'da 1 byte'tan 15 byte'a kadar uzunlukta komutlar vardır. RISC işlemcilerde ise tüm makine komutları eşit uzunluktadır. ARM'da her komut tam 4 byte'tır. Bu özellik, işlemcinin komutları bellekten daha hızlı çekip (fetch) anlamlandırmasını sağlar.

**Pipeline Verimliliği:** RISC işlemcilerde pipeline mekanizması CISC'e göre çok daha verimli çalışır. Pipeline, işlemcinin bir makine komutunu çalıştırırken sonraki komutlar üzerinde hazırlık yapması anlamına gelir. RISC'in düzenli yapısı bu mekanizmanın daha iyi işlemesine olanak tanır.

**Load/Store Mimarisi:** RISC işlemciler "load/store" tarzı komut kullanır. Belleğe erişen komutlarla aritmetik/mantıksal işlem yapan komutlar birbirinden tamamen ayrıdır. ADD, SUB gibi komutların tüm operandları yazmaç olmak zorundadır. CISC'te ise bir komut hem belleğe erişebilir hem de işlem yapabilir.

**Güç Tüketimi:** RISC işlemciler yukarıda belirtilen tasarım prensiplerinden dolayı toplamda daha az güç harcar. Bu özellik mobil cihazlarda ve gömülü sistemlerde tercih edilmelerinin en önemli nedenidir.

**Komut Süreleri:** RISC işlemcilerde makine komutlarının çalışma süreleri birbirine çok yakındır (genellikle 1 cycle). CISC'te ise komutlar arasında büyük süre farklılıkları olabilir.

---

## 🔷 ARM Tarihçesi ve Terminoloji

ARM işlemcilerinin tarihi, 1980'li yılların başında "Acorn Computer" isimli İngiliz firmasına dayanmaktadır. Bu firma "BBC Micro" isimli 64K'lık ev bilgisayarlarını üretiyordu ve bu bilgisayarlarda Rockwell'in 8 bitlik 6502 işlemcileri kullanılıyordu.

Firma daha sonra Berkeley RISC projesinden etkilenerek kendi RISC işlemcilerini tasarlamaya başladı ve ilk ARM modelleri ortaya çıktı. 1990 yılında Apple ve VLSI Technology şirketleriyle ortaklık kurularak ARM firması resmen kuruldu. ARM kısaltması başlangıçta "Acorn RISC Machine" anlamına geliyordu, firma kurulduktan sonra "Advanced RISC Machine" olarak değiştirildi.

ARM bir **tasarım firmasıdır** ve fabrikalara sahip değildir. Yaptığı tasarımları lisanslayarak üretici firmalara satar. Apple, Qualcomm, Samsung gibi şirketler en kapsamlı lisanslara sahip olup kendi özelleştirilmiş ARM işlemcilerini tasarlayabilmektedir.

### ARM Terminolojisi

ARM dünyasında çalışmak için bazı özel terimleri bilmek gerekir:

**Core:** Belirli bir mikroişlemci tasarımını ifade eder. Bu tasarım lisans sahibi üretici firmalar tarafından fiziksel çiplere dönüştürülür.

**Cortex:** Bir grup core'un ve işlemciyle ilişkili çeşitli birimlerin (önbellek, FPU, vb.) bir araya getirilmesiyle oluşan yapı. "Cortex" terimi ARM11'den sonra kullanılmaya başlanmıştır. Core ve Cortex terimleri donanımsal mimariyi belirtir.

**Version (ARMv):** Komut kümesi mimarisini (ISA - Instruction Set Architecture) belirtir. Yazılımsal mimariyi tanımlar. Farklı Cortex'ler aynı versiyon numarasını kullanabilir. Örneğin Cortex-A53 ve Cortex-A72 ikisi de ARMv8-A versiyonunu kullanır.

### ARM Profilleri

ARM dünyasında üç mimari profil bulunmaktadır. Profil isimleri Cortex adlarında ve versiyon numaralarında tire (-) karakterinden sonra A, R veya M harfleriyle belirtilir:

**A Profili (Application):** Yüksek performans gerektiren uygulamalar için tasarlanmıştır. Linux, Android gibi masaüstü işletim sistemlerinin çalıştırılabileceği, kişisel bilgisayar olarak kullanılabilecek işlemcileri kapsar. Raspberry Pi ve BeagleBone Black'teki işlemciler A profilidir. Biz bu kursta A profili işlemcilerle çalışacağız.

**R Profili (Realtime):** Gerçek zamanlı uygulamalar için tasarlanmış, düşük gecikme süresi gerektiren sistemlerde kullanılır. Otomotiv ve endüstriyel kontrol sistemlerinde tercih edilir. A profiline benzer ancak daha seyrek kullanılır.

**M Profili (Microcontroller):** ARM'ın mikrodenetleyici olarak kullanılan işlemcilerini kapsar. Düşük güç tüketimi ve düşük maliyet hedeflenir. Bu işlemciler Linux işletim sistemini çalıştıramaz; genellikle bare-metal programlarla veya RTOS ile kullanılır. STM32, nRF serisi gibi popüler mikrodenetleyiciler M profilidir.

### ARM Versiyonları

ARM'ın en önemli iki versiyonu ARMv7 ve ARMv8'dir:

| Versiyon | Bit | Açıklama | Örnekler |
|----------|-----|----------|----------|
| ARMv7-A | 32-bit | Yaygın 32-bit mimari | Cortex-A8 (BBB), Cortex-A7 |
| ARMv8-A | 64-bit | 64-bit ve 32-bit uyumluluk | Cortex-A53 (RPi3), Cortex-A72 (RPi4) |
| ARMv8.2-A | 64-bit | Gelişmiş 64-bit | Cortex-A76 (RPi5) |

ARMv8 işlemciler hem 32-bit hem de 64-bit modda çalışabilir. Bu özellik geriye dönük uyumluluk sağlar.

---

## 🔢 İşlemci Bit Sayısı

İlk mikroişlemciler 8 bit işlemleri yapabiliyordu. Zamanla 16 bit, ardından 32 bit ve bugün ağırlıklı olarak 64 bit işlemciler kullanılmaktadır. Bir işlemcinin N bitlik olmasının anlamları şunlardır:

**İşlem Kapasitesi:** N bitlik bir işlemci, tek bir makine komutuyla N bit üzerinde işlem yapabilir. 32 bitlik bir işlemci tek komutla 32 bitlik iki sayıyı toplayabilir.

**Adreslenebilir Bellek:** N bitlik bir işlemci tipik olarak 2^N boyutunda fiziksel RAM'i adresleyebilir. 32 bit işlemci maksimum 4 GB RAM kullanabilir. 64 bit işlemci teorik olarak 16 exabyte adresleyebilir ancak pratik limitler vardır.

**Veri Transfer Genişliği:** N bitlik işlemcide CPU ile RAM arasında tek seferde N bit veri transfer edilebilir.

8 bitten 16 bite ve 16 bitten 32 bite geçişte belirgin hızlanmalar yaşandı. Ancak 32 bitten 64 bite geçişte hızlanma o kadar dramatik olmadı çünkü 64 bitlik işlemler günlük kullanımda o kadar yoğun değildir. 64 bitin asıl avantajı 4 GB üzeri RAM kullanabilmektir. Örneğin 8 GB RAM'li Raspberry Pi 5'te 64-bit işletim sistemi kullanmak RAM'in tamamından faydalanmayı sağlar; 32-bit sistem yalnızca 4 GB kullanabilir.

---

## ⏱️ Gerçek Zamanlı Sistemler

Gömülü sistemlerin önemli bir bölümü gerçek zamanlı olaylarla ilişkilidir. Bu sistemlerde işlemlerin yalnızca doğru sonuç vermesi yetmez, bu sonucu belirli bir zaman dilimi içinde vermesi de kritiktir.

### Hard Real-Time (Katı Gerçek Zamanlı)

Katı gerçek zamanlı sistemlerde belirlenen sürenin (deadline) aşılması kabul edilemez; aşılması durumunda sistem başarısız sayılır. Bu başarısızlık hayati tehlike veya ciddi maddi hasar yaratabilir.

**Örnekler:**
- Hava yastığı tetikleme sistemleri (milisaniyeler içinde çalışmalı)
- ABS fren sistemleri
- Kalp pilleri
- Uçak otopilot sistemleri
- Endüstriyel robot kontrol sistemleri

### Soft Real-Time (Yumuşak Gerçek Zamanlı)

Yumuşak gerçek zamanlı sistemlerde sürenin aşılması felaket yaratmaz, ancak performans ve kullanıcı deneyimi olumsuz etkilenir. Sistem çalışmaya devam eder.

**Örnekler:**
- Video oynatıcılar (kare atlaması olabilir)
- VoIP sistemleri (ses kalitesi düşer)
- Ağ paket işleme
- Oyun konsolları

---

## 📦 SBC Karşılaştırması

### Raspberry Pi Serisi

Raspberry Pi, özellikle az gelişmiş ülkelerde düşük maliyetli bilgisayar oluşturmayı hedefleyen bir projedir. Klavye, fare ve monitör takılarak tam teşekküllü bir kişisel bilgisayar olarak kullanılabilir.

| Model | SoC | CPU | RAM | Bit | Fiyat |
|-------|-----|-----|-----|-----|-------|
| **Pi 3B+** | BCM2837 | Cortex-A53 1.4GHz | 1GB | 32/64 | ~$35 |
| **Pi 4B** | BCM2711 | Cortex-A72 1.5GHz | 2-8GB | 32/64 | ~$55+ |
| **Pi 5** | BCM2712 | Cortex-A76 2.4GHz | 4-8GB | 32/64 | ~$60+ |

Raspberry Pi 5 donanımı:
- BCM2712 SoC ve ayrı RAM çipi
- 4 USB soketi (2x USB 2.0, 2x USB 3.0)
- Gigabit Ethernet
- 40 pinlik GPIO header
- 2x micro HDMI çıkışı
- ON/OFF düğmesi
- micro SD kart yuvası

### BeagleBone Black

BeagleBone Black, özellikle GPIO yoğun projeler ve endüstriyel uygulamalar için tercih edilen bir SBC'dir. Daha fazla analog giriş pinine ve PRU (Programmable Real-time Unit) özelliğine sahiptir.

| Model | SoC | CPU | RAM | eMMC | Bit |
|-------|-----|-----|-----|------|-----|
| **BBB** | AM335x | Cortex-A8 1GHz | 512MB | 4GB | 32 |
| **BBB Wireless** | AM335x | Cortex-A8 1GHz | 512MB | 4GB | 32 |

BeagleBone Black donanımı:
- Texas Instruments AM335x SoC
- 4GB on-board eMMC flash (işletim sistemi hazır yüklü)
- 2x46 pinlik GPIO header (P8 ve P9)
- USB host ve client portları
- Ethernet
- micro HDMI
- micro SD kart yuvası

### Hangisi Ne Zaman?

| Kullanım Senaryosu | Önerilen Platform |
|--------------------|-------------------|
| Genel amaç, masaüstü kullanım | Raspberry Pi 4/5 |
| GPIO yoğun projeler | BeagleBone Black |
| Analog giriş gerektiren projeler | BeagleBone Black |
| Düşük maliyetli prototip | Raspberry Pi Zero |
| Endüstriyel uygulama | BeagleBone Industrial |
| AI/ML projeleri | Raspberry Pi 5 / Jetson |
| Gerçek zamanlı kontrol | BeagleBone + PRU |

---

## 🖥️ Gömülü Linux vs Desktop Linux

| Özellik | Desktop Linux | Gömülü Linux |
|---------|---------------|--------------|
| **Boyut** | GB seviyesi | MB seviyesi (minimal) |
| **Boot Süresi** | 10-60 saniye | Milisaniye-birkaç saniye |
| **RAM** | 4-64 GB | 256MB-4GB |
| **Depolama** | SSD/HDD (TB) | Flash (MB-GB) |
| **UI** | Genellikle var (GNOME, KDE) | Opsiyonel/yok |
| **Paket Yönetimi** | Tam (apt, yum, pacman) | Minimal veya yok |
| **Kernel** | Generic (tüm donanımlar) | Özelleştirilmiş (hedef donanım) |
| **Güncelleme** | Online | Firmware yükseltme |

---

## 💻 Temel Komutlar

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
cat /proc/meminfo     # Detaylı bellek

# Disk bilgisi
df -h                 # Disk kullanımı
lsblk                 # Blok cihazlar

# Sistem yükü
uptime                # Çalışma süresi
top / htop            # Canlı işlem izleme
```

---

## 🔧 Örnek Kod

```c
/**
 * @file hello_embedded.c
 * @brief Gömülü sistem bilgisi programı
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    char hostname[256];
    
    printf("=====================================\n");
    printf("   Merhaba Gömülü Linux Dünyası!     \n");
    printf("=====================================\n\n");
    
    if (gethostname(hostname, sizeof(hostname)) == 0)
        printf("Hostname: %s\n", hostname);
    
    printf("\n--- Mimari Bilgisi ---\n");
    #if defined(__aarch64__)
        printf("Mimari: ARM 64-bit (AArch64)\n");
    #elif defined(__arm__)
        printf("Mimari: ARM 32-bit\n");
    #elif defined(__x86_64__)
        printf("Mimari: x86_64\n");
    #else
        printf("Mimari: Bilinmiyor\n");
    #endif
    
    printf("\n--- Sistem Bilgisi ---\n");
    system("uname -a");
    
    printf("\n--- CPU ---\n");
    system("cat /proc/cpuinfo | grep -E 'model name|Hardware' | head -2");
    
    printf("\n--- Bellek ---\n");
    system("free -h | head -2");
    
    return 0;
}
```

**Derleme:**
```bash
# Host için
gcc -o hello hello_embedded.c

# BBB için (cross-compile)
arm-linux-gnueabihf-gcc -o hello hello_embedded.c

# RPi 64-bit için
aarch64-linux-gnu-gcc -o hello hello_embedded.c
```

---

## 🎯 Pratik Ödevler

1. Raspberry Pi veya BeagleBone Black'e Linux işletim sistemi kur
2. SSH ile uzaktan bağlan
3. `uname -a` ve `/proc/cpuinfo` ile sistem bilgilerini incele
4. `hello_embedded.c` programını derle ve çalıştır
5. ARM versiyonunu ve Cortex modelini belirle

---

## 📚 Kaynaklar

- [ARM Developer Documentation](https://developer.arm.com/documentation)
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [BeagleBoard Documentation](https://docs.beagleboard.org/)
- [Bootlin Embedded Linux Training](https://bootlin.com/training/)
- [The Linux Kernel Documentation](https://docs.kernel.org/)

---

*Sonraki ders: Geliştirme Ortamı Kurulumu*
