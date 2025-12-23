# Ders 11: Gerçek Zamanlı Linux (Real-Time Linux)

> **Konu:** PREEMPT_RT, Gerçek Zamanlı Scheduling ve Zamanlama
> **Kaynak:** C ve Sistem Programcıları Derneği - Gömülü Linux Sistemleri Kursu

---

## 📋 Bu Derste İşlenenler

- [x] Gerçek zamanlı sistem kavramları
- [x] Hard vs Soft real-time
- [x] Linux ve real-time sınırlamaları
- [x] PREEMPT_RT patch ve kurulum
- [x] Real-time scheduling politikaları
- [x] Zamanlama mekanizmaları (jiffies, HZ, timer)
- [x] Latency ölçümü ve optimizasyon
- [x] RT uygulama best practices

---

## ⏱️ Gerçek Zamanlı Sistem Nedir?

Gömülü sistemlerin önemli bir bölümü **gerçek zamanlı (real-time)** olaylarla ilişkilidir. Bu sistemlerin bir bölümü dış dünyadaki değişimlere karşı belirli bir süre içinde yanıt oluşturmaya çalışmaktadır. Gerçek zamanlı bir sistemde sonucun doğruluğu kadar **zamanlamasının** da kritik olduğu anlaşılmalıdır.

Örneğin bir gömülü sistem ortamdaki ısı belli bir kritik düzeye geldiğinde bir işlemi başlatabilir. Ya da bir kalp pili kalp ritmi bozulduğunda kalbe uyarılar göndererek ritim bozukluğunu düzeltmeye çalışabilir. Hava araçlarındaki gömülü sistemler o anki hava şartlarına göre otomatik kumanda sistemi işlevini görür. Bu sistemlerin hepsinde **zamanlama kritik öneme sahiptir**.

### Hard Real-Time vs Soft Real-Time

Gerçek zamanlı sistemler kendi aralarında iki kategoriye ayrılır:

**Katı Gerçek Zamanlı (Hard Real-Time):** Bu sistemlerde deadline (son tarih) aşımı **felaket** anlamına gelir. Sistemin yanıt süresi garanti edilmelidir ve aşım durumunda ciddi hasarlar oluşabilir.

| Örnek | Açıklama |
|-------|----------|
| Araç ABS sistemi | Fren anında 10ms içinde tepki verilmeli |
| Hava yastığı | Çarpışma tespitinden <20ms içinde açılmalı |
| Kalp pili | Ritim düzensizliğinde anında müdahale |
| Endüstriyel robot | Hassas hareket zamanlaması |
| Nükleer santral kontrolü | Kritik eşik aşımında anında durdurma |

**Gevşek Gerçek Zamanlı (Soft Real-Time):** Bu sistemlerde deadline aşımı **kalite düşüşü** anlamına gelir ancak sistem çalışmaya devam edebilir. Arada kalan gecikmeler tolere edilebilir.

| Örnek | Açıklama |
|-------|----------|
| Video streaming | Bir kare gecikmesi kabul edilebilir |
| Ses işleme | Küçük kesintiler fark edilebilir |
| Oyun motorları | FPS düşüşü yaşanabilir |
| Yazıcı kuyruğu | Geç basım kabul edilebilir |

### Temel Metrikler

| Metrik | Açıklama | Örnek Değer |
|--------|----------|-------------|
| **Latency** | Olaydan yanıta geçen süre | 10µs - 10ms |
| **Jitter** | Latency varyasyonu (tutarsızlık) | <5µs |
| **Determinism** | Öngörülebilirlik derecesi | %99.99 |
| **Deadline** | Maksimum kabul edilebilir yanıt süresi | 1ms |
| **WCET** | En kötü durum çalışma süresi | 500µs |

---

## 🐧 Linux ve Real-Time

### Standart Linux'un Sınırlamaları

Masaüstü Linux sistemleri **katı gerçek zamanlı (hard real-time)** işlemler için uygun değildir. Ancak bazı **gevşek gerçek zamanlı (soft real-time)** işlemlere olanak sağlayabilmektedir. Standart Linux çekirdeğinin real-time uygulamalar için bazı temel sorunları vardır:

