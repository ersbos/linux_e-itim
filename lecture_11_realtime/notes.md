# Ders 11: Gerçek Zamanlı Linux (Real-Time Linux)

> **Konu:** PREEMPT_RT, Gerçek Zamanlı Scheduling

---

## 📋 Bu Derste İşlenenler

- [x] Real-time kavramları
- [x] Linux ve real-time
- [x] PREEMPT_RT patch
- [x] Real-time scheduling
- [x] Latency ölçümü

---

## ⏱️ Gerçek Zamanlı Sistem Nedir?

**Real-time system** = Sonucun doğruluğu kadar **zamanlamasının** da kritik olduğu sistem.

### Türler

| Tür | Açıklama | Örnek |
|-----|----------|-------|
| **Hard Real-Time** | Deadline aşımı = felaket | ABS, hava yastığı |
| **Soft Real-Time** | Deadline aşımı = kalite düşüşü | Video streaming |
| **Firm Real-Time** | Geç sonuç değersiz ama hasar yok | Finansal işlem |

### Temel Metrikler

| Metrik | Açıklama |
|--------|----------|
| **Latency** | Olaydan yanıta geçen süre |
| **Jitter** | Latency varyasyonu |
| **Determinism** | Öngörülebilirlik |
| **Deadline** | Son tarih |

---

## 🐧 Linux ve Real-Time

### Standart Linux Sorunları

- Preemption sınırlamaları
- Interrupt latency yüksek
- Priority inversion
- Öngörülemeyen lock süreleri

### Çözümler

| Yaklaşım | Açıklama |
|----------|----------|
| **PREEMPT_RT** | Mainline Linux + RT patch |
| **Xenomai** | Dual-kernel yaklaşımı |
| **RTAI** | Real-Time Application Interface |

---

## ⚡ PREEMPT_RT Patch

### PREEMPT_RT Nedir?

Linux kernel'ı fully preemptible yapan patch seti.

### Sağladığı İyileştirmeler

- Threaded interrupts
- Priority inheritance
- High-resolution timers
- Preemptible critical sections

### Kurulum

```bash
# Kernel kaynak kodu
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.tar.xz

# RT patch
wget https://cdn.kernel.org/pub/linux/kernel/projects/rt/6.1/patch-6.1-rt1.patch.xz

# Patch uygula
cd linux-6.1
xzcat ../patch-6.1-rt1.patch.xz | patch -p1

# Konfigürasyon
make menuconfig
# → General setup → Preemption Model → Fully Preemptible Kernel (RT)

# Derleme
make -j$(nproc)
make modules_install
make install
```

### Konfigürasyon Seçenekleri

```
General setup --->
    Preemption Model --->
        (X) Fully Preemptible Kernel (RT)
    Timers subsystem --->
        [*] High Resolution Timer Support
```

---

## 📊 Real-Time Scheduling

### Linux Scheduling Policies

| Policy | Öncelik | Açıklama |
|--------|---------|----------|
| `SCHED_FIFO` | 1-99 | First-In-First-Out (preemptible) |
| `SCHED_RR` | 1-99 | Round-Robin (time-sliced) |
| `SCHED_DEADLINE` | - | Earliest Deadline First |
| `SCHED_OTHER` | 0 | Normal (CFS) |

### RT Öncelik Ayarlama

```bash
# Mevcut policy
chrt -p $$

# SCHED_FIFO ile çalıştır (öncelik 50)
sudo chrt -f 50 ./my_rt_program

# SCHED_RR ile çalıştır
sudo chrt -r 50 ./my_rt_program

# Çalışan process'in önceliğini değiştir
sudo chrt -f -p 50 <PID>
```

### C ile RT Scheduling

```c
#include <sched.h>
#include <stdio.h>

int main(void)
{
    struct sched_param param;
    
    /* Öncelik 80 */
    param.sched_priority = 80;
    
    /* SCHED_FIFO ayarla */
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
        perror("sched_setscheduler");
        return 1;
    }
    
    printf("RT scheduling aktif!\n");
    
    /* Kritik iş yap */
    while (1) {
        /* Real-time task */
    }
    
    return 0;
}
```

### Memory Locking

```c
#include <sys/mman.h>

int main(void)
{
    /* Tüm belleği kilitle (sayfalama engelle) */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        perror("mlockall");
        return 1;
    }
    
    /* RT işlemler */
    
    return 0;
}
```

---

## 📏 Latency Ölçümü

### cyclictest

```bash
# Kurulum
sudo apt install rt-tests

# Test çalıştır (5 dakika)
sudo cyclictest -l50000 -m -S -p90 -i200 -h400 -q

# Sonuç örneği:
# T: 0 ( 1234) P:90 I:200 C:50000 Min:1 Act:3 Avg:2 Max:15
```

### Parametreler

| Parametre | Açıklama |
|-----------|----------|
| `-l` | Loop sayısı |
| `-m` | Memory lock |
| `-S` | Per-CPU thread |
| `-p` | Priority |
| `-i` | Interval (µs) |
| `-h` | Histogram |

### Sonuç Yorumlama

| Metrik | İyi Değer |
|--------|-----------|
| **Max latency** | < 50 µs (RT kernel) |
| **Avg latency** | < 10 µs |

---

## 🛠️ RT Uygulama Best Practices

```c
/* rt_best_practices.c */

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/mman.h>

int main(void)
{
    struct sched_param param;
    
    /* 1. Belleği kilitle */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        perror("mlockall");
        exit(1);
    }
    
    /* 2. RT scheduling ayarla */
    param.sched_priority = 80;
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
        perror("sched_setscheduler");
        exit(1);
    }
    
    /* 3. CPU affinity (opsiyonel) */
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  /* CPU 0'a bağla */
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
    
    /* 4. Stack prefault */
    unsigned char dummy[8192];
    memset(dummy, 0, sizeof(dummy));
    
    printf("RT uygulama hazir!\n");
    
    /* Real-time döngü */
    while (1) {
        /* Kritik iş */
    }
    
    return 0;
}
```

---

## 🎯 Pratik

1. `cyclictest` ile standart kernel latency ölç
2. PREEMPT_RT patch'i uygula ve derle
3. RT kernel ile latency karşılaştır
4. SCHED_FIFO ile program çalıştır

---

*Sonraki ders: Debugging ve Profiling*