**1. Preemption Sınırlamaları:** Standart çekirdekte kernel modu kod çalışırken preemption (kesme) yapılamaz. Uzun süren kernel işlemleri sırasında yüksek öncelikli bir görev beklemek zorunda kalabilir.

**2. Interrupt Latency:** Donanım kesmeleri geldiğinde işlenmeleri zaman alabilir. Özellikle uzun interrupt handler'lar diğer işlemleri geciktirir.

**3. Priority Inversion:** Düşük öncelikli bir görev kritik bir kaynağı tutarken, yüksek öncelikli görev beklemek zorunda kalabilir. Bu durum beklenmeyen gecikmelere yol açar.

**4. Öngörülemeyen Lock Süreleri:** Spinlock ve mutex gibi senkronizasyon mekanizmalarının bekletme süreleri öngörülemez olabilir.

**5. Paging ve Sayfa Hataları:** Sayfa hatası (page fault) oluştuğunda disk I/O gerekebilir, bu da milisaniyeler süren gecikmeler oluşturur.

### Real-Time Çözümleri

| Yaklaşım | Açıklama | Latency |
|----------|----------|---------|
| **PREEMPT_RT** | Mainline Linux + RT patch | 10-100µs |
| **Xenomai** | Dual-kernel (RT co-kernel) | 1-10µs |
| **RTAI** | Real-Time Application Interface | 1-10µs |
| **VxWorks** | Ticari RTOS | <1µs |
| **FreeRTOS** | Bare-metal RTOS | <1µs |

---

## ⚡ PREEMPT_RT Patch

### PREEMPT_RT Nedir?

PREEMPT_RT, Linux çekirdeğini **tamamen preemptible (kesintiye uğratılabilir)** hale getiren bir patch setidir. Bu patch seti sayesinde standart Linux gevşek real-time iş yüklerini çok daha iyi karşılayabilir duruma gelir.

**PREEMPT_RT Sağladığı İyileştirmeler:**

1. **Threaded Interrupts:** Interrupt handler'lar kernel thread olarak çalıştırılır, böylece önceliklendirilebilir hale gelir.

2. **Priority Inheritance:** Bir mutex'i tutan düşük öncelikli thread, bekleyen yüksek öncelikli thread'in önceliğini miras alır.

3. **High-Resolution Timers:** Milisaniye yerine mikrosaniye çözünürlüklü timer'lar kullanılabilir.

4. **Preemptible Critical Sections:** Spinlock'lar sleeping lock'lara dönüştürülür ve preemption mümkün hale gelir.

5. **Deterministic Memory Allocation:** Bellek ayırma işlemlerinde determinizm artırılır.

### PREEMPT_RT Kurulum

```bash
# 1. Kernel kaynak kodunu indir
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.tar.xz
tar -xf linux-6.1.tar.xz

# 2. RT patch'i indir
wget https://cdn.kernel.org/pub/linux/kernel/projects/rt/6.1/patch-6.1-rt1.patch.xz

# 3. Patch'i uygula
cd linux-6.1
xzcat ../patch-6.1-rt1.patch.xz | patch -p1

# 4. Konfigürasyon
make menuconfig
```

### Kernel Konfigürasyonu

menuconfig'de şu seçenekleri etkinleştirin:

```
General setup --->
    Preemption Model --->
        ( ) No Forced Preemption (Server)
        ( ) Voluntary Kernel Preemption (Desktop)
        ( ) Preemptible Kernel (Low-Latency Desktop)
        (X) Fully Preemptible Kernel (RT)    ← Bunu seçin

Timers subsystem --->
    [*] High Resolution Timer Support

Power management and ACPI options --->
    CPU Frequency scaling --->
        [ ] CPU Frequency scaling    ← Devre dışı bırakın (opsiyonel)
```

**Derleme ve Kurulum:**
```bash
# Derleme
make -j$(nproc)

# Modül kurulumu
sudo make modules_install

# Kernel kurulumu
sudo make install

# Grub güncelle ve reboot
sudo update-grub
sudo reboot
```

---

## ⏰ Linux Zamanlama Mekanizmaları

### Timer Donanımı

Linux sistemlerinde zamanlama işlemleri iki temel mekanizma ile gerçekleştirilir:

**1. Periyodik Donanım Kesmeleri:** Global timer devresi periyodik kesmeler oluşturur. Bu kesmeler jiffies değişkenini artırır ve scheduling kararlarını tetikler.

| Platform | Timer Donanımı |
|----------|----------------|
| PC (eski) | Intel 8254 PIT |
| PC (yeni) | HPET (High Precision Event Timer) |
| Multi-core | APIC Timer (yerel timer) |
| ARM | ARM Generic Timer |

**2. Time Stamp Counter (TSC):** Modern işlemcilerde yüksek çözünürlüklü sayaç bulunur. Kısa süreli zamanlama için kullanılır.

### jiffies ve HZ

Linux çekirdeğinde global timer kesmesi her oluştuğunda **jiffies** isimli değişken 1 artırılır. Bu değişken sistem açıldığından bu yana kaç timer tick'i geçtiğini sayar.

```c
#include <linux/jiffies.h>

/* jiffies: Sistem açıldığından beri geçen tick sayısı */
unsigned long j = jiffies;

/* 64-bit versiyon (32-bit sistemlerde de güvenli) */
u64 j64 = get_jiffies_64();

/* 1 saniye = HZ tick */
unsigned long one_second = jiffies + HZ;

/* 500ms = HZ/2 tick */
unsigned long half_second = jiffies + HZ/2;

/* msecs_to_jiffies: Milisaniyeyi jiffies'e çevir */
unsigned long timeout = jiffies + msecs_to_jiffies(100); /* 100ms */
```

**HZ değeri:** Timer frekansını belirtir. Çekirdek konfigürasyonunda CONFIG_HZ parametresi ile belirlenir.

| HZ Değeri | Periyot | Kullanım |
|-----------|---------|----------|
| 100 | 10ms | Eski çekirdekler, düşük güç |
| 250 | 4ms | Güncel default |
| 1000 | 1ms | Düşük latency uygulamaları |

**Timer bilgilerini görüntüleme:**
```bash
# Timer listesi
cat /proc/timer_list

# Clock kaynakları
cat /sys/devices/system/clocksource/clocksource0/available_clocksource
cat /sys/devices/system/clocksource/clocksource0/current_clocksource

# HZ değeri (yaklaşık - çekirdek yapısına göre)
grep CONFIG_HZ /boot/config-$(uname -r)
```

---

## 📊 Real-Time Scheduling Politikaları

Linux'ta farklı scheduling (programlama) politikaları bulunur. Real-time uygulamalar için özel politikalar mevcuttur.

### Scheduling Politikaları

| Policy | Öncelik | Açıklama |
|--------|---------|----------|
| `SCHED_FIFO` | 1-99 | First-In-First-Out, preemptible |
| `SCHED_RR` | 1-99 | Round-Robin, time-sliced |
| `SCHED_DEADLINE` | - | Earliest Deadline First |
| `SCHED_OTHER` | 0 | Normal (CFS - Completely Fair Scheduler) |
| `SCHED_BATCH` | 0 | Throughput odaklı batch işler |
| `SCHED_IDLE` | 0 | En düşük öncelik |

**SCHED_FIFO:** En basit real-time policy. Aynı öncelikteki görevler sırayla çalışır. Yüksek öncelikli görev çalışır duruma geldiğinde düşük öncelikli hemen preempt edilir.

**SCHED_RR:** SCHED_FIFO'ya benzer ancak aynı öncelikteki görevler zaman dilimi (time quantum) ile sırayla çalıştırılır.

**SCHED_DEADLINE:** Linux 3.14 ile eklenen en gelişmiş real-time policy. Her görev için runtime, deadline ve period parametreleri tanımlanır.

### chrt Komutu ile Scheduling

```bash
# Mevcut process'in policy'sini görüntüle
chrt -p $$

# SCHED_FIFO ile öncelik 50'de çalıştır
sudo chrt -f 50 ./my_rt_program

# SCHED_RR ile çalıştır
sudo chrt -r 50 ./my_rt_program

# Çalışan process'in önceliğini değiştir
sudo chrt -f -p 80 <PID>

# SCHED_DEADLINE (runtime, deadline, period in nanoseconds)
sudo chrt -d --sched-runtime 10000000 --sched-deadline 30000000 \
    --sched-period 30000000 0 ./deadline_task
```

### C ile Real-Time Scheduling

```c
/* rt_scheduling.c - Real-time scheduling ayarlama */

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <string.h>

void exit_sys(const char *msg);

int main(void)
{
    struct sched_param param;
    int policy;

    /* Mevcut policy'yi al */
    policy = sched_getscheduler(0);
    printf("Mevcut policy: %d\n", policy);

    /* Öncelik aralığını sorgula */
    int min_prio = sched_get_priority_min(SCHED_FIFO);
    int max_prio = sched_get_priority_max(SCHED_FIFO);
    printf("SCHED_FIFO öncelik aralığı: %d - %d\n", min_prio, max_prio);

    /* Öncelik 80 ayarla */
    param.sched_priority = 80;

    /* SCHED_FIFO policy'si ayarla */
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0)
        exit_sys("sched_setscheduler");

    printf("SCHED_FIFO aktif, öncelik: %d\n", param.sched_priority);

    /* Real-time görev */
    while (1) {
        /* Kritik zaman gereksinimli iş */
        /* ... */
    }

    return 0;
}

void exit_sys(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
```

---

## 🔒 Memory Locking

Real-time uygulamalarda sayfa hataları (page fault) beklenmeyen gecikmelere neden olabilir. Bunun önlenmesi için bellek kilitleme (memory locking) kullanılır.

```c
/* memory_lock.c - Bellek kilitleme */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

int main(void)
{
    /* Tüm mevcut ve gelecek sayfaları kilitle */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        fprintf(stderr, "mlockall: %s\n", strerror(errno));
        fprintf(stderr, "Root yetkisi gerekebilir veya ulimit ayarı\n");
        return 1;
    }

    printf("Tüm bellek RAM'de kilitlendi\n");

    /* Belirli bir adresi kilitle */
    char buffer[4096];
    if (mlock(buffer, sizeof(buffer)) < 0) {
        perror("mlock");
    }

    /* Real-time işlemler... */

    /* Kilidi kaldır (opsiyonel, program çıkışında otomatik) */
    munlockall();

    return 0;
}
```

**mlockall Flags:**

| Flag | Açıklama |
|------|----------|
| `MCL_CURRENT` | Mevcut tüm sayfaları kilitle |
| `MCL_FUTURE` | Gelecekte allocate edilecek sayfaları da kilitle |
| `MCL_ONFAULT` | Sayfa erişildiğinde kilitle (Linux 4.4+) |

---

## 📏 Latency Ölçümü

### cyclictest

cyclictest, Linux'ta real-time latency ölçümü için standart araçtır.

```bash
# Kurulum
sudo apt install rt-tests

# Basit test (5 dakika)
sudo cyclictest -l50000 -m -Sp90 -i200 -h400 -q

# Detaylı test
sudo cyclictest \
    --loops=100000 \        # 100000 döngü
    --mlockall \            # Bellek kilitle
    --smp \                 # Her CPU için thread
    --priority=90 \         # Öncelik 90
    --interval=200 \        # 200µs aralık
    --histogram=400 \       # 400µs'e kadar histogram
    --quiet                 # Sadece sonuç göster
```

**Örnek Çıktı:**
```
T: 0 ( 1234) P:90 I:200 C: 50000 Min:    1 Act:    3 Avg:    2 Max:   15
T: 1 ( 1235) P:90 I:200 C: 50000 Min:    1 Act:    2 Avg:    2 Max:   12
```

| Alan | Açıklama |
|------|----------|
| T | Thread numarası |
| P | Öncelik |
| I | Interval (µs) |
| C | Tamamlanan döngü |
| Min | Minimum latency (µs) |
| Act | Son ölçüm (µs) |
| Avg | Ortalama latency (µs) |
| Max | Maksimum latency (µs) |

### Beklenen Değerler

| Kernel Tipi | Max Latency | Açıklama |
|-------------|-------------|----------|
| Standart | 100µs - 10ms | Öngörülemez |
| PREEMPT_RT | 10-50µs | Soft RT için uygun |
| Xenomai | 1-10µs | Hard RT için uygun |

---

## 🛠️ Real-Time Uygulama Best Practices

```c
/* rt_best_practices.c - RT uygulama şablonu */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>

#define RT_PRIORITY 80
#define STACK_PREFAULT_SIZE (8 * 1024)

void exit_sys(const char *msg);

int main(void)
{
    struct sched_param param;
    cpu_set_t cpuset;

    printf("Real-time uygulama başlatılıyor...\n");

    /* 1. Belleği kilitle (page fault önle) */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        perror("mlockall (root gerekebilir)");
        /* Devam et, kritik değilse */
    }

    /* 2. RT scheduling ayarla */
    param.sched_priority = RT_PRIORITY;
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
        perror("sched_setscheduler (root gerekebilir)");
        /* Normal öncelikle devam et */
    }

    /* 3. CPU affinity (opsiyonel - belirli CPU'ya bağla) */
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  /* CPU 0'a bağla */
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) < 0) {
        perror("sched_setaffinity");
    }

    /* 4. Stack prefault (stack page fault önle) */
    {
        unsigned char dummy[STACK_PREFAULT_SIZE];
        memset(dummy, 0, sizeof(dummy));
    }

    /* 5. Heap prefault (malloc kullanılacaksa önceden ayır) */
    void *heap_buffer = malloc(64 * 1024);
    if (heap_buffer) {
        memset(heap_buffer, 0, 64 * 1024);
    }

    printf("RT uygulama hazır!\n");
    printf("Priority: %d, CPU: 0\n", RT_PRIORITY);

    /* Real-time ana döngü */
    while (1) {
        /* 
         * Kritik zaman gereksinimli işlemler
         * 
         * YAPILMAMASI GEREKENLER:
         * - printf() veya dosya I/O (bloke edebilir)
         * - malloc()/free() (lock kullanır)
         * - Sistem çağrıları (bloke edebilir)
         * 
         * YAPILABİLECEKLER:
         * - Memory-mapped I/O
         * - Önceden allocate edilmiş buffer'lar
         * - Lock-free algoritmalar
         */
    }

    free(heap_buffer);
    return 0;
}

void exit_sys(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
```

### RT Uygulama Kuralları

> [!IMPORTANT]
> **Real-time döngü içinde YAPILMAMASI gerekenler:**
> - `printf()`, `fprintf()` - Buffer flushing bloke edebilir
> - `malloc()`, `free()` - Heap lock'ları
> - Dosya I/O - Disk erişimi
> - Network I/O - Beklenmeyen gecikme
> - `sleep()`, `usleep()` - Preemption

> [!TIP]
> **Real-time performansı artırmak için:**
> - Tüm buffer'ları önceden allocate edin
> - Memory-mapped I/O kullanın
> - Lock-free veya wait-free algoritmalar tercih edin
> - CPU affinity ile izolasyon sağlayın
> - `isolcpus` kernel parametresi ile CPU ayırın

---

## 🎯 Pratik Ödevler

1. `cyclictest` ile standart kernel latency ölçün
2. PREEMPT_RT patch'i uygulayın ve kernel derleyin
3. RT kernel ile latency'yi karşılaştırın
4. SCHED_FIFO ile basit bir program çalıştırın
5. `mlockall` ile bellek kilitleme test edin
6. CPU affinity ile process'i tek CPU'ya bağlayın
7. Best practices şablonunu GPIO uygulamasına uygulayın

---

## 📚 Kaynaklar

- [PREEMPT_RT Wiki](https://wiki.linuxfoundation.org/realtime/start)
- [RT Patches](https://cdn.kernel.org/pub/linux/kernel/projects/rt/)
- [cyclictest Documentation](https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cyclictest/start)
- [Bootlin RT Training](https://bootlin.com/training/preempt-rt/)

---

*Sonraki ders: Debugging ve Profiling*
